/**
 * Created by gautham on 24/01/17.
 */
var fs = require('fs'),
    esprima = require('esprima'),
    estraverse = require('estraverse');

var filename = process.argv[2];
var code = fs.readFileSync(filename, 'utf-8');
var ast = esprima.parse(code,{range:true, loc:true, comment:true, tolerant:true});
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
    },
    leave: function (node)
    {
        if (/Function/.test(node.type))
        {
            var functionName = node.id ? node.id.name : 'anonymous';
            console.log('variables accessible in', functionName);
            for (var i in scopes)
                console.log('\t', scopes[i].name, scopes[i].vars);
            scopes.pop();
        }
    }
});

console.log('variables in global', scopes[0].vars);