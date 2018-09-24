// Here's a single line comment
function OnUpdate(doc, meta) {
    /*
        Now, here's a multiline comment
     */
    var bucket = '`beer-sample`',
        city = 'Bangalore';
    var res = new N1qlQuery(`SELECT * FROM system:bucket WHERE city == "city"`);
}

