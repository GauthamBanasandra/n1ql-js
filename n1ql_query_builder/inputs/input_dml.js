function query() {
    if (res1.isInstance) {
        res1.x = res1.iter(function (r1) {
            return res1.stopIter({
                'code': 'return',
                'args': '(r1)',
                'data': r1
            });
        });
        switch (res1.x.code + res1.x.args) {
            case 'return(r1)':
                return res1.x.data;
        }
    } else {
        for (var r1 of res1) {
            return r1;
        }
    }
}