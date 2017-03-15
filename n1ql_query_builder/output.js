function N1qlQuery(queryStr) {
    this.query = queryStr;
    this.iter = function (callback) {
        callback(this.query);
    }
    var rows = [];
    for (var i = 0; i < 100; ++i) {
        rows.push(i);
    }
    this.rows = rows;
}

var all = new N1qlQuery();
j=20;

if (all instanceof N1qlQuery) {
    all.iter(function (i) {
        if (all instanceof N1qlQuery) {
            all.iter(function (i) {
                if (all instanceof N1qlQuery) {
                    all.iter(function (i) {
                        if (all instanceof N1qlQuery) {
                            all.iter(function (i) {
                                console.log(i, j);
                            });
                        } else {
                            for (var i of all) {
                                console.log(i, j);
                            }
                        }
                    });
                } else {
                    for (var i of all) {
                        if (all instanceof N1qlQuery) {
                            all.iter(function (i) {
                                console.log(i, j);
                            });
                        } else {
                            for (var i of all) {
                                console.log(i, j);
                            }
                        }
                    }
                }
            });
        } else {
            for (var i of all) {
                if (all instanceof N1qlQuery) {
                    all.iter(function (i) {
                        if (all instanceof N1qlQuery) {
                            all.iter(function (i) {
                                console.log(i, j);
                            });
                        } else {
                            for (var i of all) {
                                console.log(i, j);
                            }
                        }
                    });
                } else {
                    for (var i of all) {
                        if (all instanceof N1qlQuery) {
                            all.iter(function (i) {
                                console.log(i, j);
                            });
                        } else {
                            for (var i of all) {
                                console.log(i, j);
                            }
                        }
                    }
                }
            }
        }
    });
} else {
    for (var i of all) {
        if (all instanceof N1qlQuery) {
            all.iter(function (i) {
                if (all instanceof N1qlQuery) {
                    all.iter(function (i) {
                        if (all instanceof N1qlQuery) {
                            all.iter(function (i) {
                                console.log(i, j);
                            });
                        } else {
                            for (var i of all) {
                                console.log(i, j);
                            }
                        }
                    });
                } else {
                    for (var i of all) {
                        if (all instanceof N1qlQuery) {
                            all.iter(function (i) {
                                console.log(i, j);
                            });
                        } else {
                            for (var i of all) {
                                console.log(i, j);
                            }
                        }
                    }
                }
            });
        } else {
            for (var i of all) {
                if (all instanceof N1qlQuery) {
                    all.iter(function (i) {
                        if (all instanceof N1qlQuery) {
                            all.iter(function (i) {
                                console.log(i, j);
                            });
                        } else {
                            for (var i of all) {
                                console.log(i, j);
                            }
                        }
                    });
                } else {
                    for (var i of all) {
                        if (all instanceof N1qlQuery) {
                            all.iter(function (i) {
                                console.log(i, j);
                            });
                        } else {
                            for (var i of all) {
                                console.log(i, j);
                            }
                        }
                    }
                }
            }
        }
    }
}
