var res1 = SELECT * from `beer-sample` LIMIT 10;
var res2 = SELECT * from `beer-sample` LIMIT 10;

x: for (var r1 of res1) {
    console.log('query1:', r1)
    for (var r2 of res2) {
        console.log('query2:', r2);      
    }
}
