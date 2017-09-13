function OnUpdate(doc, meta) {
    var bucket = '`beer-sample`', lim = 10;
    var res = new N1qlQuery('select * FROM ' + bucket + ' WHERE LIMIT ' + lim + '');
    if (res.isInstance) {
        res.iter(function (row) {
            log(row);
        });
    } else {
        for (var row of res) {
            log(row);
        }
    }
}