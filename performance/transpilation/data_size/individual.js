/**
 * Created by gautham on 09/05/17.
 */

var size = 100000000;
function generate_data(size) {
    var data = [];
    for (var i = 0; i < size; ++i) {
        data.push(i);
    }

    return data;
}

var res1 = generate_data(size),
    sum = 0,
    normalIterations = 0;

var start = Date.now();
// Normal code.
for (var item1 of res1) {
    sum += item1;
    ++normalIterations;
}
var end = Date.now();

var normalCodeTime = end - start;
console.log(normalCodeTime / 1000.0, 'seconds');