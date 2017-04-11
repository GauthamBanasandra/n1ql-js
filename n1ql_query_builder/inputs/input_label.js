l1:
    loop: {
        for (var row of res)
            while (true)
                l: {
                    break loop;
                    if (true)
                        break l1;
                };
            log(x);
    }
    