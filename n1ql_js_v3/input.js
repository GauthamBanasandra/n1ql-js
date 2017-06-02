var count = 0;

function query() {
    var res1 = new N1qlQuery(`SELECT * FROM \`beer-sample\` LIMIT 10;`);
    var res2 = new N1qlQuery(`SELECT * FROM \`beer-sample\` LIMIT 10;`);
    var res3 = new N1qlQuery(`SELECT * FROM \`beer-sample\` LIMIT 10;`);

    try {
        var c1 = 0;
        for (var r1 of res1) {
            ++c1;
            var c2 = 0;
            x:for (var r2 of res2) {
                ++c2;
                var c3 = 0;
                for (var r3 of res3) {
                    ++c3;
                    ++count;
                    log('level3:\t' + r3['beer-sample'].name);
                    break;
                }
                if (c3 != 1) {
                    log('\t\t\t\t\t\t\t\tc3 = ' + c3);
                    log('');
                }
            }
            if (c2 != 10) {
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
}
var res = query();
log('iterations = ' + count);
log('query returned:\t' + res);
