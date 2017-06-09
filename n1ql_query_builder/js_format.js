/**
 * Created by gautham on 14/02/17.
 */
var fs = require('fs'),
    esprima = require('esprima'),
    escodegen = require('escodegen');

var filename = process.argv[2];
var code = fs.readFileSync(filename, 'utf-8');

// Get the Abstract Syntax Tree (ast) of the input code.
var ast = esprima.parse(code, {attachComment: true});

console.log(escodegen.generate(ast, {comment: true}));