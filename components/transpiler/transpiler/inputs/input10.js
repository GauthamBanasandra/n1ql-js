function OnUpdate(doc, meta) {
  var lim = 10,
  	country = 'United States',
  	city = 'Philadelphia',
  	code = '94107';
  try {
    var query = SELECT * FROM `beer-sample` WHERE code = $code;
    
    
    //
    
    
    for(var row of query) {
      log(row);
    }
    
    log(query.metadata);
  } catch(e) {
    log(e);
  }
}
function OnDelete(meta) {
}

OnUpdate(1, 2);
