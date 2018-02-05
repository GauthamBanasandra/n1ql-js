function OnUpdate(doc, meta) {
    var bucket = '`beer-sample`',
        abvLim = 20,
        n = 'abcd';

    var res = new N1qlQuery('SELECT * FROM `beer-sample` WHERE name = $n', {namedParams: {n: n}});

    var res1 = new N1qlQuery('SELECT * FROM `beer-sample` WHERE name = $n', {namedParams: {n: n}});

    try {
        for (var row of res) {
            log(row);
            for (var row of res1) {
                log(row);
            }
        }
    } catch (e) {
    }
}

function OnDelete(doc) {
}