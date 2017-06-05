var n1ql = new N1qlQuery(`SELECT * FROM :bucket;`);
x:for (var r1 of res1) {
    for (var r2 of res2)
        break x;
}