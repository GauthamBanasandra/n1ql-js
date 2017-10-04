function OnUpdate(doc, meta) {
    var bucket = '`beer-sample`',
        abvLim = 20;

    var res = new N1qlQuery('meRge  brewery_id, name FROM :bucket WHERE type == \'beer\' AND abv > ":abvLim"');

    for(var row of res) {
        log(row);
        helloWorld[row.brewery_id] = row.name;
    }
}

function OnDelete(doc) {
}