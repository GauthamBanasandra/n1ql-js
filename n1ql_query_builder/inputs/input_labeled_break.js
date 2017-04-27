var res2 = [1, 2, 4, 5];
x:
    if (res2.isInstance) {
        res2.x = res2.iter(function (r) {
            for (var i = 0; i < 10; ++i) {
                return;
            }
        });
    } else {
        y:for (var r of res2) {
            for (var i = 0; i < 10; ++i) {
                continue y;
            }
        }
    }