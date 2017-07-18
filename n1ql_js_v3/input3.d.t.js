var res1 = new N1qlQuery('select * from `beer-sample` LIMIT 10;');
var res2 = new N1qlQuery('select * from `beer-sample` LIMIT 10;');
if (res1.isInstance) {
    res1.iter(function (r1) {
        console.log('query1:', r1);
    });
} else {
    x:
        for (var r1 of res1) {
            console.log('query1:', r1);
        }
}
function N1qlQuery(query) {
    this.query = query;
    this.metadata = null;
    this.isInstance = true;
    this.iter = iter;
    this.execQuery = execQuery;
    this.stopIter = stopIter;
    this.getReturnValue = getReturnValue;
}
//# sourceMappingURL=file:///Users/gautham/projects/github/n1ql-js/n1ql_js_v3/input3.d.map.json