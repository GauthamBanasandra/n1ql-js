function Stack() {
    var stack = [];

    this.push = function (item) {
        stack.push(item);
    }
    this.pop = function () {
        if (this.size() == 0)
            throw 'Stack underflow exception';
        return stack.pop();
    }
    this.size = function (params) {
        return stack.length;
    }
}

var numStack = new Stack();
numStack.push(1);
numStack.push(12);
numStack.push(11);

console.log('stack length='+numStack.size());

for (var i = 0; i < numStack.size(); ++i)
    console.log(numStack.pop());
console.log('stack length='+numStack.size());
console.log(numStack.pop());
console.log('stack length='+numStack.size());
numStack.pop();
numStack.pop();