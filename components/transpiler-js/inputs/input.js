x = 10;
const c = 8, d = 9;

function print()
{
    var y = 4;

    for (var i = 0; i < 4; ++i)
    {
        var z = 10;
    }
    function inner(k, y)
    {
        // this is a comment inside inner
        function func1()
        {
            var x1=4;
        }
    }

    function outer()
    {
        function func1()
        {
            var x1=4;
        }
    }
    inner();
    console.log(i);
}

print();