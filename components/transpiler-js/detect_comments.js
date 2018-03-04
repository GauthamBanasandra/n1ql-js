/**
 * Created by gautham on 24/01/17.
 */
var fs = require('fs'),
    esprima = require('esprima'),
    estraverse = require('estraverse'),
    escodegen = require('escodegen');

var filename = process.argv[2];
var code = fs.readFileSync(filename, 'utf-8');
var ast = esprima.parse(code, {attachComment: true});
var scopes = [];
var inst = {};

estraverse.traverse(ast, {
    enter: function (node)
    {
        if (/Function/.test(node.type))
        {
            inst = {name: node.id ? node.id.name : 'anonymous', vars: []};
            scopes.push(inst);
        }
        if (/Program/.test(node.type))
        {
            inst = {name: 'global', vars: []};
            scopes.push(inst);
        }

        if (node.type === 'AssignmentExpression')
            scopes[scopes.length - 1].vars.push(node.left.name);

        if (node.type === 'VariableDeclaration')
            for (var i in node.declarations)
                scopes[scopes.length - 1].vars.push(node.declarations[i].id.name);

        if (node.type === 'ArrayExpression')
        {
            console.log('found a query:', node.leadingComments[0].value, '\n');

            var n1ql = esprima.parse('n1ql("select x from y where z");').body[0].expression;

            Object.keys(node).forEach(function (key)
            {
                delete node[key];
            });

            Object.keys(n1ql).forEach(function (key)
            {
                node[key] = n1ql[key];
            });
        }
    },
    leave: function (node)
    {
        if (/Function/.test(node.type))
        {
            /*var functionName = node.id ? node.id.name : 'anonymous';
            console.log('variables accessible in', functionName);
            for (var i in scopeStack)
                console.log('\t', scopeStack[i].name, scopeStack[i].vars);*/
            scopes.pop();
        }
    }
});
console.log(escodegen.generate(ast, {comment: true}));