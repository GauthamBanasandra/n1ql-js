/**
 * Created by gautham on 02/02/17.
 */
var couchbase = require('couchbase');
var cluster = new couchbase.Cluster('couchbase://0.0.0.0:8091?detailed_errcodes=1');
var bucket = cluster.openBucket('beer-sample');
var n1ql_query = couchbase.N1qlQuery;
bucket.query(
    n1ql_query.fromString('SELECT * FROM `beer-sample`;'),
    function (err, result)
    {
        if(err)
            throw err;

        console.log(result);
    });