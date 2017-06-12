/**
 * Created by gautham on 12/06/17.
 */

function query() {
    console.log('inside query');
}

function modify_query() {
    query = function () {
        console.log('inside modified query');
    }
}

modify_query();
query();