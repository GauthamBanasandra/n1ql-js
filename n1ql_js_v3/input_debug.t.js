var res1 = new N1qlQuery('select * from `beer-sample` LIMIT 10;');
var res2 = new N1qlQuery('select * from `beer-sample` LIMIT 10;');
if (res1.isInstance) {
    res1.iter(function (r1) {
        console.log('query1:', r1);
        if (res2.isInstance) {
            res2.iter(function (r2) {
                console.log('query2:', r2);
                if (res3.isInstance) {
                    res3.iter(function (r3) {
                        console.log('query3:', r3);
                    });
                } else {
                    for (var r3 of res3) {
                        console.log('query3:', r3);
                    }
                }
            });
        } else {
            for (var r2 of res2) {
                console.log('query2:', r2);
                if (res3.isInstance) {
                    res3.iter(function (r3) {
                        console.log('query3:', r3);
                    });
                } else {
                    for (var r3 of res3) {
                        console.log('query3:', r3);
                    }
                }
            }
        }
    });
} else {
    for (var r1 of res1) {
        console.log('query1:', r1);
        if (res2.isInstance) {
            res2.iter(function (r2) {
                console.log('query2:', r2);
                if (res3.isInstance) {
                    res3.iter(function (r3) {
                        console.log('query3:', r3);
                    });
                } else {
                    for (var r3 of res3) {
                        console.log('query3:', r3);
                    }
                }
            });
        } else {
            for (var r2 of res2) {
                console.log('query2:', r2);
                if (res3.isInstance) {
                    res3.iter(function (r3) {
                        console.log('query3:', r3);
                    });
                } else {
                    for (var r3 of res3) {
                        console.log('query3:', r3);
                    }
                }
            }
        }
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
//# sourceMappingURL=file:///Users/gautham/projects/github/n1ql-js/n1ql_js_v3/input_debug.json