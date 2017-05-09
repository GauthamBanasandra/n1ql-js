/**
 * Created by gautham on 05/05/17.
 */
let process = require('process');
/*let size = Number(process.argv[2]),
 runs = Number(process.argv[3]);*/
let size = 100000,
    runs = 20;

function generate_data(size) {
    let data = [];
    for (let i = 0; i < size; ++i) {
        data.push(i);
    }

    return data;
}

function run() {
    let res1 = generate_data(size),
        sum = 0,
        normalIterations = 0;

    let start = process.hrtime()[1];
    // Normal code.
    // for (let item1 of res1) {
    //     sum += item1;
    //     ++normalIterations;
    // }
    res1.forEach(function (item1) {
        sum += item1;
        ++normalIterations;
    });
    let end = process.hrtime()[1];

    return end - start;
}

for (let i = 0; i < runs; ++i) {
    let runtime = run();
    console.log(runtime);
}