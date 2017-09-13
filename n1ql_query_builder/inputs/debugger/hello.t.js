function OnUpdate(doc, meta) {
    log('document', doc);
    var bucket = '`beer-sample`', abvLim = 20;
    var res = new N1qlQuery('SELECT brewery_id, name FROM ' + bucket + ' WHERE type == \'beer\' AND abv > ' + abvLim + '');
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
function OnDelete(doc) {
}
function N1qlQuery(query) {
    this.query = query;
    this.metadata = null;
    this.isInstance = true;
    this.iter = function (params) {
    };
}