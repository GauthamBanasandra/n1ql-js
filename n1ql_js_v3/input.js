function query() {
    var count = 0;
    var bucket = '`beer-sample`';
    var res1 = new N1qlQuery(`SELECT * FROM :bucket LIMIT 10;`);
    var res2 = new N1qlQuery(`SELECT * FROM :bucket LIMIT 10;`);
    var res3 = new N1qlQuery(`SELECT * FROM :bucket LIMIT 10;`);

    x: for (var r1 of res1) {
        log('level1: ' + r1['beer-sample'].name);

        r1 = 'hello';

        function innerQuery() {
            x: for (var r2 of res2) {
                log('level2: ' + r2['beer-sample'].name);
                for (var r3 of res1) {
                    ++count;
                    log('level3: ' + r3['beer-sample'].name);
                    if (count > 10) {
                        return r1;
                    }
                }
            }
        }

        return innerQuery;
    }

    log('iterations = ' + count);
}

var row = query();
log('query returned:' + JSON.stringify(row()));
