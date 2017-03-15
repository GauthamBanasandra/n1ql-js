function Stack() {
    var stack = [];

    this.stackCopy = stack;
    this.push = function (item) {
        stack.push(item);
    }

    this.pop = function () {
        if (stack.length == 0)
            throw 'Stack underflow exception';
        return stack.pop();
    }

    this.peek = function () {
        var size = this.getSize();
        if (size >= 0) {
            return stack[size - 1];
        }
        return;
    }

    this.getSize = function () {
        return stack.length;
    }

    // debug.
    this.printAll = function () {
        for (var item of stack) {
            console.log(item);
            console.log();
        }
    }
}

function LoopModifier(modifier) {
    this.modType = modifier;
    var ancestorStack = new Stack();
    this.stackIndex = -1;

    this.stackCopy = ancestorStack.stackCopy;

    var associations = new Set();
    switch (this.modType) {
        case 'break':
            associations.add('DoWhileStatement');
            associations.add('ForStatement');
            associations.add('ForInStatement');
            associations.add('ForOfStatement');
            associations.add('SwitchStatement');
            associations.add('WhileStatement');
            associations.add('LabeledStatement');
            break;
        default:
            throw 'Invalid modifier';
    }

    this.checkAssoc = function (nodeType) {
        return associations.has(nodeType);
    }

    this.pushIfAssoc = function (node) {
        if (this.checkAssoc(node.type)) {
            switch (this.modType) {
                case 'break':
                    node.breakStackIndex = this.stackIndex;
                    break;
                default:
                    throw 'Invalid modifier type';
            }
            ancestorStack.push(node);
        }
    }

    this.popIfAssoc = function () {
        if (ancestorStack.getSize() > 0) {
            switch (this.modType) {
                case 'break':
                    if (this.stackIndex == ancestorStack.peek().breakStackIndex) {
                        return ancestorStack.pop();
                    }
                    break;
                default:
                    throw 'Invalid modifier type';
            }
        }
    }

    this.getSize = function () {
        return ancestorStack.getSize();
    }

    this.isReplaceReq = function () {
        if (ancestorStack.getSize() > 0) {
            switch (this.modType) {
                case 'break':
                    if (ancestorStack.peek().type === 'ForOfStatement') {
                        return true;
                    }
                    break;

                default:
                    break;
            }
        }
    }
    // debug.
    this.printAll = function () {
        ancestorStack.printAll();
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
    return JSON.parse(JSON.stringify(node));
}

function get_iter_ast(forOfNode, mode) {
    var nodeCopy=deep_copy(forOfNode);

    if (mode === 'for_of') {
        estraverse.traverse(nodeCopy, {
            enter: function (node) {
                ++breakMod.stackIndex;

                if (node.isGen) {
                    return;
                }
                breakMod.pushIfAssoc(node);

                switch (node.type) {
                    case 'BreakStatement':
                        if (breakMod.isReplaceReq()) {
                            const stopIterAst = esprima.parse(nodeCopy.right.name + '.stopIter();').body[0];
                            replace_node(node, stopIterAst);
                        }
                        break;

                    default:
                        break;
                }
            },
            leave: function (node) {
                breakMod.popIfAssoc();
                --breakMod.stackIndex;
            }
        });
        var iter = esprima.parse(
            forOfNode.right.name +
            '.iter(function (' + (forOfNode.left.name ? forOfNode.left.name : forOfNode.left.declarations[0].id.name) + '){});'
        ).body[0];
        iter.expression.arguments[0].body = nodeCopy.body;

        return iter;
    }

    throw 'Invalid arg ' + mode + ' for get_iter_ast';
}

// Returns iterator consturct with dynamic type checking.
function get_iter_compatible_ast(forOfNode) {
    // Make a copy of the 'for ... of ...' loop.
    var nodeCopy = deep_copy(forOfNode);
    
    // Iterator AST.
    var iterAst = get_iter_ast(nodeCopy, 'for_of');

    // 'if ... else ...' with dynamic type checking.
    var ifElseAst = esprima.parse('if(' + forOfNode.right.name + ' instanceof N1qlQuery){}else{}').body[0];
    // Push the iterator AST into 'if' block.
    ifElseAst.consequent.body.push(iterAst);
    // Push the user-written 'for ... of ...' loop into 'else' block.
    ifElseAst.alternate.body.push(forOfNode);

    estraverse.traverse(ifElseAst, {
        enter: function (node) {
            node.isGen = true;
        }
    });

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
var breakMod = new LoopModifier('break');

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
        
            replace_node(node, deep_copy(iterAst));
        }
    }
});

console.log(escodegen.generate(ast, {
    comment: true
}));