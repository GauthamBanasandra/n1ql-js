/**
 * Created by gautham on 04/05/17.
 */
const SIZE = 1000;
function generate_data(size) {
    var data = [];
    for (var i = 0; i < size; ++i) {
        data.push(Math.random());
    }

    return data;
}

function N1qlQuery(size) {
    this.isInstance = true;
    this.data = generate_data(size);
    this.stopSignal = false;
    this.metaData = {};
    this.iter = function (callback) {
        for (var item in this.data) {
            if (this.stopSignal) {
                this.stopSignal = false;
                return this.metaData;
            }
            callback(item);
        }
    };
    this.stopIter = function (stopData) {
        this.stopSignal = true;
        this.metaData = stopData;
    }
}

var res1 = generate_data(SIZE),
    res2 = generate_data(SIZE),
    res3 = generate_data(SIZE),
    sum = 0,
    iterations = 0;

var start = Date.now();
for (var item1 of res1) {
    for (var item2 of res2)
        for (var item3 of res3) {
            sum += item1 + item2 + item3;
            ++iterations;
            break;
        }
}
var end = Date.now();

var normalCodeTime = end - start;
console.log('normal code execution time:', normalCodeTime);
console.log('iterations =', iterations);

res1 = new N1qlQuery(SIZE);
res2 = new N1qlQuery(SIZE);
res3 = new N1qlQuery(SIZE);
iterations = sum = 0;

start = Date.now();
if (res1.isInstance) {
    res1.x = res1.iter(function (item1) {
        if (res2.isInstance) {
            res2.x = res2.iter(function (item2) {
                if (res3.isInstance) {
                    res3.x = res3.iter(function (item3) {
                        sum += item1 + item2 + item3;
                        ++iterations;
                        return res3.stopIter({'code': 'break'});
                    });
                } else {
                    for (var item3 of res3) {
                        sum += item1 + item2 + item3;
                        ++iterations;
                        break;
                    }
                }
            });
        } else {
            for (var item2 of res2) {
                if (res3.isInstance) {
                    res3.x = res3.iter(function (item3) {
                        sum += item1 + item2 + item3;
                        ++iterations;
                        return res3.stopIter({'code': 'break'});
                    });
                } else {
                    for (var item3 of res3) {
                        sum += item1 + item2 + item3;
                        ++iterations;
                        break;
                    }
                }
            }
        }
    });
} else {
    for (var item1 of res1) {
        if (res2.isInstance) {
            res2.x = res2.iter(function (item2) {
                if (res3.isInstance) {
                    res3.x = res3.iter(function (item3) {
                        sum += item1 + item2 + item3;
                        ++iterations;
                        return res3.stopIter({'code': 'break'});
                    });
                } else {
                    for (var item3 of res3) {
                        sum += item1 + item2 + item3;
                        ++iterations;
                        break;
                    }
                }
            });
        } else {
            for (var item2 of res2) {
                if (res3.isInstance) {
                    res3.x = res3.iter(function (item3) {
                        sum += item1 + item2 + item3;
                        ++iterations;
                        return res3.stopIter({'code': 'break'});
                    });
                } else {
                    for (var item3 of res3) {
                        sum += item1 + item2 + item3;
                        ++iterations;
                        break;
                    }
                }
            }
        }
    }
}

end = Date.now();
var transpiledCodeTime = end - start;
console.log('transpiled code execution time:', transpiledCodeTime);
console.log('iterations =', iterations);
console.log('transpiledCodeTime : normalCodeTime =', transpiledCodeTime/normalCodeTime)