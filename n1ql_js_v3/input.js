var res1 = new N1qlQuery('SELECT * FROM `beer-sample` LIMIT 2;');
var res2 = new N1qlQuery('SELECT * FROM `beer-sample` LIMIT 3;');

res1.iter(function(r1) {
    res2.iter(function(r2) {
        var res3 = new N1qlQuery('SELECT * FROM `beer-sample` LIMIT 2;');
        for (var r3 of res3.execQuery()) {
            log('level1: ' + r1['beer-sample'].name);
            log('level2: ' + r2['beer-sample'].name);
            log('level3: ' + r3['beer-sample'].name);
            log('');
        }
    });
});
