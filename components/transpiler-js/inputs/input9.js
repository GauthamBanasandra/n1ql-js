function OnUpdate(doc, meta) {
    log('document', doc);
    //res = test_continue();
    var nums = [1, 2, 3, 4, 5, 6, 7, 8, 9 , 10];
    var count = 0;
    for(var row of nums) {
        for(var row of nums) {
            count++;
            if(count == 5 || count == 7){
                continue;
            }
            ++count;
        }
    }
    var res1 = SELECT * FROM src_bucket LIMIT 10;
    var count1 = 0;
    for(var row1 of res) {
        for(var row2 of res1) {
            count1++;
            if(count1 == 5 || count1 == 7){
                return;
            }
            ++count1;
        }
    }
    log("count : ",count);
    log("count1 : ",count1);
    if (count === count1){
        dst_bucket[meta.id] = doc;
    }
}
function OnDelete(meta) {
}

//function test_continue(){
//
//}