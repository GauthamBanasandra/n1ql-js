/**
 * Created by gautham on 02/02/17.
 */
var couchbase = require('couchbase');
var cluster = new couchbase.Cluster('couchbase://localhost/');
var bucket = cluster.openBucket('default');
var n1ql_query = couchbase.N1qlQuery;
bucket.get('user:king_arthur', function (err, result)
{
    if(err)
        throw err;

    console.log(result.value);
});