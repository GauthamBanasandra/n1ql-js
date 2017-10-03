function query() {
    var count = 0;
    var bucket = '`beer-sample`';
    var res1 = new N1qlQuery(`select * FROM :bucket LIMIT 10`);
    var res2 = new N1qlQuery(`select * FROM :bucket LIMIT 10`);
    var res3 = new N1qlQuery(`select * FROM :bucket LIMIT 10`);

    x: for (var r1 of res1) {
        console.log('level1: ' + r1['beer-sample'].name);

        r1 = 'hello';

        function innerQuery() {
            x: for (var r2 of res2) {
                console.log('level2: ' + r2['beer-sample'].name);
                for (var r3 of res1) {
                    ++count;
                    console.log('level3: ' + r3['beer-sample'].name);
                    if (count > 10000) {
                        return r1;
                    }
                }
            }

                console.log('res2 metadata:' + JSON.stringify(res2.metadata));
        }

        return innerQuery;
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

