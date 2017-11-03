function OnUpdate(doc, meta) {
    var lim = 10,
    	count = 0;

		// Timeout
    var res1 = SELECT * FROM `beer-sample` LIMIT :lim;
    for(var row1 of res1) {
    	var docId = meta.id + (++count);
			var ins = INSERT INTO dst_bucket (KEY, VALUE) VALUES (:docId, 'Hello world');
			ins.execQuery();
			while(true){
			}
    }
}

function OnDelete(meta) {
}