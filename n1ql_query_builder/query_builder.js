/**
 * Created by gautham on 25/01/17.
 */
var fs = require('fs'),
    esprima = require('esprima'),
    estraverse = require('estraverse'),
    escodegen = require('escodegen');

var filename = process.argv[2];
var code = fs.readFileSync(filename, 'utf-8');
var ast = esprima.parse(code, {attachComment: true});
var scopes = [];
var scopeIdx = -1;

function linq_query(query)
{
    /*var spawn = require('child_process').spawnSync('/Users/gautham/projects/linq_js/linq_parser/src/a.out',
     [],
     {input: (query.length + 10) + ' ' + query + '\n'});
     console.log(spawn.stdout.toString().length);
     return spawn.stdout.toString();*/

    return '\'' + query + '\'';
}

function traverse(mode)
{
    var inst = {};

    if (mode === 'gen_code')
        scopeIdx = -1;

    estraverse.traverse(ast, {
        enter: function (node)
        {
            if (/Function/.test(node.type))
            {
                if (mode === 'var_scope')
                {
                    inst = {name: node.id ? node.id.name : 'anonymous', vars: {}};
                    scopes.push(inst);
                    ++scopeIdx;
                }
                else if (mode === 'gen_code')
                    ++scopeIdx;
            }

            if (/Program/.test(node.type))
            {
                if (mode === 'var_scope')
                {
                    inst = {name: 'global', vars: {}};
                    scopes.push(inst);
                    ++scopeIdx;
                }
                else if (mode === 'gen_code')
                    ++scopeIdx;
            }

            if (mode === 'var_scope')
            {
                if (node.type === 'AssignmentExpression')
                    scopes[scopeIdx].vars[node.left.name] = null;

                if (node.type === 'VariableDeclaration')
                    for (var i in node.declarations)
                        scopes[scopeIdx].vars[node.declarations[i].id.name] = null;

                if (node.type === 'FunctionDeclaration')
                    if (node.params && node.params.length > 0)
                        for (var i in node.params)
                            scopes[scopeIdx].vars[node.params[i].name] = null;
            }

            if (mode === 'gen_code')
                if (check_linq_n1ql(node))
                {
                    var varsInScope = get_vars_in_scope(scopeIdx);
                    var query = node.body.innerComments[0].value.trim();
                    var firstWord = query.split(' ')[0];
                    var n1qlQuery = firstWord === 'select' ? build_query(query, varsInScope) : linq_query(query);
                    var n1qlAst = esprima.parse(n1qlQuery).body[0].expression;

                    Object.keys(node).forEach(function (key)
                    {
                        delete node[key];
                    });

                    Object.keys(n1qlAst).forEach(function (key)
                    {
                        node[key] = n1qlAst[key];
                    });
                }
        },
        leave: function (node)
        {
            if (/Function/.test(node.type))
            {
                // Uncomment to display functions and the variables that are active in them.
                /*if (mode === 'var_scope')
                {
                    var functionName = node.id ? node.id.name : 'anonymous';
                    var vars = get_vars_in_scope(stackIdx);

                    console.log(functionName, vars.join(', '));
                }*/

                --scopeIdx;
            }
        }
    });
}

function check_linq_n1ql(node)
{
    return (
        node.type === 'FunctionExpression' &&
        node.id === null &&
        typeof node.params === typeof [] &&
        node.params.length === 0 &&
        node.body.type === 'BlockStatement' &&
        typeof node.body.body === typeof [] &&
        node.body.body.length === 0 &&
        node.body.innerComments.length === 1 &&
        node.body.innerComments[0].type === 'Block'
    );
}

function build_query(query, vars)
{
    for (var i in vars)
    {
        var re = new RegExp('\\b' + vars[i] + '\\b', 'g');
        query = query.replace(re, '" + ' + vars[i] + ' + "');
    }

    return 'n1ql("' + query + '");';
}

function get_vars_in_scope(idx)
{
    var vars = [];

    for (var i = 0; i <= idx; ++i)
        Object.keys(scopes[i].vars).forEach(function (key)
        {
            vars[key] = null;
        });

    return Object.keys(vars);
}

traverse('var_scope');
// Uncomment to display the variables that are active in the global scope.
// console.log('global', get_vars_in_scope(0).join(', '), '\n\n');

traverse('gen_code');

console.log(escodegen.generate(ast, {comment: true}));
