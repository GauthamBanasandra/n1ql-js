var name = 'Rochefort 10';
var lim = 100;
var res = new N1qlQuery('SELECT  * FROM `beer-sample` LIMIT :lim;');
var count = 0,
count1 = 0;
x: for (var row of res) {
  ++count1;
  var res2 = new N1qlQuery('SELECT  * FROM `beer-sample` LIMIT :lim;');
  var count2 = 0;
  try {
    for (var row2 of res2) {
      ++count2;
      ++count;
      log(`count1 = ${count1} count2 = ${count2}`);
      log(`row1 = ${row['beer-sample'].name} row2 = ${row2['beer-sample'].name}`);
      continue x;
    }
  } catch (e) {
    
  }
  
}

log(`count = ${count}`);
