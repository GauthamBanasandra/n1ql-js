function query() {
    var res1 = new N1qlQuery('select * from `beer-sample` LIMIT 10;');
    var res2 = new N1qlQuery('select * from `beer-sample` LIMIT 10;');
    if (res1.isInstance) {
        res1.iter(function (row) {
            if (res2.isInstance) {
                res2.iter(function (row) {
                    return res2.stopIter({
                        'code': 'return',
                        'args': '(x)',
                        'data': x
                    });
                });
                switch (res2.getReturnValue(true)) {
                case 'return(x)':
                    return res1.stopIter({
                        'code': 'return',
                        'args': 'res2.getReturnValue().data',
                        'data': res2.getReturnValue().data
                    });
                }
            } else {
                for (var row of res2) {
                    return res1.stopIter({
                        'code': 'return',
                        'args': '(x)',
                        'data': x
                    });
                }
            }
        });
        switch (res1.getReturnValue(true)) {
        case 'returnres2.getReturnValue().data':
            return res1.getReturnValue().data;;
        case 'return(x)':
            return res1.getReturnValue().data;;
        }
    } else {
        for (var row of res1) {
            if (res2.isInstance) {
                res2.iter(function (row) {
                    return res2.stopIter({
                        'code': 'return',
                        'args': '(x)',
                        'data': x
                    });
                });
                switch (res2.getReturnValue(true)) {
                case 'return(x)':
                    return res2.getReturnValue().data;
                }
            } else {
                for (var row of res2) {
                    return x;
                }
            }
        }
    }
}
function N1qlQuery(query) {
    var stopSignal = false;
    this.isInstance = true;
    this.query = query;
    var stopMeta = {};
    this.iter = function (callback) {
        for (var i = 0; i < 100; i++) {
            if (!stopSignal) {
                callback(i);
            } else {
                stopSignal = false;
                return stopMeta;
            }
        }
    };
    this.stopIter = function (meta) {
        stopMeta = meta;
        stopSignal = true;
    };
    this.getReturnValue = function (concat) {
        var returnValue;
        if (concat) {
            returnValue = stopMeta.code + stopMeta.args;
        } else {
            returnValue = stopMeta;
        }
        return returnValue;
    };
}