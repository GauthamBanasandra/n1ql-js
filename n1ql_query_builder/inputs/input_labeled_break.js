var count = 0;

function query() {
    var res1 = new N1qlQuery(`SELECT * FROM \`beer-sample\` LIMIT 10;`);
    var res2 = new N1qlQuery(`SELECT * FROM \`beer-sample\` LIMIT 10;`);

    var list = [];
    try {
        var c1 = 0;
        for (var r1 of res1) {
            ++c1;
            var c2 = 0;
            list.push(r1['beer-sample'].name);
            for (var r2 of res2) {
                ++c2;
                ++count;
                log('level2:\t' + r2['beer-sample'].name);
                throw 'an exception';
            }
            if (c2 != 1) {
                log('\t\t\t\tc2 = ' + c2);
                log('');
            }
        }
        if (c1 != 10) {
            log('c1 = ' + c1);
            log('');
        }
        var stop = getTime();
        log('time = ' + (stop - start));
    } catch (e) {
    }
    log('');
    for (var item of list) {
        log('level1:\t' + item);
    }
}
query();
log('iterations = ' + count);
