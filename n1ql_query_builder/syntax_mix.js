/**
 * Created by gautham on 08/06/17.
 */
var fs = require('fs'),
    parser = require('acorn/dist/acorn_loose'),
    escodegen = require('escodegen');

var filename = process.argv[2];
var code = fs.readFileSync(filename, 'utf-8');
var ast = parser.parse_dammit(code);
console.log(escodegen.generate(ast));