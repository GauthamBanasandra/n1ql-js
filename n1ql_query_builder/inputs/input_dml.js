if (res1.isInstance) {
    res1.x = res1.iter(function (r) {
        if (res2.isInstance) {
            res2.x = res2.iter(function (r) {
                if (res3.isInstance) {
                    res3.x = res3.iter(function (r) {
                        if (res4.isInstance) {
                            res4.x = res4.iter(function (r) {
                                return res4.stopIter({
                                    'code': 'labeled_continue',
                                    'args': 'x'
                                });
                            });
                            switch (res4.x.code + res4.x.args) {
                                case 'labeled_continuex':
                                    return res3.stopIter({
                                        'code': 'labeled_continue',
                                        'args': 'x'
                                    });
                            }
                        } else {
                            for (var r of res4) {
                                return res3.stopIter({
                                    'code': 'labeled_continue',
                                    'args': 'x'
                                });
                            }
                        }
                    });
                    switch (res3.x.code + res3.x.args) {
                        case 'labeled_continuex':
                            return res2.stopIter({
                                'code': 'labeled_continue',
                                'args': 'x'
                            });
                    }
                } else {
                    for (var r of res3) {
                        if (res4.isInstance) {
                            res4.x = res4.iter(function (r) {
                                return res4.stopIter({
                                    'code': 'labeled_continue',
                                    'args': 'x'
                                });
                            });
                            switch (res4.x.code + res4.x.args) {
                                case 'labeled_continuex':
                                    return res2.stopIter({
                                        'code': 'labeled_continue',
                                        'args': 'x'
                                    });
                            }
                        } else {
                            for (var r of res4) {
                                return res2.stopIter({
                                    'code': 'labeled_continue',
                                    'args': 'x'
                                });
                            }
                        }
                    }
                }
            });
            switch (res2.x.code + res2.x.args) {
                case 'labeled_continuex':
                    return;
            }
        } else {
            for (var r of res2) {
                if (res3.isInstance) {
                    res3.x = res3.iter(function (r) {
                        if (res4.isInstance) {
                            res4.x = res4.iter(function (r) {
                                return res4.stopIter({
                                    'code': 'labeled_continue',
                                    'args': 'x'
                                });
                            });
                            switch (res4.x.code + res4.x.args) {
                                case 'labeled_continuex':
                                    return res3.stopIter({
                                        'code': 'labeled_continue',
                                        'args': 'x'
                                    });
                            }
                        } else {
                            for (var r of res4) {
                                return res3.stopIter({
                                    'code': 'labeled_continue',
                                    'args': 'x'
                                });
                            }
                        }
                    });
                    switch (res3.x.code + res3.x.args) {
                        case 'labeled_continuex':
                            return;
                    }
                } else {
                    for (var r of res3) {
                        if (res4.isInstance) {
                            res4.x = res4.iter(function (r) {
                                return res4.stopIter({
                                    'code': 'labeled_continue',
                                    'args': 'x'
                                });
                            });
                            switch (res4.x.code + res4.x.args) {
                                case 'labeled_continuex':
                                    return;
                            }
                        } else {
                            for (var r of res4) {
                                return;
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
                res2.x = res2.iter(function (r) {
                    if (res3.isInstance) {
                        res3.x = res3.iter(function (r) {
                            if (res4.isInstance) {
                                res4.x = res4.iter(function (r) {
                                    return res4.stopIter({
                                        'code': 'labeled_continue',
                                        'args': 'x'
                                    });
                                });
                                switch (res4.x.code + res4.x.args) {
                                    case 'labeled_continuex':
                                        return res3.stopIter({
                                            'code': 'labeled_continue',
                                            'args': 'x'
                                        });
                                }
                            } else {
                                for (var r of res4) {
                                    return res3.stopIter({
                                        'code': 'labeled_continue',
                                        'args': 'x'
                                    });
                                }
                            }
                        });
                        switch (res3.x.code + res3.x.args) {
                            case 'labeled_continuex':
                                return res2.stopIter({
                                    'code': 'labeled_continue',
                                    'args': 'x'
                                });
                        }
                    } else {
                        for (var r of res3) {
                            if (res4.isInstance) {
                                res4.x = res4.iter(function (r) {
                                    return res4.stopIter({
                                        'code': 'labeled_continue',
                                        'args': 'x'
                                    });
                                });
                                switch (res4.x.code + res4.x.args) {
                                    case 'labeled_continuex':
                                        return res2.stopIter({
                                            'code': 'labeled_continue',
                                            'args': 'x'
                                        });
                                }
                            } else {
                                for (var r of res4) {
                                    return res2.stopIter({
                                        'code': 'labeled_continue',
                                        'args': 'x'
                                    });
                                }
                            }
                        }
                    }
                });
                switch (res2.x.code + res2.x.args) {
                    case 'labeled_continuex':
                        continue x;
                }
            } else {
                for (var r of res2) {
                    if (res3.isInstance) {
                        res3.x = res3.iter(function (r) {
                            if (res4.isInstance) {
                                res4.x = res4.iter(function (r) {
                                    return res4.stopIter({
                                        'code': 'labeled_continue',
                                        'args': 'x'
                                    });
                                });
                                switch (res4.x.code + res4.x.args) {
                                    case 'labeled_continuex':
                                        return res3.stopIter({
                                            'code': 'labeled_continue',
                                            'args': 'x'
                                        });
                                }
                            } else {
                                for (var r of res4) {
                                    return res3.stopIter({
                                        'code': 'labeled_continue',
                                        'args': 'x'
                                    });
                                }
                            }
                        });
                        switch (res3.x.code + res3.x.args) {
                            case 'labeled_continuex':
                                continue x;
                        }
                    } else {
                        for (var r of res3) {
                            if (res4.isInstance) {
                                res4.x = res4.iter(function (r) {
                                    return res4.stopIter({
                                        'code': 'labeled_continue',
                                        'args': 'x'
                                    });
                                });
                                switch (res4.x.code + res4.x.args) {
                                    case 'labeled_continuex':
                                        continue x;
                                }
                            } else {
                                for (var r of res4) {
                                    continue x;
                                }
                            }
                        }
                    }
                }
            }
        }
}