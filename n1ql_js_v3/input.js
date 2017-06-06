var count = 0;
var res1 = new N1qlQuery(`SELECT * FROM \`beer-sample\` LIMIT 10;`);
var res2 = new N1qlQuery(`SELECT * FROM \`beer-sample\` LIMIT 10;`);
var res3 = new N1qlQuery(`SELECT * FROM \`beer-sample\` LIMIT 10;`);

x:for (var r1 of res1) {
    log('level 1:\t' + r1['beer-sample'].name);
    for (var r2 of res2) {
        log('level 2:\t' + r2['beer-sample'].name);
        try {
            for (var r3 of res3) {
                ++count;
                log('level 3:\t' + r3['beer-sample'].name);
                log(y);
            }
        } catch (e) {
            log(e.message);
        }
    }
}

log('iterations:\t' + count);