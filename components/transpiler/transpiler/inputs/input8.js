var query = SELECT count(*),join_yr FROM src_bucket group by join_yr;
for(var row of query){
  log('each row: ', row);
}
log(query.metadata);
