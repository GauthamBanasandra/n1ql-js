var res1 = new N1qlQuery(`SELECT name from \`beer-sample\` LIMIT 10;`);

for(var r1 of res1) {
    for (var r11 of res1) {
        log(r1);
        break;
    }
}