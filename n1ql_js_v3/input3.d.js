var res1 = new N1qlQuery(`select * from \`beer-sample\` LIMIT 10;`);
var res2 = new N1qlQuery(`select * from \`beer-sample\` LIMIT 10;`);

x: for (var r1 of res1) {
    console.log('query1:', r1)    
}

function N1qlQuery(query) {
  this.query = query;
  this.metadata = null;
  this.isInstance = true;
  this.iter = iter;
  this.execQuery = execQuery;
  this.stopIter = stopIter;
  this.getReturnValue = getReturnValue;
}
