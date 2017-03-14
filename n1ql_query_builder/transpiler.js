function Stack() {
    var stack = [];

    this.push = function (item) {
        stack.push(item);
    }
    this.pop = function () {
        if (stack.length == 0)
            throw 'Stack underflow exception';
        return stack.pop();
    }
    this.getSize = function (params) {
        return stack.length;
    }
}

// Replaces source node with the target node.
function replace_node(source, target) {
    Object.keys(source).forEach(function (key) {
        delete source[key];
    });

    Object.keys(target).forEach(function (key) {
        source[key] = target[key];
    });
}

function convert_to_block_stmt(node) {
    var forBodyAst = {};

    // Make a deep copy of the body.
    Object.keys(node.body).forEach(function (key) {
        forBodyAst[key] = node.body[key];
    });

    // Transform the previous single-line statement into a block.
    node.body.body = [forBodyAst];
    node.body.type = 'BlockStatement';
}

function deep_copy(node) {
    var nodeCopy = {};

    Object.keys(node).forEach(function (key) {
        nodeCopy[key] = node[key];
    });

    return nodeCopy;
}

function get_iter_ast(node, mode) {
    if (mode === 'for_of') {
        var body = deep_copy(node.body);

        var iter = esprima.parse(
            node.right.name +
            '.iter(function (' + (node.left.name ? node.left.name : node.left.declarations[0].id.name) + '){});'
        ).body[0];
        iter.expression.arguments[0].body = body;

        return iter;
    }

    throw 'Invalid arg ' + mode + ' for get_iter_ast';
}

// Returns iterator consturct with dynamic type checking.
function get_iter_compatible_ast(forOfNode) {
    // Iterator AST.
    var iterAst = get_iter_ast(forOfNode, 'for_of');
    // 'if ... else ...' with dynamic type checking.
    var ifElseAst = esprima.parse('if(' + forOfNode.right.name + ' instanceof N1qlQuery){}else{}').body[0];

    // Make a copy of the 'for ... of ...' loop.
    var nodeCopy = deep_copy(forOfNode);

    // Push the iterator AST into 'if' block.
    ifElseAst.consequent.body.push(iterAst);
    // Push the user-written 'for ... of ...' loop into 'else' block.
    ifElseAst.alternate.body.push(nodeCopy);

    // Traverse all the 'for ... of ...' loops in the 'else' block and mark them as visited - so that we don't recursively convert these into iterator constructs.
    estraverse.traverse(nodeCopy, {
        enter: function (node) {
            if (/ForOfStatement/.test(node.type))
                node.isVisited = true;
        }
    });

    return ifElseAst;
}

var fs = require('fs'),
    esprima = require('esprima'),
    estraverse = require('estraverse'),
    escodegen = require('escodegen');

var filename = process.argv[2];
var code = fs.readFileSync(filename, 'utf-8');
var breakAncestors = new Stack();

// Get the Abstract Syntax Tree (ast) of the input code.
var ast = esprima.parse(code, {
    attachComment: true,
    sourceType: 'script'
});

estraverse.traverse(ast, {
    leave: function (node) {
        // TODO : Handle the case when the source of 'for ... of ...' is of type x.y
        // Modifies all the 'for ... of ...' constructs to work with iteration. Takes care to see to it that it visits the node only once.
        if (/ForOfStatement/.test(node.type) && !node.isVisited) {
            if (!/BlockStatement/.test(node.body.type)) {
                convert_to_block_stmt(node);
            }
            var iterAst = get_iter_compatible_ast(node);
            replace_node(node, iterAst);
        }
    }
});

console.log(escodegen.generate(ast, { comment: true }));