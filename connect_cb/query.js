/**
 * Created by gautham on 02/02/17.
 */
var couchbase = require('couchbase');
var cluster = new couchbase.Cluster('couchbase://localhost/');
var bucket = cluster.openBucket('default');
var n1ql_query = couchbase.N1qlQuery;
bucket.query(
    n1ql_query.fromString('SELECT * FROM default;'),
    function (err, result)
    {
        if(err)
            throw err;

        console.log(result);
    });