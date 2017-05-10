/**
 * Created by gautham on 10/05/17.
 */

function json_parse_js(input) {
    return JSON.parse(input);
}

var str = '{"key":"This is key", "value":"This is value"}';

function run() {
    // Measuring time taken for json_parse_js
    var start = getTime();  // getTime returns time in micro-seconds
    json_parse_js(str);
    var end = getTime();
    var js_time = end - start;

    // Measuring time taken for json_parse_v8 - exposed from C++
    start = getTime();
    json_parse_v8(str);
    end = getTime();
    var v8_time = end - start;

    // Ratio of the two times
    return v8_time / js_time;
}

function mean(data) {
    var sum = 0;
    for (var number of data) {
        sum += number;
    }
    return sum / data.length;
}

function median(data) {
    data = JSON.parse(JSON.stringify(data));
    data.sort(function (a, b) {
        return a - b;
    });

    var mid = Math.floor(data.length / 2);
    return data.length % 2 ? data[mid] : (data[mid - 1] + data[mid]) / 2;
}

const RUNS = 20;
var runs = [];
for (var i = 0; i < RUNS; ++i) {
    var runtime = run();
    log(runtime);
    runs.push(runtime);
}

runs.sort(function (a, b) {
    return a - b;
});
log('mean =', mean(runs));
log('median =', median(runs));
