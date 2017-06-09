function OnUpdate(doc, meta) {
    log("doc: ", doc, " meta: ", meta);
    var bucket = '`beer-sample`';

    var res1 = SELECT * FROM :bucket LIMIT 10;
    var res2 = SELECT * FROM :bucket LIMIT 10;

    x:for(var r1 of res1) {
        log('level1: ', r1['beer-sample'].name);
        for(var r2 of res2) {
            log('level2: ', r2['beer-sample'].name);
            break x;
        }
    }
}

function OnDelete(msg) {
}