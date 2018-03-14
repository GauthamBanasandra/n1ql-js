function get_break_assoc(nodeType) {
    return /DoWhileStatement/.test(nodeType) ||
        /ForStatement/.test(nodeType) ||
        /ForInStatement/.test(nodeType) ||
        /ForOfStatement/.test(nodeType) ||
        /SwitchStatement/.test(nodeType) ||
        /WhileStatement/.test(nodeType) ||
        /LabeledStatement/.test(nodeType);
}

function get_nodetype_assoc(nodeType) {
    var assocStack = { breakAssoc: false, continueAssoc: false, returnAssoc: false };

    assocStack.breakAssoc = get_break_assoc(nodeType);

    return assocStack;
}

function push_stack(node) {
    var nodeAssoc = get_nodetype_assoc(node.type);

    if (nodeAssoc.breakAssoc) {
        breakAssocStack.push(node);
    }
}

function pop_stack(node) {
    var breakAssocStackSize = breakAssocStack.length;

    // console.assert(breakAssocStackSize > 0);

    if (index == breakAssocStackSize - 1) {
        breakAssocStack.pop();
    }
}

var fs = require('fs'),
    esprima = require('esprima'),
    estraverse = require('estraverse'),
    escodegen = require('escodegen');

var filename = process.argv[2];
var code = fs.readFileSync(filename, 'utf-8');

// Get the Abstract Syntax Tree (ast) of the input code.
var ast = esprima.parse(code, {
    attachComment: true,
    sourceType: 'script'
});

var index = -1;
var breakAssocStack = [];

estraverse.traverse(ast, {
    enter: function (node) {
        ++index;
        node.stackIndex = index;
        push_stack(node);

        if (/BreakStatement/.test(node.type)) {
            for (var n of breakAssocStack) {
                console.log(n);
                console.log();
            }
        }
    },
    leave: function (node) {
        console.assert(typeof node.stackIndex !== 'undefined');
        pop_stack(node);
        --index;
    }
});