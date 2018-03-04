/**
 * Created by gautham on 02/03/17.
 */
/*outer :
 for (var r of res) {
 for (var i = 0; i < 10; ++i)
 continue;
 for (var j = 0; j < 10; ++j)
 continue outer;
 }

 outer:{
 res.iter(function (r) {
 for (var i = 0; i < 10; ++i)
 continue;
 for (var j = 0; j < 10; ++j)
 return;
 });
 }

 outer :
 for (var i = 0; i < 10; ++i) {
 for (var r of res)
 continue outer;
 }

 outer :
 for (var i = 0; i < 10; ++i) {
 res.iter(function (r) {
 return;
 })
 }*/
var l='l';
let s = `hel${l}o`;
console.log(s);