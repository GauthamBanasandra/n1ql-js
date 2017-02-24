function OnUpdate() {
    var x = 10;
    var y = 20;
    iter(function () {
        var z = x + y;
        log(z);
        x = z;
    });

    log(x);
}
OnUpdate();
