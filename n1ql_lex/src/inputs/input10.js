function OnUpdate(doc, meta) {
				SELECT * FROM src_bucket LIMIT 10;
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
    var count1 = 0;
    for(var row of res1) {
      for(var row of res1) {
        count1++;
        if(count1 == 5 || count1 == 7){
          continue;
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