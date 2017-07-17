var res1 = new N1qlQuery('select * from `beer-sample` LIMIT 10;');
var res2 = new N1qlQuery('select * from `beer-sample` LIMIT 10;');
x:
    while (true) {
        if (res1.isInstance) {
            res1.iter(function (r1) {
                console.log('query1:', r1);
                return res1.stopIter({
                    'code': 'labeled_break',
                    'args': 'x'
                });
            });
            switch (res1.getReturnValue(true)) {
            case 'labeled_breakx':
                break x;
            }
        } else {
            for (var r1 of res1) {
                console.log('query1:', r1);
                break x;
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