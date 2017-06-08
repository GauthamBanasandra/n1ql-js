/**
 * Created by gautham on 06/06/17.
 */
var fs = require('fs'),
    esprima = require('esprima'),
    estraverse = require('estraverse');

var filename = process.argv[2];
var code = fs.readFileSync(filename, 'utf-8');
console.log(get_max_iter_depth(code));

function get_max_iter_depth(code) {
    var ast = esprima.parse(code, {sourceType: 'script'}),
        max_depth = 0,
        curr_depth = 0;
    estraverse.traverse(ast, {
        enter: function (node) {
            if (/ForOfStatement/.test(node.type)) {
                ++curr_depth;
                if (curr_depth > max_depth) {
                    max_depth = curr_depth;
                }
            }
        },
        leave: function (node) {
            if (/ForOfStatement/.test(node.type)) {
                --curr_depth;
            }
        }
    });

    console.assert(curr_depth == 0, 'curr_depth must be 0 after parsing completely');
    return max_depth;
}