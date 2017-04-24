var list = [1, 2, 3, 4, 5];
x: if (true) {
    console.log('hello');
} else {
    console.log('inside else');
    for (var r of list) {
        continue;
    }
}