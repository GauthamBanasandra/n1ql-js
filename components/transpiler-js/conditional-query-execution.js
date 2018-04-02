function q1(v) {
    var q = INSERT INTO `hello-world` ( KEY, VALUE ) VALUES ( UUID() ,$v);
}

function q2() {
    INSERT INTO `hello-world` ( KEY, VALUE ) VALUES ( UUID() ,'timerCallback');
}

function q3() {
    INSERT INTO `hello-world` ( KEY, VALUE ) VALUES ( UUID() ,'timerCallback');
}

function q4() {
    INSERT INTO `hello-world` ( KEY, VALUE ) VALUES ( UUID() ,'timerCallback');
}


var q1 = INSERT INTO `hello-world` ( KEY, VALUE ) VALUES ( UUID() ,$v);
var q2 = INSERT INTO `hello-world` ( KEY, VALUE ) VALUES ( UUID() ,$v);
var q3 = INSERT INTO `hello-world` ( KEY, VALUE ) VALUES ( UUID() ,$v);
var q4 = INSERT INTO `hello-world` ( KEY, VALUE ) VALUES ( UUID() ,$v);

var queries = [q1, q2, q3, q4];
queries[0].execQuery();

var stmt = new N1qlQuery('INSERT INTO destination (KEY, VALUE) VALUES (\'key\', \'value\');', { namedParams: {} }).execQuery();

stmt.execQuery();