const testFolder = '/Users/gautham/Downloads/json_street_trees/';
const fs = require('fs');
var rows = 0;

fs.readdir(testFolder, (err, files) => {
    files.forEach(file => {
        var jsonFile = require(testFolder + file);
        rows += jsonFile.length;
        let i = 0;
        for (var row of jsonFile) {
            query = 'INSERT INTO default (KEY, VALUE) VALUES ("odwalla-juice' + rows + i + '", ' + JSON.stringify(row) + ')';
            var couchbase = require('couchbase');
            var cluster = new couchbase.Cluster('couchbase://localhost/');
            var bucket = cluster.openBucket('default');
            var n1ql_query = couchbase.N1qlQuery;
            bucket.query(
                n1ql_query.fromString(query),
                function (err, result) {
                    if (err)
                        throw err;

                    console.log(result);
                });
        }
        console.log('rows=' + rows);
    });
});