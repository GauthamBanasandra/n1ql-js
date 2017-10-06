var res = new N1qlQuery('SELECT * FROM `beer-sample` LIMIT 10;');

for (var row of res) {
  log(row['beer-sample'].name);
}

log(res.metadata);

