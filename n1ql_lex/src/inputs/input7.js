var name = 'Rochefort 10',
lim = 10;
var res = SELECT *
          FROM `beer-sample`
          LIMIT :lim; var select;
for(var row of res) {
  log(row);
}