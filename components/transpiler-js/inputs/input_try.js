function query() {
    var count = 0;
    var bucket = '`beer-sample`';
    var res = new N1qlQuery(`SELECT * FROM :bucket LIMIT 10;`);
    var res1 = new N1qlQuery(`SELECT name FROM :bucket LIMIT 10;`);

    x: for (var r3 of res) {
        log('outer level: ' + r3.name);
        try {
            for (var r1 of res) {
                log('level1: ' + r1['beer-sample'].name);
                for (var r2 of res) {
                    ++count;
                    log('level2: ' + r2['beer-sample'].name);
                    break x;
                }
            }
        } catch (e) {
            log(e);
        }
    }
    log('iterations = ' + count);
}

query();
