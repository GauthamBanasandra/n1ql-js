y:while (true) {
    for (var r1 of res) {
        x: for (var r2 of res) {
            for (var r2 of res) {
                break x;
            }
        }
        break y;
    }
}