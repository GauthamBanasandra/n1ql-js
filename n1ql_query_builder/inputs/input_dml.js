/**
 * Created by gautham on 16/02/17.
 */
function n1ql() {
    var bucket = 'default';
    var all = (function () {/* select *         FROM :bucket; */
    });
    for (var i of all){
        log(i);
    }
}
n1ql();