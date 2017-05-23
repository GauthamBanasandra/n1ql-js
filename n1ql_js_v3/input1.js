var res1 = new N1qlQuery('SELECT * FROM `beer-sample` LIMIT 10;');
for(var row of res1.execQuery()) {
  log(row['beer-sample'].brewery_id);
  log('');
}
