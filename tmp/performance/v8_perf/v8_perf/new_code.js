/**
 * Created by gautham on 04/05/17.
 */

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
        this.metaData = {};
        // The definition for iter is provided by C++.
        this.iter = iter;
        this.stopIter = function (stopData) {
            this.stopSignal = true;
            this.metaData = stopData;
        }
    }

    var res1 = generate_data(size),
        sum = 0,
        normalIterations = 0,
        transpiledIterations = 0;

    var str = '{"key":"This is key", "value":"This is value"}';
    var start = getTime();
    // Normal code.
    for (var item1 of res1) {
        var item1 = JSON.parse(str);
        ++normalIterations;
    }
    var end = getTime();


    var normalCodeTime = end - start;

    res1 = new N1qlQuery(size);
    transpiledIterations = sum = 0;


    start = getTime();
    // Transpiled code.
    if (res1.isInstance) {
        res1.x = res1.iter(function (item1) {
            item1 = JSON.parse(item1);
            ++transpiledIterations;
        });
    } else {
        for (var item1 of res1) {
            sum += item1;
            ++transpiledIterations;
        }
    }
    end = getTime();

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

const SIZE = 5000000,
    RUNS = 20;
var runs = [];
for (var i = 0; i < RUNS; ++i) {
    var runtime = run(SIZE);
    log(runtime);
    runs.push(runtime);
}

runs.sort(function (a, b) {
    return a - b;
});
log('mean =', mean(runs));
log('median =', median(runs));
