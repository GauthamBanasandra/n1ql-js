var res = new N1qlQuery('SELECT * FROM `beer-sample` LIMIT 10;');
res.iter(function (r){
         log(r['beer-sample'].name);
         });
