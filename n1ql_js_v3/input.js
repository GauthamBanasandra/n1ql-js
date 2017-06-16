function query() {
    var count = 0;
    var bucket = '`beer-sample`';
    var res = new N1qlQuery(`SELECT * FROM :bucket LIMIT 10;`);

    for (var r1 of res) {
        log('level1: ' + r1['beer-sample'].name);
    }
    log('iterations = ' + count);
}

query();
