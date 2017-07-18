function query() {
    var res1 = new N1qlQuery('select * from `beer-sample` LIMIT 10;');
    var res2 = new N1qlQuery('select * from `beer-sample` LIMIT 10;');
    if (res1.isInstance) {
        res1.iter(function (r) {
            if (res2.isInstance) {
                res2.iter(function (r) {
                    if (res3.isInstance) {
                        res3.iter(function (r) {
                            if (res4.isInstance) {
                                res4.iter(function (r) {
                                    return res4.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'x'
                                    });
                                });
                                switch (res4.getReturnValue(true)) {
                                case 'labeled_breakx':
                                    return res3.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'x'
                                    });
                                }
                            } else {
                                for (var r of res4) {
                                    return res3.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'x'
                                    });
                                }
                            }
                        });
                        switch (res3.getReturnValue(true)) {
                        case 'labeled_breakx':
                            return res2.stopIter({
                                'code': 'labeled_break',
                                'args': 'x'
                            });
                        }
                    } else {
                        for (var r of res3) {
                            if (res4.isInstance) {
                                res4.iter(function (r) {
                                    return res4.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'x'
                                    });
                                });
                                switch (res4.getReturnValue(true)) {
                                case 'labeled_breakx':
                                    return res2.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'x'
                                    });
                                }
                            } else {
                                for (var r of res4) {
                                    return res2.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'x'
                                    });
                                }
                            }
                        }
                    }
                });
                switch (res2.getReturnValue(true)) {
                case 'labeled_breakx':
                    return res1.stopIter({
                        'code': 'labeled_break',
                        'args': 'x'
                    });
                }
            } else {
                for (var r of res2) {
                    if (res3.isInstance) {
                        res3.iter(function (r) {
                            if (res4.isInstance) {
                                res4.iter(function (r) {
                                    return res4.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'x'
                                    });
                                });
                                switch (res4.getReturnValue(true)) {
                                case 'labeled_breakx':
                                    return res3.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'x'
                                    });
                                }
                            } else {
                                for (var r of res4) {
                                    return res3.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'x'
                                    });
                                }
                            }
                        });
                        switch (res3.getReturnValue(true)) {
                        case 'labeled_breakx':
                            return res1.stopIter({
                                'code': 'labeled_break',
                                'args': 'x'
                            });
                        }
                    } else {
                        for (var r of res3) {
                            if (res4.isInstance) {
                                res4.iter(function (r) {
                                    return res4.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'x'
                                    });
                                });
                                switch (res4.getReturnValue(true)) {
                                case 'labeled_breakx':
                                    return res1.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'x'
                                    });
                                }
                            } else {
                                for (var r of res4) {
                                    return res1.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'x'
                                    });
                                }
                            }
                        }
                    }
                }
            }
        });
    } else {
        x:
            for (var r of res1) {
                if (res2.isInstance) {
                    res2.iter(function (r) {
                        if (res3.isInstance) {
                            res3.iter(function (r) {
                                if (res4.isInstance) {
                                    res4.iter(function (r) {
                                        return res4.stopIter({
                                            'code': 'labeled_break',
                                            'args': 'x'
                                        });
                                    });
                                    switch (res4.getReturnValue(true)) {
                                    case 'labeled_breakx':
                                        return res3.stopIter({
                                            'code': 'labeled_break',
                                            'args': 'x'
                                        });
                                    }
                                } else {
                                    for (var r of res4) {
                                        return res3.stopIter({
                                            'code': 'labeled_break',
                                            'args': 'x'
                                        });
                                    }
                                }
                            });
                            switch (res3.getReturnValue(true)) {
                            case 'labeled_breakx':
                                return res2.stopIter({
                                    'code': 'labeled_break',
                                    'args': 'x'
                                });
                            }
                        } else {
                            for (var r of res3) {
                                if (res4.isInstance) {
                                    res4.iter(function (r) {
                                        return res4.stopIter({
                                            'code': 'labeled_break',
                                            'args': 'x'
                                        });
                                    });
                                    switch (res4.getReturnValue(true)) {
                                    case 'labeled_breakx':
                                        return res2.stopIter({
                                            'code': 'labeled_break',
                                            'args': 'x'
                                        });
                                    }
                                } else {
                                    for (var r of res4) {
                                        return res2.stopIter({
                                            'code': 'labeled_break',
                                            'args': 'x'
                                        });
                                    }
                                }
                            }
                        }
                    });
                    switch (res2.getReturnValue(true)) {
                    case 'labeled_breakx':
                        break x;
                    }
                } else {
                    for (var r of res2) {
                        if (res3.isInstance) {
                            res3.iter(function (r) {
                                if (res4.isInstance) {
                                    res4.iter(function (r) {
                                        return res4.stopIter({
                                            'code': 'labeled_break',
                                            'args': 'x'
                                        });
                                    });
                                    switch (res4.getReturnValue(true)) {
                                    case 'labeled_breakx':
                                        return res3.stopIter({
                                            'code': 'labeled_break',
                                            'args': 'x'
                                        });
                                    }
                                } else {
                                    for (var r of res4) {
                                        return res3.stopIter({
                                            'code': 'labeled_break',
                                            'args': 'x'
                                        });
                                    }
                                }
                            });
                            switch (res3.getReturnValue(true)) {
                            case 'labeled_breakx':
                                break x;
                            }
                        } else {
                            for (var r of res3) {
                                if (res4.isInstance) {
                                    res4.iter(function (r) {
                                        return res4.stopIter({
                                            'code': 'labeled_break',
                                            'args': 'x'
                                        });
                                    });
                                    switch (res4.getReturnValue(true)) {
                                    case 'labeled_breakx':
                                        break x;
                                    }
                                } else {
                                    for (var r of res4) {
                                        break x;
                                    }
                                }
                            }
                        }
                    }
                }
            }
    }
}
function N1qlQuery(query) {
    var stopSignal = false;
    this.isInstance = true;
    this.query = query;
    var stopMeta = {};
    this.iter = function (callback) {
        for (var i = 0; i < 100; i++) {
            if (!stopSignal) {
                callback(i);
            } else {
                stopSignal = false;
                return stopMeta;
            }
        }
    };
    this.stopIter = function (meta) {
        stopMeta = meta;
        stopSignal = true;
    };
    this.getReturnValue = function (concat) {
        var returnValue;
        if (concat) {
            returnValue = stopMeta.code + stopMeta.args;
        } else {
            returnValue = stopMeta;
        }
        return returnValue;
    };
}