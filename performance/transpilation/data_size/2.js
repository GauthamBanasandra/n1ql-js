/**
 * Created by gautham on 04/05/17.
 */
const SIZE = 10000;
function generate_data(size) {
    var data = [];
    for (var i = 0; i < size; ++i) {
        data.push(Math.random());
    }

    return data;
}

var res1 = generate_data(SIZE),
    sum = 0,
    iterations = 0;

console.time('normal code');

for (var item1 of res1) {
    sum += item1;
    ++iterations;
}

console.timeEnd('normal code');
console.log('iterations=', iterations);

function N1qlQuery(size) {
    this.isInstance = true;
    this.data = generate_data(size);
    this.iter = function (callback) {
        for (var item in this.data) {
            callback(item);
        }
    };
}

res1 = new N1qlQuery(SIZE);
iterations = sum = 0;

console.time('transpiled code');
if (res1.isInstance) {
    res1.x = res1.iter(function (item1) {
        sum += item1;
        ++iterations;
    });
} else {
    for (var item1 of res1) {
        sum += item1;
        ++iterations;
    }
}
console.timeEnd('transpiled code');
console.log('iterations=', iterations);
