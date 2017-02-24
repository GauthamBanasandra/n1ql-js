function get_city(_name) {
    var cities = 'Bangalore';
    var _area = 1000;

    var n1ql_res = (function() { /* select city from cities where area > :_area and name = ':_name'; */ });
    for(var i of n1ql_res)
        console.log(i);
    for(var i of cities)
        console.log(i);

    return n1ql_res;
}

get_city();

function n1ql(query) {
    console.log(query);
}