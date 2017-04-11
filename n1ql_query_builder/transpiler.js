var fs = require('fs'),
    esprima = require('esprima'),
    estraverse = require('estraverse'),
    escodegen = require('escodegen');

var filename = process.argv[2];
var code = fs.readFileSync(filename, 'utf-8');

console.log(escodegen.generate(get_ast(code, esprima, estraverse, escodegen), {
    comment: true
}));

// TODO:    Handle the case when comment appears inside a string - /* this is 'a comm*/'ent */ - must be handled in the
// lex. TODO:    Bug - Doesn't detect the N1QL variable if it's in the global scope.
function get_ast(code, esprima, estraverse, escodegen) {
    function Stack() {
        var stack = [];

        this.stackCopy = stack;

        this.push = function (item) {
            stack.push(item);
        };

        this.pop = function () {
            if (stack.length === 0)
                throw 'Stack underflow exception';
            return stack.pop();
        };

        this.peek = function () {
            var size = this.getSize();
            if (size >= 0) {
                return stack[size - 1];
            }
        };

        this.getSize = function () {
            return stack.length;
        };

        this.contains = function (item) {
            for (var _item of stack) {
                if (_item === item) {
                    return true;
                }
            }
            return false;
        };

        // debug.
        this.printAll = function () {
            for (var item of stack) {
                console.log(item);
                console.log();
            }
        }
    }

    function LoopModifier(modifier) {
        var ancestorStack = new Stack();
        this.modType = modifier;
        this.stackIndex = -1;

        // debug.
        this.stackCopy = ancestorStack.stackCopy;

        var associations = new Set();
        switch (this.modType) {
            case LoopModifier.CONST.BREAK:
                associations.add('DoWhileStatement');
                associations.add('ForStatement');
                associations.add('ForInStatement');
                associations.add('ForOfStatement');
                associations.add('SwitchStatement');
                associations.add('WhileStatement');
                associations.add('LabeledStatement');
                break;
            case LoopModifier.CONST.CONTINUE:
                associations.add('DoWhileStatement');
                associations.add('ForStatement');
                associations.add('ForInStatement');
                associations.add('ForOfStatement');
                associations.add('SwitchStatement');
                associations.add('WhileStatement');
                break;
            case LoopModifier.CONST.LABELED_BREAK:
                associations.add('LabeledStatement');
                break;
            case LoopModifier.CONST.RETURN:
                associations.add('FunctionDeclaration');
                associations.add('FunctionExpression');
                break;
            default:
                throw 'Invalid modifier';
        }

        this.checkAssoc = function (nodeType) {
            return associations.has(nodeType);
        };

        this.pushIfAssoc = function (node) {
            if (this.checkAssoc(node.type)) {
                switch (this.modType) {
                    case LoopModifier.CONST.BREAK:
                        node.breakStackIndex = this.stackIndex;
                        break;
                    case LoopModifier.CONST.CONTINUE:
                        node.continueStackIndex = this.stackIndex;
                        break;
                    case LoopModifier.CONST.LABELED_BREAK:
                        console.assert(/LabeledStatement/.test(node.type), 'can only push a labeled statement');
                        node.lblBreakStackIndex = this.stackIndex;
                        break;
                    case LoopModifier.CONST.RETURN:
                        node.returnStackIndex = this.stackIndex;
                        break;
                    default:
                        throw 'Invalid modifier type';
                }

                ancestorStack.push(node);
            }
        };

        this.popIfAssoc = function () {
            if (ancestorStack.getSize() > 0) {
                switch (this.modType) {
                    case LoopModifier.CONST.BREAK:
                        if (this.stackIndex === ancestorStack.peek().breakStackIndex) {
                            return ancestorStack.pop();
                        }
                        break;
                    case LoopModifier.CONST.CONTINUE:
                        if (this.stackIndex === ancestorStack.peek().continueStackIndex) {
                            return ancestorStack.pop();
                        }
                        break;
                    case LoopModifier.CONST.LABELED_BREAK:
                        if (this.stackIndex === ancestorStack.peek().lblBreakStackIndex) {
                            return ancestorStack.pop();
                        }
                        break;
                    case LoopModifier.CONST.RETURN:
                        if (this.stackIndex === ancestorStack.peek().returnStackIndex) {
                            return ancestorStack.pop();
                        }
                        break;
                    default:
                        throw 'Invalid modifier type';
                }
            }
        };

        this.getSize = function () {
            return ancestorStack.getSize();
        };

        // Returns a boolean suggesting whether the node needs to be replaced.
        this.isReplaceReq = function (args) {
            console.assert(ancestorStack.getSize() >= 0, 'ancestorStack size can not be negative');
            switch (this.modType) {
                // For break and continue, the replacement criteria is the for-of node being the parent on TOS.
                case LoopModifier.CONST.CONTINUE:
                case LoopModifier.CONST.BREAK:
                    return ancestorStack.getSize() > 0 && /ForOfStatement/.test(ancestorStack.peek().type);
                // For labelled break, the replacement criteria is the absence of the label which the break is
                // associated with.
                case LoopModifier.CONST.LABELED_BREAK:
                    return !ancestorStack.contains(args);
                // For return statement, the replacement criteria is the absence of a function on TOS.
                case LoopModifier.CONST.RETURN:
                    if (ancestorStack.getSize() === 0) {
                        return true;
                    }

                    return !(/FunctionDeclaration/.test(ancestorStack.peek().type) ||
                    /FunctionExpression/.test(ancestorStack.peek().type));
                default:
                    throw 'Invalid modifier type';
            }
        };

        // debug.
        this.printAll = function () {
            ancestorStack.printAll();
        }
    }

    LoopModifier.CONST = {
        BREAK: 'break',
        CONTINUE: 'continue',
        LABELED_BREAK: 'labelled_break',
        RETURN: 'return'
    };

    function Ast(_type) {
        this.type = _type;
    }

    function SwitchAst(discriminantAst) {
        Ast.call(this, 'SwitchStatement');
        this.discriminant = discriminantAst;
        this.cases = [];
    }

    function CaseAst(testStr) {
        Ast.call(this, 'SwitchCase');
        this.test = {
            "type": "Literal",
            "value": testStr,
            "raw": String.raw `${testStr}`
        };
        this.consequent = [];
    }

    function BreakAst() {
        Ast.call(this, 'BreakStatement');
        this.label = null;
    }

    function LabeledBreakAst(label) {
        BreakAst.call(this);
        this.label = {
            "type": "Identifier",
            "name": label
        }
    }

    function Arg(code, args) {
        this.code = code;
        this.args = args;
        this.toString = function () {
            var obj = {};
            for(var key of Object.keys(this)) {
                if(this.hasOwnProperty(key)){
                    obj[key]=this[key];
                }
            }

            return JSON.stringify(obj);
        }
    }

    // Replaces source node with the target node and returns a reference to the new node.
    function replace_node(source, target) {
        Object.keys(source).forEach(function (key) {
            delete source[key];
        });
        Object.keys(target).forEach(function (key) {
            source[key] = target[key];
        });

        return source;
    }

    function insert_node(parentBody, insAfterNode, nodeToInsert) {
        console.assert(parentBody instanceof Array, 'parentBody must be an Array');
        console.assert(parentBody.indexOf(insAfterNode) !== -1, 'node not found in the parent body');

        var insertIndex = parentBody.indexOf(insAfterNode) + 1;
        parentBody.splice(insertIndex, 0, nodeToInsert);
    }

    function insert_array(parentBody, insAfterNode, arrayToInsert) {
        console.assert(parentBody instanceof Array, 'parentBody must be an Array');
        console.assert(arrayToInsert instanceof Array, 'arrayToInsert must be an Array');
        var insertIndex = parentBody.indexOf(insAfterNode) + 1;
        parentBody.splice.apply(parentBody, [insertIndex, 0].concat(arrayToInsert));
    }

    function convert_to_block_stmt(node) {
        switch (node.type) {
            case 'ForOfStatement':
                // Transform the previous single-line statement into a block.
                node.body.body = [deep_copy(node.body)];
                node.body.type = 'BlockStatement';
                break;
            case 'IfStatement':
                node.consequent.body = [deep_copy(node.consequent)];
                node.consequent.type = 'BlockStatement';
                // If the 'else' part exists, convert it to a block statement.
                if (node.alternate) {
                    node.alternate.body = [deep_copy(node.alternate)];
                    node.alternate.type = 'BlockStatement';
                }
                break;
        }
    }

    function deep_copy(node) {
        return JSON.parse(JSON.stringify(node));
    }

    // Returns an iterator construct for a given for-of loop ast.
    // We currently support only for for-of loop. forEach might be supported in the future.
    function get_iter_ast(forOfNode, mode) {
        // List to store post iteration exit conditions.
        var postIter = [];
        // This is the property that will be set on the N1qlQuery instance - contains return value of iterator.
        var iterProp = 'x';
        var nodeCopy = deep_copy(forOfNode);

        if (mode === 'for_of') {
            estraverse.traverse(nodeCopy, {
                enter: function (node) {
                    ++breakMod.stackIndex;
                    ++continueMod.stackIndex;
                    ++lblBreakMod.stackIndex;
                    ++returnMod.stackIndex;

                    if (node.isGen) {
                        return;
                    }

                    breakMod.pushIfAssoc(node);
                    continueMod.pushIfAssoc(node);
                    lblBreakMod.pushIfAssoc(node);
                    returnMod.pushIfAssoc(node);

                    var arg,
                        stopIterAst,
                        argsAst,
                        returnStmtAst;
                    // If any of the exit criteria is encountered, then that statement may be replaced.
                    switch (node.type) {
                        case 'BreakStatement':
                            // Labeled break statement.
                            if (node.label && lblBreakMod.isReplaceReq(node.label.name)) {
                                stopIterAst = esprima.parse(nodeCopy.right.name + ".stopIter();");
                                arg = new Arg(LoopModifier.CONST.LABELED_BREAK, node.label.name);
                                // Need to wrap 'arg' inside '()' to turn it into a statement - it becomes a JSON
                                // object otherwise.
                                argsAst = esprima.parse('(' + arg + ')');

                                if (!(arg in postIter)) {
                                    postIter.push(arg);
                                } // Unlabeled break statement.
                            } else if (!node.label && breakMod.isReplaceReq()) {
                                stopIterAst = esprima.parse(nodeCopy.right.name + ".stopIter();");
                                arg = new Arg(LoopModifier.CONST.BREAK);
                                argsAst = esprima.parse('(' + arg + ')');
                            }

                            if (stopIterAst && argsAst) {
                                returnStmtAst = {
                                    "type": "ReturnStatement",
                                    "argument": stopIterAst.body[0].expression
                                };
                                // Add 'arg' as the argument to 'stopIter()'.
                                stopIterAst.body[0].expression.arguments.push(argsAst.body[0].expression);
                                replace_node(node, returnStmtAst);
                            }
                            break;
                        case 'ContinueStatement':
                            if (continueMod.isReplaceReq()) {
                                returnStmtAst = {
                                    "type": "ReturnStatement",
                                    "argument": null
                                };
                                replace_node(node, returnStmtAst);
                            }
                            break;
                        case 'ReturnStatement':
                            if (returnMod.isReplaceReq(node)) {
                                stopIterAst = esprima.parse(nodeCopy.right.name + ".stopIter();");

                                console.log('need to replace return node ', node);
                            }
                            break;
                        case 'IfStatement':
                            if (!/BlockStatement/.test(node.type)) {
                                convert_to_block_stmt(node);
                            }
                            break;
                    }
                },
                leave: function (node) {
                    breakMod.popIfAssoc();
                    continueMod.popIfAssoc();
                    lblBreakMod.popIfAssoc();
                    returnMod.popIfAssoc();

                    --breakMod.stackIndex;
                    --continueMod.stackIndex;
                    --lblBreakMod.stackIndex;
                    --returnMod.stackIndex;
                }
            });
            var iter = esprima.parse(
                forOfNode.right.name + '.' + iterProp + '=' + forOfNode.right.name +
                '.iter(function (' + (forOfNode.left.name ? forOfNode.left.name : forOfNode.left.declarations[0].id.name) + '){});'
            ).body[0];
            iter.expression.right.arguments[0].body = nodeCopy.body;

            var iterBlockAst = esprima.parse('{}').body[0];
            iterBlockAst.body.push(iter);

            var postIterAst = get_post_iter_ast(forOfNode.right.name, iterProp, postIter);
            if (postIterAst) {
                iterBlockAst.body.push(postIterAst);
            }

            return iterBlockAst;
        }

        throw 'Invalid arg ' + mode + ' for get_iter_ast';
    }

    // Returns a switch-case block to perform post-iteration steps.
    function get_post_iter_ast(iterVar, prop, postIterStmts) {
        if (postIterStmts.length <= 0) {
            return null;
        }

        var discriminantAst = esprima.parse(iterVar + '.' + prop + '.code' + '+' + iterVar + '.' + prop + '.args').body[0].expression;
        var switchAst = new SwitchAst(discriminantAst);

        for (var postIterStmt of postIterStmts) {
            var postIter = JSON.parse(postIterStmt);
            var caseAst = new CaseAst(postIter.code + postIter.args);
            switch (postIter.code) {
                case LoopModifier.CONST.BREAK:
                case LoopModifier.CONST.CONTINUE:
                    break;
                case LoopModifier.CONST.LABELED_BREAK:
                    caseAst.consequent.push(new LabeledBreakAst(postIter.args));
                    break;
            }

            switchAst.cases.push(caseAst);
        }

        return switchAst;
    }

    // Returns iterator consturct with dynamic type checking.
    function get_iter_compatible_ast(forOfNode) {
        // Make a copy of the 'for ... of ...' loop.
        var nodeCopy = deep_copy(forOfNode);

        // Iterator AST.
        var iterAst = get_iter_ast(nodeCopy, 'for_of');

        // 'if ... else ...' which perform dynamic type checking.
        var ifElseAst = esprima.parse('if(' + forOfNode.right.name + '.isInstance){}else{}').body[0];
        // Push the iterator AST into 'if' block.
        ifElseAst.consequent.body = iterAst.body;
        // Push the user-written 'for ... of ...' loop into 'else' block.
        ifElseAst.alternate.body.push(forOfNode);

        // Mark all the nodes of 'ifElseAst' to avoid repeated operations.
        estraverse.traverse(ifElseAst, {
            enter: function (node) {
                node.isGen = true;
            }
        });

        // Traverse all the for-of loops in the 'else' block and mark them as visited - so that we don't
        // recursively convert these into iterator constructs.
        estraverse.traverse(nodeCopy, {
            enter: function (node) {
                if (/ForOfStatement/.test(node.type))
                    node.isVisited = true;
            }
        });

        return ifElseAst;
    }

    // Build an ast node for N1QL function call from the query.
    function get_query_ast(query) {
        // Identifier regex.
        var re = /:([a-zA-Z_$][a-zA-Z_$0-9]*)/g;

        // Replace the :<var> with proper substitution.
        query = query.replace(re, '" + $1 + "');
        query = 'new N1qlQuery("' + query + '");';

        return esprima.parse(query).body[0].expression;
    }

    function is_n1ql_node(node) {
        // A N1QL node is a statement of the form new N1qlQuery('...');
        return /NewExpression/.test(node.type) &&
            /N1qlQuery/.test(node.callee.name);
    }

    var breakMod = new LoopModifier(LoopModifier.CONST.BREAK);
    var continueMod = new LoopModifier(LoopModifier.CONST.CONTINUE);
    var lblBreakMod = new LoopModifier(LoopModifier.CONST.LABELED_BREAK);
    var returnMod = new LoopModifier(LoopModifier.CONST.RETURN);

    // Get the Abstract Syntax Tree (ast) of the input code.
    var ast = esprima.parse(code, {
        attachComment: true,
        sourceType: 'script'
    });

    estraverse.traverse(ast, {
        leave: function (node) {
            // Perform variable substitution in query constructor.
            if (is_n1ql_node(node) && node.arguments.length > 0) {
                var queryAst = get_query_ast(node.arguments[0].quasis[0].value.raw);
                replace_node(node, deep_copy(queryAst));
            }

            // TODO : Handle the case when the source of 'for ... of ...' is of type x.y
            // Modifies all the 'for ... of ...' constructs to support iteration.
            // Takes care to see to it that it visits the node only once.
            if (/ForOfStatement/.test(node.type) && !node.isVisited) {
                if (!/BlockStatement/.test(node.body.type)) {
                    convert_to_block_stmt(node);
                }

                var iterAst = get_iter_compatible_ast(node);
                replace_node(node, deep_copy(iterAst));
            }
        }
    });

    return ast;
}