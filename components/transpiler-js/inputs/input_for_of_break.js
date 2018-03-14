/**
 * Created by gautham on 23/02/17.
 */
function modify_break() {
    var bucket = 'default';
    var list = (function () {/* select *         FROM :bucket; */
    });

    for(var item of list)
    {
        console.log(item);
        break;
    }
}