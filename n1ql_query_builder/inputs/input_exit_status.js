function query() {
    x:for (var row of res1)
        for (var row of res2) {
            if (row.length > 5)
                break x;
            docTimer('hello');
            return row;
        }
}