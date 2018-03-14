function query() {
    var count = 0;
    var bucket = '`beer-sample`';
    var res1 = new N1qlQuery('select * FROM ' + bucket + ' LIMIT 10');
    var res2 = new N1qlQuery('select * FROM ' + bucket + ' LIMIT 10');
    var res3 = new N1qlQuery('select * FROM ' + bucket + ' LIMIT 10');
    if (res1.isInstance) {
        res1.iter(function (r1) {
            console.log('level1: ' + r1['beer-sample'].name);
            r1 = 'hello';
            function innerQuery() {
                if (res2.isInstance) {
                    res2.iter(function (r2) {
                        console.log('level2: ' + r2['beer-sample'].name);
                        if (res1.isInstance) {
                            res1.iter(function (r3) {
                                ++count;
                                console.log('level3: ' + r3['beer-sample'].name);
                                if (count > 10000) {
                                    return res1.stopIter({
                                        'code': 'return',
                                        'args': '(r1)',
                                        'data': r1
                                    });
                                }
                            });
                            switch (res1.getReturnValue(true)) {
                            case 'return(r1)':
                                return res2.stopIter({
                                    'code': 'return',
                                    'args': 'res1.getReturnValue().data',
                                    'data': res1.getReturnValue().data
                                });
                            }
                        } else {
                            for (var r3 of res1) {
                                ++count;
                                console.log('level3: ' + r3['beer-sample'].name);
                                if (count > 10000) {
                                    return res2.stopIter({
                                        'code': 'return',
                                        'args': '(r1)',
                                        'data': r1
                                    });
                                }
                            }
                        }
                    });
                    switch (res2.getReturnValue(true)) {
                    case 'returnres1.getReturnValue().data':
                        return res2.getReturnValue().data;;
                    case 'return(r1)':
                        return res2.getReturnValue().data;;
                    }
                } else {
                    x:
                        for (var r2 of res2) {
                            console.log('level2: ' + r2['beer-sample'].name);
                            if (res1.isInstance) {
                                res1.iter(function (r3) {
                                    ++count;
                                    console.log('level3: ' + r3['beer-sample'].name);
                                    if (count > 10000) {
                                        return res1.stopIter({
                                            'code': 'return',
                                            'args': '(r1)',
                                            'data': r1
                                        });
                                    }
                                });
                                switch (res1.getReturnValue(true)) {
                                case 'return(r1)':
                                    return res1.getReturnValue().data;
                                }
                            } else {
                                for (var r3 of res1) {
                                    ++count;
                                    console.log('level3: ' + r3['beer-sample'].name);
                                    if (count > 10000) {
                                        return r1;
                                    }
                                }
                            }
                        }
                }
                console.log('res2 metadata:' + JSON.stringify(res2.metadata));
            }
            return res1.stopIter({
                'code': 'return',
                'args': '(innerQuery)',
                'data': innerQuery
            });
        });
        switch (res1.getReturnValue(true)) {
        case 'return(innerQuery)':
            return res1.getReturnValue().data;;
        }
    } else {
        x:
            for (var r1 of res1) {
                console.log('level1: ' + r1['beer-sample'].name);
                r1 = 'hello';
                function innerQuery() {
                    if (res2.isInstance) {
                        res2.iter(function (r2) {
                            console.log('level2: ' + r2['beer-sample'].name);
                            if (res1.isInstance) {
                                res1.iter(function (r3) {
                                    ++count;
                                    console.log('level3: ' + r3['beer-sample'].name);
                                    if (count > 10000) {
                                        return res1.stopIter({
                                            'code': 'return',
                                            'args': '(r1)',
                                            'data': r1
                                        });
                                    }
                                });
                                switch (res1.getReturnValue(true)) {
                                case 'return(r1)':
                                    return res2.stopIter({
                                        'code': 'return',
                                        'args': 'res1.getReturnValue().data',
                                        'data': res1.getReturnValue().data
                                    });
                                }
                            } else {
                                for (var r3 of res1) {
                                    ++count;
                                    console.log('level3: ' + r3['beer-sample'].name);
                                    if (count > 10000) {
                                        return res2.stopIter({
                                            'code': 'return',
                                            'args': '(r1)',
                                            'data': r1
                                        });
                                    }
                                }
                            }
                        });
                        switch (res2.getReturnValue(true)) {
                        case 'returnres1.getReturnValue().data':
                            return res2.getReturnValue().data;;
                        case 'return(r1)':
                            return res2.getReturnValue().data;;
                        }
                    } else {
                        x:
                            for (var r2 of res2) {
                                console.log('level2: ' + r2['beer-sample'].name);
                                if (res1.isInstance) {
                                    res1.iter(function (r3) {
                                        ++count;
                                        console.log('level3: ' + r3['beer-sample'].name);
                                        if (count > 10000) {
                                            return res1.stopIter({
                                                'code': 'return',
                                                'args': '(r1)',
                                                'data': r1
                                            });
                                        }
                                    });
                                    switch (res1.getReturnValue(true)) {
                                    case 'return(r1)':
                                        return res1.getReturnValue().data;
                                    }
                                } else {
                                    for (var r3 of res1) {
                                        ++count;
                                        console.log('level3: ' + r3['beer-sample'].name);
                                        if (count > 10000) {
                                            return r1;
                                        }
                                    }
                                }
                            }
                    }
                    console.log('res2 metadata:' + JSON.stringify(res2.metadata));
                }
                return innerQuery;
            }
    }
    console.log('iterations = ' + count);
}
var row = query();
console.log('query returned:' + JSON.stringify(row()));
function N1qlQuery(query) {
    this.query = query;
    this.metadata = null;
    this.isInstance = true;
    this.iter = iter;
    this.execQuery = execQuery;
    this.stopIter = stopIter;
    this.getReturnValue = getReturnValue;
}
//# sourceMappingURL=file:///Users/gautham/projects/github/n1ql-js/n1ql_js_v3/input1.d.map.json