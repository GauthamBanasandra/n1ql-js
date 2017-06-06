function N1qlQuery(query) {
  this.query = query;
  this.metadata = null;
  this.isInstance = true;
  this.iter = iter;
  this.execQuery = execQuery;
  this.stopIter = stopIter;
}
