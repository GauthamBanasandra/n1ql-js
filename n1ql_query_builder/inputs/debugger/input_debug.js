var res1 = new N1qlQuery(`select * from \`beer-sample\` LIMIT 10;`);
var res2 = new N1qlQuery(`select * from \`beer-sample\` LIMIT 10;`);

for (var r1 of res1) {
    console.log('query1:', r1)
    for (var r2 of res2) {
        console.log('query2:', r2);
      	for (var r3 of res3) {
        console.log('query3:', r3);
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
	}
}