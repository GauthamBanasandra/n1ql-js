function query() {
    var res1 = new N1qlQuery('SELECT * FROM `beer-sample` LIMIT 2;');
    var res2 = new N1qlQuery('SELECT * FROM `beer-sample` LIMIT 3;');
    if (res1.isInstance) {
        res1.x = res1.iter(function (r1) {
            if (res2.isInstance) {
                res2.x = res2.iter(function (r2) {
                    log('r1: ' + r1['beer-sample'].name);
                    log('r2: ' + r2['beer-sample'].name);
                    return res2.stopIter({
                        'code': 'return',
                        'args': '(null)',
                        'data': null
                    });
                });
                switch (res2.x.code + res2.x.args) {
                    case 'return(null)':
                        return res1.stopIter({
                            'code': 'return',
                            'args': 'res2.x.data',
                            'data': res2.x.data
                        });
                }
            } else {
                for (var r2 of res2) {
                    log('r1: ' + r1['beer-sample'].name);
                    log('r2: ' + r2['beer-sample'].name);
                    return res1.stopIter({
                        'code': 'return',
                        'args': '(null)',
                        'data': null
                    });
                }
            }
        });
        switch (res1.x.code + res1.x.args) {
            case 'returnres2.x.data':
                return res1.x.data;;
            case 'return(null)':
                return res1.x.data;;
        }
    } else {
        for (var r1 of res1) {
            if (res2.isInstance) {
                res2.x = res2.iter(function (r2) {
                    log('r1: ' + r1['beer-sample'].name);
                    log('r2: ' + r2['beer-sample'].name);
                    return res2.stopIter({
                        'code': 'return',
                        'args': '(null)',
                        'data': null
                    });
                });
                switch (res2.x.code + res2.x.args) {
                    case 'return(null)':
                        return res2.x.data;
                }
            } else {
                for (var r2 of res2) {
                    log('r1: ' + r1['beer-sample'].name);
                    log('r2: ' + r2['beer-sample'].name);
                    return null;
                }
            }
        }
    }
    return;
}