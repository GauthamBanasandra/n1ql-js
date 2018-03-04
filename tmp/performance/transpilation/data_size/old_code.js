/**
 * Created by gautham on 04/05/17.
 */
var process = require('process');

function run(size) {
    function generate_data(size) {
        var data = [];
        for (var i = 0; i < size; ++i) {
            data.push(i);
        }

        return data;
    }

    function N1qlQuery(size) {
        this.isInstance = true;
        this.data = generate_data(size);
        this.stopSignal = false;
        this.metaData = {};
        // Here's where the bottle-neck lies.
        // This is incorrect. Because, in the eventing checkout, we are exposing iter from C++ - https://github.com/couchbase/eventing/blob/master/v8_consumer/src/builtin.js
        // Thus, we need to move this to C++.
        this.iter = function (callback) {
            for (var item in this.data) {
                callback(item);
            }
        };
        this.stopIter = function (stopData) {
            this.stopSignal = true;
            this.metaData = stopData;
        }
    }

    var res1 = generate_data(size),
        sum = 0,
        normalIterations = 0,
        transpiledIterations = 0;


    var start = process.hrtime()[1];
    // Normal code.
    for (var item1 of res1) {
        sum += item1;
        ++normalIterations;
    }
    var end = process.hrtime()[1];


    var normalCodeTime = end - start;

    res1 = new N1qlQuery(size);
    res2 = new N1qlQuery(size);
    res3 = new N1qlQuery(size);
    transpiledIterations = sum = 0;


    start = process.hrtime()[1];
    // Transpiled code.
    if (res1.isInstance) {
        res1.x = res1.iter(function (item1) {
            sum += item1;
            ++transpiledIterations;
        });
    } else {
        for (var item1 of res1) {
            sum += item1;
            ++transpiledIterations;
        }
    }
    end = process.hrtime()[1];


    var transpiledCodeTime = end - start;

    if (normalIterations !== transpiledIterations)
        throw 'iterations are not equal';
    if (transpiledCodeTime < 0 || normalCodeTime < 0)
        return 0;

    return transpiledCodeTime / normalCodeTime;
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

const SIZE = 100000,
    RUNS = 20;
var runs = [];
for (var i = 0; i < RUNS; ++i) {
    var runtime = run(SIZE);
    console.log(runtime);
    runs.push(runtime);
}

runs.sort(function (a, b) {
    return a - b;
});
console.log('mean =', mean(runs));
console.log('median =', median(runs));