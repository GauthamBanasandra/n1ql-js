for (var r of res1) {
    x:for (var r of res2)
        for (var r of res3)
            if (true)
                continue x;
    break;
}