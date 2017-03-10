var result;

function n1ql()
{
    var bucket='default';
    var res=SELECT * FROM :bucket;
    
    gen=row_gen(res);
    
    log(gen.next());
    log(gen.next());
}
n1ql();

function* row_gen(num)
{
    num.iter(function(row){
             yield* row;
        });
}
