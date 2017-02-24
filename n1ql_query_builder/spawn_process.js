/**
 * Created by gautham on 30/01/17.
 */
/*
 var spawn = require('child_process').spawn,
 child = spawn('/Users/gautham/projects/linq_js/linq_parser/src/a.out'),
 sleep=require('sleep');
 child.stdin.setEncoding('utf-8');
 // child.stdout.pipe(process.stdout);
 var result='';
 child.stdout.on('data', function (data)
 {
 result+=data.toString();
 });
 child.on('close', function ()
 {
 console.log(result);
 });
 var query = 'from city in cities where 1 select city.name;';
 child.stdin.write(var query = 'from city in cities where 1 select city.name;';);
 child.stdin.end();*/
var query = 'from city in cities where 1 select city.name;';
var spawn = require('child_process').spawnSync('/Users/gautham/projects/linq_js/linq_parser/src/a.out',
    [],
    {input: (query.length + 10) + ' ' + query + '\n'});
console.log(spawn.stdout.toString());