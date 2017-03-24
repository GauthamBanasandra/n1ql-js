out1: {
    console.log('out1');
    out2: {
        var x = 0;
        switch (x) {
            case 1:
                break;
            case 0:
                break out1;
        }
    }

    console.log('out2');
}

out3: {
    var x = 10;
    loop:
    while (x--) {
        out4: {
            if (x == 5)
                continue loop;
        }
        console.log(x);
    }
    console.log('x=', x);
}