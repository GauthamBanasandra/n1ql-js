function N1qlQuery(queryStr) {
    this.query = queryStr;
    this.iter = function (callback) {
        callback(query);
    }
    var rows = [];
    for (var i = 0; i < 100; ++i) {
        rows.push(i);
    }
    this.rows = rows;
}

function OnUpdate() {
    var res = new N1qlQuery('select * from default;');
    try {
        for (var row of res.rows) {
            console.log(row);
        }
    } catch (error) {
    }
}

OnUpdate();