function OnUpdate(doc, meta) {
    var bucket = '`beer-sample`',
        abvLim = 20,
        n = 'abcd';

    var res = new N1qlQuery('SELECT * FROM `beer-sample` WHERE name = $n', {namedParams: {n: n}});

    for (var row of res.x) {
        log(row);
    }
}

function OnDelete(doc) {
}