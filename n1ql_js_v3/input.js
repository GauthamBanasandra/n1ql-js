function query() {
    var bucket = '`beer-sample`';
    var res = new N1qlQuery(`SELECT * FROM :bucket LIMIT 10;`);

    for (var r of res) {
        log(r['beer-sample']);
    }
}

query();
