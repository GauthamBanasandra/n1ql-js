function query() {
    var bucket = '`beer-sample`';
    var res1 = new N1qlQuery('select * from ' + bucket + ' LIMIT 10;');
    var res2 = new N1qlQuery('select * from `beer-sample` LIMIT 10;');
    x:
        while (true)
            if (res3.isInstance) {
                res3.iter(function (r) {
                    if (res3.isInstance) {
                        res3.iter(function (r) {
                            return res3.stopIter({
                                'code': 'return',
                                'args': '(a + b)',
                                'data': a + b
                            });
                        });
                        switch (res3.getReturnValue(true)) {
                        case 'return(a + b)':
                            return res3.stopIter({
                                'code': 'return',
                                'args': 'res3.getReturnValue().data',
                                'data': res3.getReturnValue().data
                            });
                        }
                    } else {
                        for (var r of res3) {
                            return res3.stopIter({
                                'code': 'return',
                                'args': '(a + b)',
                                'data': a + b
                            });
                        }
                    }
                });
                switch (res3.getReturnValue(true)) {
                case 'returnres3.getReturnValue().data':
                    return res3.getReturnValue().data;;
                case 'return(a + b)':
                    return res3.getReturnValue().data;;
                }
            } else {
                for (var r of res3) {
                    if (res3.isInstance) {
                        res3.iter(function (r) {
                            return res3.stopIter({
                                'code': 'return',
                                'args': '(a + b)',
                                'data': a + b
                            });
                        });
                        switch (res3.getReturnValue(true)) {
                        case 'return(a + b)':
                            return res3.getReturnValue().data;
                        }
                    } else {
                        for (var r of res3) {
                            return a + b;
                        }
                    }
                }
            }
}