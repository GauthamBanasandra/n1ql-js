/**
 * Created by gautham on 14/02/17.
 */
var re = /:([a-zA-Z_$][a-zA-Z_$0-9]*)/g;
var string = "select city from cities where area > :_area and name = ':_name';";
do {
    var match = re.exec(string);
    if (match)
        console.log(match[1]);
} while (match != null);