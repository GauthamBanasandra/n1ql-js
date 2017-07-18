function query() {
    var res1 = new N1qlQuery('select * from `beer-sample` LIMIT 10;');
    var res2 = new N1qlQuery('select * from `beer-sample` LIMIT 10;');
    if (res1.isInstance) {
        res1.iter(function (r) {
            if (res2.isInstance) {
                res2.iter(function (r) {
                    return;
                });
            } else {
                x:
                    for (var r of res2) {
                        continue x;
                    }
            }
        });
    } else {
        for (var r of res1) {
            if (res2.isInstance) {
                res2.iter(function (r) {
                    return;
                });
            } else {
                x:
                    for (var r of res2) {
                        continue x;
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