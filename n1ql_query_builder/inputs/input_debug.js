var res1 = new N1qlQuery(`select * from \`beer-sample\`;`);
var res2 = new N1qlQuery(`select * from \`beer-sample\`;`);

x:for (var r1 of res1) {
    for (var r2 of res2) {		
        console.log('query 1', r1);
        console.log('query 2', r2);		
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
    }
}