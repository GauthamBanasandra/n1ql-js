if (res1.isInstance) {
    res1.x = res1.iter(function (r) {
        if (res2.isInstance) {
            res2.x = res2.iter(function (r) {
                if (res3.isInstance) {
                    res3.x = res3.iter(function (r) {
                        if (res4.isInstance) {
                            res4.x = res4.iter(function (r) {
                                return res4.stopIter({
                                    'code': 'labeled_break',
                                    'args': 'z',
                                    'bubble': true
                                });
                            });
                            switch (res4.x.code + res4.x.args) {
                                case 'labeled_breakz':
                                    return res3.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'z',
                                        'bubble': true
                                    });
                            }
                        } else {
                            for (var r of res4) {
                                return res3.stopIter({
                                    'code': 'labeled_break',
                                    'args': 'z',
                                    'bubble': true
                                });
                            }
                        }
                    });
                    switch (res3.x.code + res3.x.args) {
                        case 'labeled_breakz':
                            return res2.stopIter({
                                'code': 'labeled_break',
                                'args': 'z',
                                'bubble': true
                            });
                    }
                } else {
                    for (var r of res3) {
                        if (res4.isInstance) {
                            res4.x = res4.iter(function (r) {
                                return res4.stopIter({
                                    'code': 'labeled_break',
                                    'args': 'z',
                                    'bubble': true
                                });
                            });
                            switch (res4.x.code + res4.x.args) {
                                case 'labeled_breakz':
                                    return res2.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'z',
                                        'bubble': true
                                    });
                            }
                        } else {
                            for (var r of res4) {
                                return res2.stopIter({
                                    'code': 'labeled_break',
                                    'args': 'z',
                                    'bubble': true
                                });
                            }
                        }
                    }
                }
            });
            switch (res2.x.code + res2.x.args) {
                case 'labeled_breakz':
                    return res1.stopIter({
                        'code': 'labeled_break',
                        'args': 'z',
                        'bubble': true
                    });
            }
        } else {
            for (var r of res2) {
                if (res3.isInstance) {
                    res3.x = res3.iter(function (r) {
                        if (res4.isInstance) {
                            res4.x = res4.iter(function (r) {
                                return res4.stopIter({
                                    'code': 'labeled_break',
                                    'args': 'z',
                                    'bubble': true
                                });
                            });
                            switch (res4.x.code + res4.x.args) {
                                case 'labeled_breakz':
                                    return res3.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'z',
                                        'bubble': true
                                    });
                            }
                        } else {
                            for (var r of res4) {
                                return res3.stopIter({
                                    'code': 'labeled_break',
                                    'args': 'z',
                                    'bubble': true
                                });
                            }
                        }
                    });
                    switch (res3.x.code + res3.x.args) {
                        case 'labeled_breakz':
                            return res1.stopIter({
                                'code': 'labeled_break',
                                'args': 'z',
                                'bubble': true
                            });
                    }
                } else {
                    for (var r of res3) {
                        if (res4.isInstance) {
                            res4.x = res4.iter(function (r) {
                                return res4.stopIter({
                                    'code': 'labeled_break',
                                    'args': 'z',
                                    'bubble': true
                                });
                            });
                            switch (res4.x.code + res4.x.args) {
                                case 'labeled_breakz':
                                    return res1.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'z',
                                        'bubble': true
                                    });
                            }
                        } else {
                            for (var r of res4) {
                                return res1.stopIter({
                                    'code': 'labeled_break',
                                    'args': 'z',
                                    'bubble': true
                                });
                            }
                        }
                    }
                }
            }
        }
    });
} else {
    z:
        for (var r of res1) {
            if (res2.isInstance) {
                res2.x = res2.iter(function (r) {
                    if (res3.isInstance) {
                        res3.x = res3.iter(function (r) {
                            if (res4.isInstance) {
                                res4.x = res4.iter(function (r) {
                                    return res4.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'z',
                                        'bubble': true
                                    });
                                });
                                switch (res4.x.code + res4.x.args) {
                                    case 'labeled_breakz':
                                        return res3.stopIter({
                                            'code': 'labeled_break',
                                            'args': 'z',
                                            'bubble': true
                                        });
                                }
                            } else {
                                for (var r of res4) {
                                    return res3.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'z',
                                        'bubble': true
                                    });
                                }
                            }
                        });
                        switch (res3.x.code + res3.x.args) {
                            case 'labeled_breakz':
                                return res2.stopIter({
                                    'code': 'labeled_break',
                                    'args': 'z',
                                    'bubble': true
                                });
                        }
                    } else {
                        for (var r of res3) {
                            if (res4.isInstance) {
                                res4.x = res4.iter(function (r) {
                                    return res4.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'z',
                                        'bubble': true
                                    });
                                });
                                switch (res4.x.code + res4.x.args) {
                                    case 'labeled_breakz':
                                        return res2.stopIter({
                                            'code': 'labeled_break',
                                            'args': 'z',
                                            'bubble': true
                                        });
                                }
                            } else {
                                for (var r of res4) {
                                    return res2.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'z',
                                        'bubble': true
                                    });
                                }
                            }
                        }
                    }
                });
                switch (res2.x.code + res2.x.args) {
                    case 'labeled_breakz':
                        break z;
                }
            } else {
                for (var r of res2) {
                    if (res3.isInstance) {
                        res3.x = res3.iter(function (r) {
                            if (res4.isInstance) {
                                res4.x = res4.iter(function (r) {
                                    return res4.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'z',
                                        'bubble': true
                                    });
                                });
                                switch (res4.x.code + res4.x.args) {
                                    case 'labeled_breakz':
                                        return res3.stopIter({
                                            'code': 'labeled_break',
                                            'args': 'z',
                                            'bubble': true
                                        });
                                }
                            } else {
                                for (var r of res4) {
                                    return res3.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'z',
                                        'bubble': true
                                    });
                                }
                            }
                        });
                        switch (res3.x.code + res3.x.args) {
                            case 'labeled_breakz':
                                break z;
                        }
                    } else {
                        for (var r of res3) {
                            if (res4.isInstance) {
                                res4.x = res4.iter(function (r) {
                                    return res4.stopIter({
                                        'code': 'labeled_break',
                                        'args': 'z',
                                        'bubble': true
                                    });
                                });
                                switch (res4.x.code + res4.x.args) {
                                    case 'labeled_breakz':
                                        break z;
                                }
                            } else {
                                for (var r of res4) {
                                    break z;
                                }
                            }
                        }
                    }
                }
            }
        }
}
