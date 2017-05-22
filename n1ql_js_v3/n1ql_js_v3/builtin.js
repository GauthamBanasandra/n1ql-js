function N1qlQuery(query) {
  this.query = query;
  this.isInstance = true;
  this.iter = iter;
  this.execQuery = execQuery;
  this.stopIter = stopIter;
}
