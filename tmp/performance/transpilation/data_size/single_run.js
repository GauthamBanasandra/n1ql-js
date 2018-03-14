/**
 * Created by gautham on 05/05/17.
 */
function generate_data(size) {
    let data = [];
    for (let i = 0; i < size; ++i) {
        data.push(i);
    }

    return data;
}

function N1qlQuery(size) {
    this.isInstance = true;
    this.data = generate_data(size);
    this.stopSignal = false;
    this.metaData = {};
    this.iter = function (callback) {
        for (let item in this.data) {
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

var size = Number(process.argv[2]);

let res1 = generate_data(size),
    sum = 0,
    normalIterations = 0,
    transpiledIterations = 0;


let start = process.hrtime()[1];
// Normal code.
for (let item1 of res1) {
    sum += item1;
    ++normalIterations;
}
let end = process.hrtime()[1];


let normalCodeTime = end - start;

res1 = new N1qlQuery(size);
transpiledIterations = sum = 0;


start = process.hrtime()[1];
// Transpiled code.
if (res1.isInstance) {
    res1.x = res1.iter(function (item1) {
        sum += item1;
        ++transpiledIterations;
    });
} else {
    for (let item1 of res1) {
        sum += item1;
        ++transpiledIterations;
    }
}
end = process.hrtime()[1];


let transpiledCodeTime = end - start;

if (normalIterations !== transpiledIterations)
    throw 'iterations are not equal';

console.log(transpiledCodeTime / normalCodeTime);