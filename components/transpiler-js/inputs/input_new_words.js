function n1ql()
{
    var bucket='default';


    for(var i of all)
        log(i);

    var up=
        (function(){/* upsert INTO :bucket (key, value)        VALUES (\"game\", {\"name\":\"lara croft tomb raider\"})        RETURNING *; */});
    /*	var all=
     SELECT *
     FROM :bucket
     LIMIT 10;
     */

    var all=
        (function(){/* select *		FROM :bucket		LIMIT 10; */});

    printf("select * from `beer-sample`");
}
n1ql();