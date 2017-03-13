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
        var body = escodegen.generate(node.body, {
            comment: true
        });
        return esprima.parse(
            node.right.name +
            '.iter(function (' + (node.left.name ? node.left.name : node.left.declarations[0].id.name) + ')' +
            body + ')'
        ).body[0];
    }

    throw 'Invalid arg ' + mode + ' for get_iter_ast';
}

var fs = require('fs'),
    esprima = require('esprima'),
    estraverse = require('estraverse'),
    escodegen = require('escodegen');

var filename = process.argv[2];
var code = fs.readFileSync(filename, 'utf-8');

// Get the Abstract Syntax Tree (ast) of the input code.
var ast = esprima.parse(code, {
    attachComment: true
});


estraverse.traverse(ast, {
    enter: function (node) {
        // TODO : Handle the case when the source of 'for ... of ...' is of type x.y
        if (/ForOfStatement/.test(node.type) && !node.isVisited) {

            if (!/BlockStatement/.test(node.body.type))
                convert_to_block_stmt(node);

            var iterAst = get_iter_ast(node, 'for_of');
            var ifAst = esprima.parse('if(typeof ' + node.right.name + '=== N1qlQuery){}else{}').body[0];

            var nodeCopy = deep_copy(node);

            ifAst.consequent.body.push(iterAst);
            ifAst.alternate.body.push(nodeCopy);

            estraverse.traverse(nodeCopy, {
                enter: function (node) {
                    if (/ForOfStatement/.test(node.type))
                        node.isVisited = true;
                }
            });
            
            replace_node(node, ifAst);
        }
    }
});
console.log(escodegen.generate(ast, { comment: true }));