/**
 * Created by gautham on 06/06/17.
 */
var fs = require('fs'),
    esprima = require('esprima'),
    estraverse = require('estraverse');

var filename = process.argv[2];
var code = fs.readFileSync(filename, 'utf-8');
console.log(getMaxIterDepth(code));

function getMaxIterDepth(code) {
    var ast = esprima.parse(code, {sourceType: 'script'}),
        maxDepth = 0,
        currDepth = 0;
    estraverse.traverse(ast, {
        enter: function (node) {
            if (/ForOfStatement/.test(node.type)) {
                ++currDepth;
                if (currDepth > maxDepth) {
                    maxDepth = currDepth;
                }
            }
        },
        leave: function (node) {
            if (/ForOfStatement/.test(node.type)) {
                --currDepth;
            }
        }
    });

    console.assert(currDepth == 0, 'curr_depth must be 0 after parsing completely');
    return maxDepth;
}