var fs = require('fs'),
    esprima = require('esprima'),
    estraverse = require('estraverse'),
    escodegen = require('escodegen');

var filename = process.argv[2];
var code = fs.readFileSync(filename, 'utf-8');
var transpiledCode = escodegen.generate(get_ast(code), {comment: true});
console.log(transpiledCode);
esprima.parse(transpiledCode);

// TODO:    Remove the arguments - esprima, estraverse, this.escodegen to get_ast in the next commit - they are
// redundant.
// TODO:    Handle the case when comment appears inside a string - /* this is 'a comm*/'ent */ - must be
// handled in the lex.
// TODO:    Bug - Doesn't detect the N1QL variable if it's in the global scope.
function get_ast(code) {
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

        this.cloneStack = function () {
            var clone = new Stack();

            for (var item of stack) {
                clone.push(deep_copy(item));
            }

            return clone;
        };

        this.reverseElements = function () {
            stack.reverse();
        };

        // debug.
        this.printAll = function () {
            for (var item of stack) {
                console.log(item);
                console.log();
            }
        }
    }

    function AncestorStack() {
        Stack.call(this);

        // Returns or pops a node that satisfies the comparator.
        function getOrPopAncestor(_this, comparator, pop) {
            var temp = new Stack();
            var found = false;

            while (_this.getSize() > 0 && !found) {
                var node = _this.pop();
                temp.push(node);
                found = comparator(deep_copy(node));
            }

            if (pop) {
                temp.pop();
            }

            while (temp.getSize() > 0) {
                _this.push(temp.pop());
            }

            return found ? node : null;
        }

        this.getAncestor = function (comparator) {
            return getOrPopAncestor(this, comparator, false);
        };

        this.popNode = function (comparator) {
            return getOrPopAncestor(this, comparator, true);
        };

        this.getTopNodeOfType = function (nodeType) {
            return this.getAncestor(function (node) {
                return nodeType === node.type;
            });
        };

        this.popTopNodeOfType = function (nodeType) {
            return this.popNode(function (node) {
                return nodeType === node.type;
            });
        };

        this.cloneAncestorStack = function () {
            var clone = new AncestorStack();
            // Clone the stack.
            var stackClone = this.cloneStack();

            while (stackClone.getSize() > 0) {
                clone.push(stackClone.pop());
            }

            // Reverse the stack elements.
            clone.reverseElements();

            return clone;
        }
    }

    function LoopModifier(modifier) {
        var ancestorStack = new AncestorStack();
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
            case LoopModifier.CONST.LABELED_CONTINUE:
            case LoopModifier.CONST.LABELED_BREAK:
                associations.add('LabeledStatement');
                break;
            case LoopModifier.CONST.RETURN:
                associations.add('FunctionDeclaration');
                associations.add('FunctionExpression');
                break;
            case LoopModifier.CONST.THROW:
                associations.add('TryStatement');
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
                    case LoopModifier.CONST.LABELED_CONTINUE:
                        console.assert(/LabeledStatement/.test(node.type), 'can only push a labeled statement');
                        node.lblContinueStackIndex = this.stackIndex;
                        break;
                    case LoopModifier.CONST.THROW:
                        node.throwStackIndex = this.stackIndex;
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
                    case LoopModifier.CONST.LABELED_CONTINUE:
                        if (this.stackIndex === ancestorStack.peek().lblContinueStackIndex) {
                            return ancestorStack.pop();
                        }
                        break;
                    case LoopModifier.CONST.THROW:
                        if (this.stackIndex === ancestorStack.peek().throwStackIndex) {
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
                case LoopModifier.CONST.LABELED_CONTINUE:
                // For labelled break, the replacement criteria is the absence of the label which the break is
                // associated with.
                case LoopModifier.CONST.LABELED_BREAK:
                    return !ancestorStack.getAncestor(function (node) {
                        if (/LabeledStatement/.test(node.type)) {
                            return args === node.label.name;
                        }
                    });
                // For return statement, the replacement criteria is the absence of a function on TOS.
                case LoopModifier.CONST.RETURN:
                    if (ancestorStack.getSize() === 0) {
                        return true;
                    }

                    return !(/FunctionDeclaration/.test(ancestorStack.peek().type) ||
                    /FunctionExpression/.test(ancestorStack.peek().type));
                case LoopModifier.CONST.THROW:
                    if (ancestorStack.getSize() === 0) {
                        return true;
                    }
                    return !/TryStatement/.test(ancestorStack.peek().type);
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
        LABELED_BREAK: 'labeled_break',
        RETURN: 'return',
        LABELED_CONTINUE: 'labeled_continue',
        THROW: 'throw'
    };

    // Utilities for AncestorStack
    function StackHelper(ancestorStack) {
        this.ancestorStack = ancestorStack.cloneAncestorStack();

        this.getTopForOfNode = function () {
            return this.ancestorStack.getTopNodeOfType('ForOfStatement');
        };

        this.popTopForOfNode = function () {
            return this.ancestorStack.popTopNodeOfType('ForOfStatement');
        };

        function search(_this, comparator, searchAll) {
            var temp = new Stack(),
                stopNodes = [],
                returnArgs = {targetFound: false};

            while (_this.ancestorStack.getSize() > 0) {
                var node = _this.ancestorStack.pop();
                temp.push(node);

                if (comparator.targetComparator(deep_copy(node))) {
                    returnArgs = {targetFound: true, stopNode: deep_copy(node), searchInterrupted: false};
                    break;
                } else if (comparator.stopComparator(deep_copy(node))) {
                    returnArgs = {targetFound: false, stopNode: deep_copy(node), searchInterrupted: true};
                    if (searchAll) {
                        stopNodes.push(returnArgs.stopNode);
                    } else {
                        break;
                    }
                }
            }

            while (temp.getSize() > 0) {
                _this.ancestorStack.push(temp.pop());
            }

            // A check to validate that targetFound and searchInterrupted are mutually exclusive.
            if (returnArgs.targetFound && returnArgs.searchInterrupted) {
                throw 'Invalid case: targetFound=' + returnArgs.targetFound +
                '\tsearchInterrupted=' + returnArgs.searchInterrupted;
            }

            if (searchAll && stopNodes.length > 0) {
                returnArgs = {targetFound: false, stopNodes: stopNodes, searchInterrupted: true};
            }

            return returnArgs;
        }

        // Targeted search.
        // The comparator should implement targetComparator - find the target node.
        // stopComparator - stop searching if a particular node is found in the ancestor stack.
        this.searchStack = function (comparator) {
            return search(this, comparator);
        };

        this.searchAllStopNodes = function (comparator) {
            return search(this, comparator, true);
        }
    }

    function Ast(type) {
        this.type = type;
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

    function LabeledStmtAst(label, body) {
        Ast.call(this, 'LabeledStatement');
        this.label = {
            "type": "Identifier",
            "name": label
        };
        this.body = body;
    }

    function LabeledBreakAst(label) {
        BreakAst.call(this);
        this.label = {
            "type": "Identifier",
            "name": label
        };
    }

    function ContinueAst() {
        Ast.call(this, 'ContinueStatement');
        this.label = null;
    }

    function LabeledContinueAst(label) {
        ContinueAst.call(this);
        this.label = {
            "type": "Identifier",
            "name": label
        };
    }

    function ReturnAst(argument) {
        Ast.call(this, 'ReturnStatement');
        this.argument = argument;
    }

    function StopIterAst(inst) {
        Ast.call(this, 'CallExpression');
        this.callee = {
            "type": "MemberExpression",
            "computed": false,
            "object": {
                "type": "Identifier",
                "name": inst
            },
            "property": {
                "type": "Identifier",
                "name": "stopIter"
            }
        };
        this.arguments = [];
    }

    // Class for maintaining the object that will be passed to 'stopIter'.
    function Arg(arg) {
        this.code = arg.code;
        this.args = arg.args;

        this.getAst = function () {
            // Need to wrap 'arg' inside '()' to turn it into a statement - it becomes a JSON
            // object otherwise.
            var argsAst = esprima.parse('(' + this.toString() + ')').body[0].expression;

            // Setting appendData to 'true' will generate the AST for 'args' and append it to 'argsAst'.
            if (arg.appendData) {
                var dataAst = {
                    "type": "Property",
                    "key": {
                        "type": "Literal",
                        "value": "data",
                        "raw": "'data'"
                    },
                    "computed": false,
                    "value": this.getDataAst(),
                    "kind": "init",
                    "method": false,
                    "shorthand": false
                };

                argsAst.properties.push(dataAst);
            }

            return argsAst;
        };

        // Returns the AST for 'args'.
        this.getDataAst = function () {
            if (!this.args) {
                throw '"args" field is needed to add "data" field';
            }

            return esprima.parse(this.args).body[0].expression;
        };

        // Stringify only the attributes of this class.
        this.toString = function () {
            var obj = {};
            for (var key of Object.keys(this)) {
                if (this.hasOwnProperty(key)) {
                    obj[key] = this[key];
                }
            }

            return JSON.stringify(obj);
        }
    }

    // Returns AST of the form 'iterVar'.'prop'.data
    function ReturnDataAst(iterVar, prop) {
        Ast.call(this, 'ExpressionStatement');
        this.expression = {
            "type": "MemberExpression",
            "computed": false,
            "object": {
                "type": "MemberExpression",
                "computed": false,
                "object": {
                    "type": "Identifier",
                    "name": iterVar
                },
                "property": {
                    "type": "Identifier",
                    "name": prop
                }
            },
            "property": {
                "type": "Identifier",
                "name": "data"
            }
        };
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

    function insert_node(parentBody, refNode, nodeToInsert, insertAfter) {
        console.assert(parentBody instanceof Array, 'parentBody must be an Array');
        console.assert(parentBody.indexOf(refNode) !== -1, 'node not found in the parent body');

        var insertIndex = insertAfter ? parentBody.indexOf(refNode) + 1 : parentBody.indexOf(refNode);
        parentBody.splice(insertIndex, 0, nodeToInsert);
    }

    function delete_node(parentBody, nodeToDel) {
        console.assert(parentBody instanceof Array, 'parentBody must be an Array');
        console.assert(parentBody.indexOf(nodeToDel) !== -1, 'node not found in the parent body');

        var deleteIndex = parentBody.indexOf(nodeToDel);
        parentBody.splice(deleteIndex, 1);
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
    function get_iter_consequent_ast(forOfNode, stackHelper) {
        var breakMod = new LoopModifier(LoopModifier.CONST.BREAK);
        var continueMod = new LoopModifier(LoopModifier.CONST.CONTINUE);
        var lblBreakMod = new LoopModifier(LoopModifier.CONST.LABELED_BREAK);
        var returnMod = new LoopModifier(LoopModifier.CONST.RETURN);
        var lblContinueMod = new LoopModifier(LoopModifier.CONST.LABELED_CONTINUE);
        var throwMod = new LoopModifier(LoopModifier.CONST.THROW);

        // List to store post iteration exit conditions.
        var postIter = [];
        // This is the property that will be set on the N1qlQuery instance - contains return value of iterator.
        var iterProp = 'x';
        var nodeCopy = deep_copy(forOfNode);

        estraverse.traverse(nodeCopy, {
            enter: function (node, parent) {
                ++breakMod.stackIndex;
                ++continueMod.stackIndex;
                ++lblBreakMod.stackIndex;
                ++returnMod.stackIndex;
                ++lblContinueMod.stackIndex;
                ++throwMod.stackIndex;

                breakMod.pushIfAssoc(node);
                continueMod.pushIfAssoc(node);
                lblBreakMod.pushIfAssoc(node);
                returnMod.pushIfAssoc(node);
                lblContinueMod.pushIfAssoc(node);
                throwMod.pushIfAssoc(node);

                var arg,
                    stopIterAst,
                    returnStmtAst;

                // Annotated nodes are those nodes that have been marked to be changed by the previous iteration.
                if (node.isAnnotated) {
                    switch (node.metaData.code) {
                        case LoopModifier.CONST.THROW:
                            // For an annotated 'throw' node, nothing needs to be passed to switch-case.
                            // So, we just return.
                            return;
                        case LoopModifier.CONST.RETURN:
                            // For 'return', the 'iterVar' must be set to the current for-of loop's source.
                            node.metaData.iterVar = nodeCopy.right.name;
                            arg = JSON.stringify(node.metaData);
                            break;
                        default:
                            // Currently expected to handle labeled break and continue statements.
                            arg = new Arg({code: node.metaData.code, args: node.metaData.args});
                            break;
                    }

                    if (postIter.indexOf(arg.toString()) === -1) {
                        postIter.push(arg.toString());
                    }

                    // Remove the annotation if it's already present.
                    // This is needed to prevent the else-block from replacing the already annotated node.
                    delete node.isAnnotated;
                    delete node.metaData;
                }

                if (node.isGen) {
                    return;
                }

                // If any of the exit criteria is encountered, then that statement may be replaced.
                switch (node.type) {
                    case 'BreakStatement':
                        stopIterAst = arg = null;
                        // Labeled break statement.
                        if (node.label && lblBreakMod.isReplaceReq(node.label.name)) {
                            // TODO:    Might want to check for null.
                            var instName = stackHelper.getTopForOfNode().right.name;
                            stopIterAst = new StopIterAst(instName);
                            arg = new Arg({code: LoopModifier.CONST.LABELED_BREAK, args: node.label.name});

                            if (postIter.indexOf(arg.toString()) === -1) {
                                postIter.push(arg);
                            } // Unlabeled break statement.
                        } else if (!node.label && breakMod.isReplaceReq()) {
                            stopIterAst = new StopIterAst(nodeCopy.right.name);
                            // TODO :   Unlabeled break is handled slightly different from unlabeled continue. Why?
                            arg = new Arg({code: LoopModifier.CONST.BREAK});
                        }

                        if (stopIterAst && arg) {
                            returnStmtAst = new ReturnAst(stopIterAst);
                            // Add 'arg' as the argument to 'stopIter()'.
                            stopIterAst.arguments.push(arg.getAst());
                            replace_node(node, returnStmtAst);
                        }
                        break;
                    case 'ContinueStatement':
                        // Labeled continue statement.
                        if (node.label && lblContinueMod.isReplaceReq(node.label.name)) {
                            if (nodeCopy.parentLabel === node.label.name) {
                                returnStmtAst = new ReturnAst(null);
                            } else {
                                arg = new Arg({code: LoopModifier.CONST.LABELED_CONTINUE, args: node.label.name});
                                stopIterAst = new StopIterAst(nodeCopy.right.name);
                                returnStmtAst = new ReturnAst(stopIterAst);
                                stopIterAst.arguments.push(arg.getAst());

                                if (postIter.indexOf(arg.toString()) === -1) {
                                    postIter.push(arg);
                                }
                            }
                            replace_node(node, returnStmtAst);
                            // Unlabeled continue statement.
                        } else if (continueMod.isReplaceReq()) {
                            returnStmtAst = new ReturnAst(null);
                            replace_node(node, returnStmtAst);
                        }
                        break;
                    case 'ReturnStatement':
                        if (returnMod.isReplaceReq(node)) {
                            // Return statement may or may not have arguments.
                            // In case there's no argument, we populate it with null.
                            var argStr = node.argument ? escodegen.generate(node.argument) : null;
                            // Must enclose the return statement's argument within an expression '()'.
                            // Otherwise, it causes an error when returning anonymous function.
                            arg = new Arg({
                                code: LoopModifier.CONST.RETURN,
                                args: '(' + argStr + ')',
                                appendData: true
                            });
                            stopIterAst = new StopIterAst(nodeCopy.right.name);
                            stopIterAst.arguments.push(arg.getAst());
                            returnStmtAst = new ReturnAst(stopIterAst);

                            var postIterArgs = JSON.stringify({
                                code: LoopModifier.CONST.RETURN,
                                args: arg.args,
                                iterVar: nodeCopy.right.name,
                                targetFunction: node.targetFunction
                            });

                            if (postIter.indexOf(postIterArgs) === -1) {
                                postIter.push(postIterArgs);
                            }

                            replace_node(node, returnStmtAst);
                        }
                        break;
                    case 'ThrowStatement':
                        if (throwMod.isReplaceReq()) {
                            // For a 'throw' statement, we need to either find the enclosing try-catch block,
                            // or the list of for-of nodes that are on the way to the try-catch block.
                            // Hence, we use 'searchAllStopNodes'.
                            var lookup = stackHelper.searchAllStopNodes({
                                targetComparator: function (item) {
                                    return /TryStatement/.test(item.type);
                                },
                                stopComparator: function (item) {
                                    return /ForOfStatement/.test(item.type);
                                }
                            });
                            if (lookup.searchInterrupted) {
                                // For each stopNode that is encountered, construct a 'stopIter' statement and insert it.
                                for (var stopNode of lookup.stopNodes) {
                                    stopIterAst = new StopIterAst(stopNode.right.name);
                                    // The stopIter statements need to be annotated so that it is picked up by the
                                    // 'else' block.
                                    stopIterAst.isAnnotated = true;
                                    stopIterAst.metaData = {
                                        code: LoopModifier.CONST.THROW,
                                        iterVar: stopNode.right.name
                                    };
                                    insert_node(parent.body, node, stopIterAst, false);
                                }
                            }
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
                lblContinueMod.popIfAssoc();
                throwMod.popIfAssoc();

                --breakMod.stackIndex;
                --continueMod.stackIndex;
                --lblBreakMod.stackIndex;
                --returnMod.stackIndex;
                --lblContinueMod.stackIndex;
                --throwMod.stackIndex;
            }
        });
        // TODO :   Create a class for iter().
        var iter = esprima.parse(
            forOfNode.right.name + '.' + iterProp + '=' + forOfNode.right.name +
            '.iter(function (' + (forOfNode.left.name ? forOfNode.left.name : forOfNode.left.declarations[0].id.name) + '){});'
        ).body[0];
        iter.expression.right.arguments[0].body = nodeCopy.body;

        var iterBlockAst = esprima.parse('{}').body[0];
        iterBlockAst.body.push(iter);

        // Pop the top for-of node.
        stackHelper.popTopForOfNode();

        var postIterAst = get_post_iter_ast(forOfNode.right.name, iterProp, postIter, stackHelper);
        if (postIterAst) {
            iterBlockAst.body.push(postIterAst);
        }

        console.assert(breakMod.getSize() === 0, 'breakMod must be empty');
        console.assert(continueMod.getSize() === 0, 'continueMod must be empty');
        console.assert(lblBreakMod.getSize() === 0, 'lblBreakMod must be empty');
        console.assert(returnMod.getSize() === 0, 'returnMod must be empty');
        console.assert(lblContinueMod.getSize() === 0, 'lblContinueMod must be empty');

        return iterBlockAst;
    }

    // Returns AST for 'else' block.
    function get_iter_alternate_ast(forOfNode, stackHelper) {
        var nodeCopy = deep_copy(forOfNode);
        var breakMod = new LoopModifier(LoopModifier.CONST.BREAK);
        var continueMod = new LoopModifier(LoopModifier.CONST.CONTINUE);
        var lblBreakMod = new LoopModifier(LoopModifier.CONST.LABELED_BREAK);
        var returnMod = new LoopModifier(LoopModifier.CONST.RETURN);
        var lblContinueMod = new LoopModifier(LoopModifier.CONST.LABELED_CONTINUE);
        var throwMod = new LoopModifier(LoopModifier.CONST.THROW);

        // debug.
        // console.log('input for else code:\n', escodegen.generate(nodeCopy), '\n');

        estraverse.traverse(nodeCopy, {
            enter: function (node, parent) {
                var lookup, stopIterAst, arg, returnStmtAst, stopNode = null;

                if (node.isAnnotated) {
                    // For an annotated 'throw' node - this is a stopIter statement, annotated by 'iter_consequent'.
                    if (node.metaData.code === LoopModifier.CONST.THROW) {
                        // Delete the 'stopIter' statement if its instance variable matches that of the current
                        // for-of loop's source - because, w.k.t since the instance variable is not an iterator in the
                        // else-block.
                        if (nodeCopy.right.name === node.metaData.iterVar) {
                            delete_node(parent.body, node);
                        }
                        return;
                    }
                    lookup = stackHelper.searchStack({
                        targetComparator: function (item) {
                            switch (node.metaData.code) {
                                case LoopModifier.CONST.RETURN:
                                    // For a 'return' statement, the target is to find the function that the 'return'
                                    // statement was associated with, before transpilation.
                                    return (/FunctionDeclaration/.test(item.type) || /FunctionExpression/.test(item.type))
                                        && item.id.name === node.metaData.targetFunction;
                                default:
                                    // Handles both labeled break and continue statements.
                                    return /LabeledStatement/.test(item.type) && item.label.name === node.metaData.args;
                            }
                        },
                        stopComparator: function (item) {
                            return /ForOfStatement/.test(item.type);
                        }
                    });
                    if (lookup.targetFound) {
                        switch (node.metaData.code) {
                            case LoopModifier.CONST.LABELED_BREAK:
                                replace_node(node, new LabeledBreakAst(node.metaData.args));
                                break;
                            case LoopModifier.CONST.LABELED_CONTINUE:
                                replace_node(node, new LabeledContinueAst(node.metaData.args));
                                break;
                            case LoopModifier.CONST.RETURN:
                                arg = new Arg({
                                    code: LoopModifier.CONST.RETURN,
                                    args: node.metaData.args,
                                    appendData: true
                                });
                                replace_node(node, new ReturnAst(arg.getDataAst()));
                                break;
                        }
                    }
                    if (lookup.searchInterrupted) {
                        console.assert(/ForOfStatement/.test(lookup.stopNode.type), 'must be a for-of node');
                        switch (node.metaData.code) {
                            case LoopModifier.CONST.LABELED_BREAK:
                                stopIterAst = new StopIterAst(lookup.stopNode.right.name);
                                arg = new Arg({code: node.metaData.code, args: node.metaData.args});
                                returnStmtAst = new ReturnAst(stopIterAst);
                                stopIterAst.arguments.push(arg.getAst());
                                break;
                            case LoopModifier.CONST.LABELED_CONTINUE:
                                if (lookup.stopNode.parentLabel === node.metaData.args) {
                                    returnStmtAst = new ReturnAst(null);
                                } else {
                                    stopIterAst = new StopIterAst(lookup.stopNode.right.name);
                                    arg = new Arg({code: node.metaData.code, args: node.metaData.args});
                                    returnStmtAst = new ReturnAst(stopIterAst);
                                    stopIterAst.arguments.push(arg.getAst());
                                }
                                break;
                            case LoopModifier.CONST.RETURN:
                                arg = new Arg({
                                    code: LoopModifier.CONST.RETURN,
                                    args: node.metaData.args,
                                    appendData: true
                                });
                                stopIterAst = new StopIterAst(lookup.stopNode.right.name);
                                stopIterAst.arguments.push(arg.getAst());
                                returnStmtAst = new ReturnAst(stopIterAst);
                                break;
                        }

                        returnStmtAst.isAnnotated = true;
                        returnStmtAst.metaData = node.metaData;

                        replace_node(node, returnStmtAst);
                    }

                    return;
                }

                ++breakMod.stackIndex;
                ++continueMod.stackIndex;
                ++lblBreakMod.stackIndex;
                ++returnMod.stackIndex;
                ++lblContinueMod.stackIndex;
                ++throwMod.stackIndex;

                breakMod.pushIfAssoc(node);
                continueMod.pushIfAssoc(node);
                lblBreakMod.pushIfAssoc(node);
                returnMod.pushIfAssoc(node);
                lblContinueMod.pushIfAssoc(node);
                throwMod.pushIfAssoc(node);

                switch (node.type) {
                    case 'BreakStatement':
                        // TODO :   Only labeled break case is being handled here. What about the unlabeled break case?
                        if (node.label && lblBreakMod.isReplaceReq(node.label.name)) {
                            lookup = stackHelper.searchStack({
                                targetComparator: function (item) {
                                    return /LabeledStatement/.test(item.type) && item.label.name === node.label.name;
                                },
                                stopComparator: function (item) {
                                    return /ForOfStatement/.test(item.type);
                                }
                            });
                            if (lookup.searchInterrupted) {
                                console.assert(/ForOfStatement/.test(lookup.stopNode.type), 'must be a for-of node');

                                stopIterAst = new StopIterAst(lookup.stopNode.right.name);
                                arg = new Arg({code: LoopModifier.CONST.LABELED_BREAK, args: node.label.name});
                                returnStmtAst = new ReturnAst(stopIterAst);
                                stopIterAst.arguments.push(arg.getAst());

                                returnStmtAst.isAnnotated = true;
                                returnStmtAst.metaData = {
                                    code: LoopModifier.CONST.LABELED_BREAK,
                                    args: node.label.name
                                };
                                replace_node(node, returnStmtAst);
                            }
                        }
                        break;
                    case 'ContinueStatement':
                        if (node.label && lblContinueMod.isReplaceReq(node.label.name)) {
                            lookup = stackHelper.searchStack({
                                targetComparator: function (item) {
                                    return /LabeledStatement/.test(item.type) && item.label.name === node.label.name;
                                },
                                stopComparator: function (item) {
                                    return /ForOfStatement/.test(item.type);
                                }
                            });
                            if (lookup.searchInterrupted) {
                                console.assert(/ForOfStatement/.test(lookup.stopNode.type), 'must be a for-of node');

                                if (lookup.stopNode.parentLabel === node.label.name) {
                                    returnStmtAst = new ReturnAst(null);
                                } else {
                                    stopIterAst = new StopIterAst(lookup.stopNode.right.name);
                                    arg = new Arg({code: LoopModifier.CONST.LABELED_CONTINUE, args: node.label.name});
                                    returnStmtAst = new ReturnAst(stopIterAst);
                                    stopIterAst.arguments.push(arg.getAst());
                                }

                                returnStmtAst.isAnnotated = true;
                                returnStmtAst.metaData = {
                                    code: LoopModifier.CONST.LABELED_CONTINUE,
                                    args: node.label.name
                                };
                                replace_node(node, returnStmtAst);
                            }
                        }
                        break;
                    case 'ReturnStatement':
                        if (node.targetFunction) {
                            lookup = stackHelper.searchStack({
                                targetComparator: function (item) {
                                    return (/FunctionDeclaration/.test(item.type) || /FunctionExpression/.test(item.type))
                                        && item.id.name === node.targetFunction;
                                },
                                stopComparator: function (item) {
                                    return /ForOfStatement/.test(item.type);
                                }
                            });
                            if (lookup.searchInterrupted) {
                                var argStr = node.argument ? escodegen.generate(node.argument) : null;

                                arg = new Arg({
                                    code: LoopModifier.CONST.RETURN,
                                    args: '(' + argStr + ')',
                                    appendData: true
                                });
                                stopIterAst = new StopIterAst(lookup.stopNode.right.name);
                                stopIterAst.arguments.push(arg.getAst());
                                returnStmtAst = new ReturnAst(stopIterAst);

                                returnStmtAst.isAnnotated = true;
                                returnStmtAst.metaData = {
                                    code: LoopModifier.CONST.RETURN,
                                    args: arg.args,
                                    iterVar: nodeCopy.right.name,
                                    targetFunction: node.targetFunction
                                };

                                replace_node(node, returnStmtAst);
                            }
                        }
                        break;
                    case 'ThrowStatement':
                        // We need to check and consider only those 'throw' statements that aren't generated.
                        if (!node.isGen && throwMod.isReplaceReq()) {
                            lookup = stackHelper.searchAllStopNodes({
                                targetComparator: function (item) {
                                    return /TryStatement/.test(item.type);
                                },
                                stopComparator: function (item) {
                                    return /ForOfStatement/.test(item.type);
                                }
                            });
                            if (lookup.searchInterrupted) {
                                for (stopNode of lookup.stopNodes) {
                                    stopIterAst = new StopIterAst(stopNode.right.name);
                                    stopIterAst.isAnnotated = true;
                                    stopIterAst.metaData = {
                                        code: LoopModifier.CONST.THROW,
                                        iterVar: stopNode.right.name
                                    };
                                    insert_node(parent.body, node, stopIterAst, false);
                                }
                            }
                        }
                        break;
                }
            },
            leave: function (node) {
                breakMod.popIfAssoc();
                continueMod.popIfAssoc();
                lblBreakMod.popIfAssoc();
                returnMod.popIfAssoc();
                lblContinueMod.popIfAssoc();
                throwMod.popIfAssoc();

                --breakMod.stackIndex;
                --continueMod.stackIndex;
                --lblBreakMod.stackIndex;
                --returnMod.stackIndex;
                --lblContinueMod.stackIndex;
                --throwMod.stackIndex;
            }
        });

        console.assert(breakMod.getSize() === 0, 'breakMod must be empty');
        console.assert(continueMod.getSize() === 0, 'continueMod must be empty');
        console.assert(lblBreakMod.getSize() === 0, 'lblBreakMod must be empty');
        console.assert(returnMod.getSize() === 0, 'returnMod must be empty');
        console.assert(lblContinueMod.getSize() === 0, 'lblContinueMod must be empty');

        // debug.
        // console.log('else code:\n', escodegen.generate(nodeCopy), '\n\n\n\n\n');

        return nodeCopy.parentLabel ? new LabeledStmtAst(nodeCopy.parentLabel, nodeCopy) : nodeCopy;
    }

    // Returns a switch-case block to perform post-iteration steps.
    function get_post_iter_ast(iterVar, prop, postIterStmts, stackHelper) {
        var discriminantAst = esprima.parse(iterVar + '.' + prop + '.code' + '+' + iterVar + '.' + prop + '.args').body[0].expression,
            switchAst = new SwitchAst(discriminantAst),
            postIter, caseAst, lookup, stopIterAst, arg, returnStmtAst, pushCase;

        for (var postIterStmt of postIterStmts) {
            pushCase = true;
            // TODO :   Changing 'var postIter' to 'const postIter' causes a unit test to fail. Investigate this issue.
            postIter = JSON.parse(postIterStmt);
            caseAst = new CaseAst(postIter.code + postIter.args);

            switch (postIter.code) {
                case LoopModifier.CONST.BREAK:
                case LoopModifier.CONST.CONTINUE:
                    break;
                case LoopModifier.CONST.LABELED_BREAK:
                    lookup = stackHelper.searchStack({
                        targetComparator: function (node) {
                            return /LabeledStatement/.test(node.type) && node.label.name === postIter.args;
                        },
                        stopComparator: function (node) {
                            return /ForOfStatement/.test(node.type);
                        }
                    });
                    if (lookup.targetFound) {
                        console.assert(lookup.stopNode.label.name === postIter.args, 'labels must match');

                        if (/ForOfStatement/.test(lookup.stopNode.body.type)) {
                            pushCase = false;
                        } else {
                            caseAst.consequent.push(new LabeledBreakAst(postIter.args));
                        }
                    }
                    if (lookup.searchInterrupted) {
                        console.assert(/ForOfStatement/.test(lookup.stopNode.type), 'must be a for-of node');

                        stopIterAst = new StopIterAst(lookup.stopNode.right.name);
                        arg = new Arg({code: LoopModifier.CONST.LABELED_BREAK, args: postIter.args});
                        returnStmtAst = new ReturnAst(stopIterAst);
                        stopIterAst.arguments.push(arg.getAst());

                        returnStmtAst.isAnnotated = true;
                        returnStmtAst.metaData = postIter;

                        caseAst.consequent.push(returnStmtAst);
                    }
                    break;
                case LoopModifier.CONST.LABELED_CONTINUE:
                    lookup = stackHelper.searchStack({
                        targetComparator: function (node) {
                            return /LabeledStatement/.test(node.type) && node.label.name === postIter.args;
                        },
                        stopComparator: function (node) {
                            return /ForOfStatement/.test(node.type);
                        }
                    });
                    if (lookup.targetFound && !lookup.searchInterrupted) {
                        console.assert(lookup.stopNode.label.name === postIter.args, 'labels must match');

                        if (/ForOfStatement/.test(lookup.stopNode.body.type)) {
                            pushCase = false;
                        } else {
                            caseAst.consequent.push(new LabeledContinueAst(postIter.args));
                        }
                    }
                    if (lookup.searchInterrupted) {
                        console.assert(/ForOfStatement/.test(lookup.stopNode.type), 'must be a for-of node');

                        if (lookup.stopNode.parentLabel === postIter.args) {
                            returnStmtAst = new ReturnAst(null);
                        } else {
                            stopIterAst = new StopIterAst(lookup.stopNode.right.name);
                            arg = new Arg({code: LoopModifier.CONST.LABELED_CONTINUE, args: postIter.args});
                            returnStmtAst = new ReturnAst(stopIterAst);
                            stopIterAst.arguments.push(arg.getAst());
                        }

                        returnStmtAst.isAnnotated = true;
                        returnStmtAst.metaData = postIter;

                        caseAst.consequent.push(returnStmtAst);
                    }
                    break;
                case LoopModifier.CONST.RETURN:
                    lookup = stackHelper.searchStack({
                        targetComparator: function (item) {
                            return (/FunctionDeclaration/.test(item.type) || /FunctionExpression/.test(item.type))
                                && (item.id ? item.id.name : null) === postIter.targetFunction;
                        },
                        stopComparator: function (item) {
                            return /ForOfStatement/.test(item.type);
                        }
                    });
                    if (lookup.targetFound) {
                        console.assert(/FunctionDeclaration/.test(lookup.stopNode.type) || /FunctionExpression/.test(lookup.stopNode.type), 'node must be a function');

                        returnStmtAst = new ReturnAst(new ReturnDataAst(postIter.iterVar, prop));
                    }
                    if (lookup.searchInterrupted) {
                        console.assert(/ForOfStatement/.test(lookup.stopNode.type), 'must be a for-of node');

                        stopIterAst = new StopIterAst(lookup.stopNode.right.name);
                        arg = new Arg({
                            code: LoopModifier.CONST.RETURN,
                            args: postIter.iterVar + '.' + prop + '.data',
                            appendData: true
                        });
                        stopIterAst.arguments.push(arg.getAst());
                        returnStmtAst = new ReturnAst(stopIterAst);

                        returnStmtAst.isAnnotated = true;
                        postIter.args = arg.args;
                        returnStmtAst.metaData = postIter;
                    }
                    caseAst.consequent.push(returnStmtAst);
                    break;
            }

            if (pushCase) {
                switchAst.cases.push(caseAst);
            }
        }

        return switchAst.cases.length > 0 ? switchAst : null;
    }

    // Returns iterator construct with dynamic type checking.
    function get_iter_compatible_ast(forOfNode) {
        var stackHelper = new StackHelper(globalAncestorStack);

        // TODO:    Add a class for if-else statements.
        // 'if ... else ...' which perform dynamic type checking.
        var ifElseAst = esprima.parse('if(' + forOfNode.right.name + '.isInstance){}else{}').body[0];

        // Iterator AST.
        var iterConsequentAst = get_iter_consequent_ast(forOfNode, stackHelper);
        // Push the iterator AST into 'if' block.
        ifElseAst.consequent.body = iterConsequentAst.body;
        // Push the user-written 'for ... of ...' loop into 'else' block.
        ifElseAst.alternate.body.push(get_iter_alternate_ast(forOfNode, stackHelper));

        estraverse.traverse(ifElseAst, {
            enter: function (node) {
                // Mark all the nodes of 'ifElseAst' to avoid repeated operations.
                node.isGen = true;

                // Traverse all the for-of loops in the 'else' block and mark them as visited - so that we don't
                // recursively convert these into iterator constructs.
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

    var globalAncestorStack = new AncestorStack();

    // Get the Abstract Syntax Tree (ast) of the input code.
    var ast = esprima.parse(code, {
        attachComment: true,
        sourceType: 'script'
    });

    estraverse.traverse(ast, {
        enter: function (node, parent) {
            globalAncestorStack.push(node);

            if (/ForOfStatement/.test(node.type) && !node.isVisited && /LabeledStatement/.test(parent.type)) {
                node.parentLabel = parent.label.name;
                parent.remLabel = true;
            }
            if (/ReturnStatement/.test(node.type)) {
                var stackHelper = new StackHelper(globalAncestorStack);
                var lookup = stackHelper.searchStack({
                    targetComparator: function (item) {
                        return /FunctionDeclaration/.test(item.type) || /FunctionExpression/.test(item.type);
                    },
                    stopComparator: function (item) {
                        return false;
                    }
                });
                if (lookup.targetFound) {
                    console.assert(/FunctionDeclaration/.test(lookup.stopNode.type) || /FunctionExpression/.test(lookup.stopNode.type), 'node must be a function');

                    // TODO :   Anonymous function might require some attention because comparing null doesn't make sense.
                    node.targetFunction = lookup.stopNode.id ? lookup.stopNode.id.name : null;
                }
            }
        },
        leave: function (node) {
            // Perform variable substitution in query constructor.
            if (is_n1ql_node(node) && node.arguments.length > 0) {
                var queryAst = get_query_ast(node.arguments[0].quasis[0].value.raw);
                replace_node(node, deep_copy(queryAst));
            }

            // TODO : Handle the case when the source of for-of loop is of type x.y
            // Modifies all the 'for ... of ...' constructs to support iteration.
            // Takes care to see to it that it visits the node only once.
            if (/ForOfStatement/.test(node.type) && !node.isVisited) {
                if (!/BlockStatement/.test(node.body.type)) {
                    convert_to_block_stmt(node);
                }

                var iterAst = get_iter_compatible_ast(node);
                replace_node(node, deep_copy(iterAst));
            } else if (/LabeledStatement/.test(node.type) && node.remLabel) {
                replace_node(node, node.body);
            }

            globalAncestorStack.pop();
        }
    });

    return ast;
}