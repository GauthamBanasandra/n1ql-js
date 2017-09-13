function OnUpdate(doc, meta) {
    log('document', doc);
    
    var bucket = '`beer-sample`',
        abvLim = 20;
    
    // N1QL queries can be used naturally in the function handler.
    // To use a JavaScript variable in the N1QL query,
    // prefix it with a colon(:) in the N1QL query. For example, ":bucket" as shown below.
    var res = new N1qlQuery(`SELECT brewery_id, name FROM :bucket WHERE type == 'beer' AND abv > :abvLim`);
    
    // We can iterate over the query results just like a JavaScript array.
    // Note that the query results can be iterated over only in a for-of loop.
    for(var row of res) {
        log(row);
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