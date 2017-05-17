try {
    if (res1.isInstance) {
        res1.x = res1.iter(function (row) {
            if (res2.isInstance) {
                res2.x = res2.iter(function (row) {
                    if (res3.isInstance) {
                        res3.x = res3.iter(function (row) {
                            (res3.stopIter())
                            (res2.stopIter())
                            (res1.stopIter())
                            throw 'error';
                        });
                    } else {
                        for (var row of res3) {
                            (res2.stopIter())
                            (res1.stopIter())
                            throw 'error';
                        }
                    }
                });
            } else {
                for (var row of res2) {
                    if (res3.isInstance) {
                        res3.x = res3.iter(function (row) {
                            (res3.stopIter())
                            (res1.stopIter())
                            throw 'error';
                        });
                    } else {
                        for (var row of res3) {
                            (res1.stopIter())
                            throw 'error';
                        }
                    }
                }
            }
        });
    } else {
        for (var row of res1) {
            if (res2.isInstance) {
                res2.x = res2.iter(function (row) {
                    if (res3.isInstance) {
                        res3.x = res3.iter(function (row) {
                            (res3.stopIter())
                            (res2.stopIter())
                            throw 'error';
                        });
                    } else {
                        for (var row of res3) {
                            (res2.stopIter())
                            throw 'error';
                        }
                    }
                });
            } else {
                for (var row of res2) {
                    if (res3.isInstance) {
                        res3.x = res3.iter(function (row) {
                            (res3.stopIter())
                            throw 'error';
                        });
                    } else {
                        for (var row of res3) {
                            throw 'error';
                        }
                    }
                }
            }
        }
    }
} catch (e) {
}