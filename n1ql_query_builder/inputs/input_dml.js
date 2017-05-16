function query() {
    if (res1.isInstance) {
        res1.x = res1.iter(function (row) {
            if (res2.isInstance) {
                res2.x = res2.iter(function (row) {
                    if (row.length > 5) {
                        return res2.stopIter({
                            'code': 'labeled_break',
                            'args': 'x'
                        });
                    }
                    return res2.stopIter({
                        'code': 'return',
                        'args': '(row)',
                        'data': row
                    });
                });
                switch (res2.x.code + res2.x.args) {
                    case 'labeled_breakx':
                        return res1.stopIter({
                            'code': 'labeled_break',
                            'args': 'x'
                        });
                    case 'return(row)':
                        return res1.stopIter({
                            'code': 'return',
                            'args': 'res2.x.data',
                            'data': res2.x.data
                        });
                }
            } else {
                for (var row of res2) {
                    if (row.length > 5)
                        return res1.stopIter({
                            'code': 'labeled_break',
                            'args': 'x'
                        });
                    return res1.stopIter({
                        'code': 'return',
                        'args': '(row)',
                        'data': row
                    });
                }
            }
        });
        switch (res1.x.code + res1.x.args) {
            case 'returnres2.x.data':
                return res1.x.data;;
            case 'return(row)':
                return res1.x.data;;
        }
    } else {
        x:
            for (var row of res1) {
                if (res2.isInstance) {
                    res2.x = res2.iter(function (row) {
                        if (row.length > 5) {
                            return res2.stopIter({
                                'code': 'labeled_break',
                                'args': 'x'
                            });
                        }
                        return res2.stopIter({
                            'code': 'return',
                            'args': '(row)',
                            'data': row
                        });
                    });
                    switch (res2.x.code + res2.x.args) {
                        case 'labeled_breakx':
                            break x;
                        case 'return(row)':
                            return res2.x.data;
                    }
                } else {
                    for (var row of res2) {
                        if (row.length > 5)
                            break x;
                        return row;
                    }
                }
            }
    }
}
