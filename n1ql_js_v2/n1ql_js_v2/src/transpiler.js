// TODO:    Handle the case when comment appears inside a string - /* this is 'a comm*/'ent */ - must be handled in the lex.
// TODO:    Bug - Doesn't detect the N1QL variable if it's in the global scope.
function get_ast(code, esprima, estraverse, escodegen) {
    function Stack() {
        var stack = [];
        
        this.stackCopy = stack;
        this.push = function (item) {
            stack.push(item);
        }
        
        this.pop = function () {
            if (stack.length == 0)
                throw 'Stack underflow exception';
            return stack.pop();
        }
        
        this.peek = function () {
            var size = this.getSize();
            if (size >= 0) {
                return stack[size - 1];
            }
            return;
        }
        
        this.getSize = function () {
            return stack.length;
        }
        
        // debug.
        this.printAll = function () {
            for (var item of stack) {
                console.log(item);
                console.log();
            }
        }
    }
    
    function LoopModifier(modifier) {
        this.modType = modifier;
        var ancestorStack = new Stack();
        this.stackIndex = -1;
        
        // debug.
        this.stackCopy = ancestorStack.stackCopy;
        
        var associations = new Set();
        switch (this.modType) {
            case 'break':
                associations.add('DoWhileStatement');
                associations.add('ForStatement');
                associations.add('ForInStatement');
                associations.add('ForOfStatement');
                associations.add('SwitchStatement');
                associations.add('WhileStatement');
                associations.add('LabeledStatement');
                break;
            case 'continue':
                associations.add('DoWhileStatement');
                associations.add('ForStatement');
                associations.add('ForInStatement');
                associations.add('ForOfStatement');
                associations.add('SwitchStatement');
                associations.add('WhileStatement');
                break;
            default:
                throw 'Invalid modifier';
        }
        
        this.checkAssoc = function (nodeType) {
            return associations.has(nodeType);
        }
        
        this.pushIfAssoc = function (node) {
            if (this.checkAssoc(node.type)) {
                switch (this.modType) {
                    case 'break':
                        node.breakStackIndex = this.stackIndex;
                        break;
                    case 'continue':
                        node.continueStackIndex = this.stackIndex;
                        break;
                    default:
                        throw 'Invalid modifier type';
                }
                ancestorStack.push(node);
            }
        }
        
        this.popIfAssoc = function () {
            if (ancestorStack.getSize() > 0) {
                switch (this.modType) {
                    case 'break':
                        if (this.stackIndex == ancestorStack.peek().breakStackIndex) {
                            return ancestorStack.pop();
                        }
                        break;
                    case 'continue':
                        if (this.stackIndex == ancestorStack.peek().continueStackIndex) {
                            return ancestorStack.pop();
                        }
                        break;
                    default:
                        throw 'Invalid modifier type';
                }
            }
        }
        
        this.getSize = function () {
            return ancestorStack.getSize();
        }
        
        this.isReplaceReq = function () {
            if (ancestorStack.getSize() > 0) {
                switch (this.modType) {
                    case 'continue':
                    case 'break':
                        if (/ForOfStatement/.test(ancestorStack.peek().type)) {
                            return true;
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        // debug.
        this.printAll = function () {
            ancestorStack.printAll();
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
        //console.assert(parentBody instanceof Array, 'parentBody must be an Array');
        //console.assert(parentBody.indexOf(insAfterNode) != -1, 'node not found in the parent body');
        
        var insertIndex = parentBody.indexOf(insAfterNode) + 1;
        parentBody.splice(insertIndex, 0, nodeToInsert);
    }
    
    function insert_array(parentBody, insAfterNode, arrayToInsert) {
        //console.assert(parentBody instanceof Array, 'parentBody must be an Array');
        //console.assert(arrayToInsert instanceof Array, 'arrayToInsert must be an Array');
        var insertIndex = parentBody.indexOf(insAfterNode) + 1;
        parentBody.splice.apply(parentBody, [insertIndex, 0].concat(arrayToInsert));
    }
    
    function convert_to_block_stmt(node) {
        switch (node.type) {
            case 'ForOfStatement':
                var forBodyAst = {};
                // Make a deep copy of the body.
                Object.keys(node.body).forEach(function (key) {
                                               forBodyAst[key] = node.body[key];
                                               });
                // Transform the previous single-line statement into a block.
                node.body.body = [forBodyAst];
                node.body.type = 'BlockStatement';
                break;
            case 'IfStatement':
                var ifBodyAst = {};
                // Make a deep copy of the 'if' body.
                Object.keys(node.consequent).forEach(function (key) {
                                                     ifBodyAst[key] = node.consequent[key];
                                                     });
                node.consequent.body = [ifBodyAst];
                node.consequent.type = 'BlockStatement';
                // If the 'else' part exists, convert it to a block statement.
                if (node.alternate) {
                    var elseBodyAst = {};
                    // Make a deep copy of the 'else' body.
                    Object.keys(node.alternate).forEach(function (key) {
                                                        elseBodyAst[key] = node.alternate[key];
                                                        });
                    node.alternate.body = [elseBodyAst];
                    node.alternate.type = 'BlockStatement';
                }
                break;
        }
    }
    
    function deep_copy(node) {
        return JSON.parse(JSON.stringify(node));
    }
    
    function get_iter_ast(forOfNode, mode) {
        var returnStmtAst = {
            "type": "ReturnStatement",
            "argument": null
        };
        
        var nodeCopy = deep_copy(forOfNode);
        if (mode === 'for_of') {
            estraverse.traverse(nodeCopy, {
                                enter: function (node, parent) {
                                ++breakMod.stackIndex;
                                ++continueMod.stackIndex;
                                
                                if (node.isGen) {
                                return;
                                }
                                
                                breakMod.pushIfAssoc(node);
                                continueMod.pushIfAssoc(node);
                                
                                switch (node.type) {
                                case 'BreakStatement':
                                if (breakMod.isReplaceReq()) {
                                var stopIterAst = esprima.parse(nodeCopy.right.name + '.stopIter();');
                                stopIterAst = replace_node(node, stopIterAst.body[0]);
                                insert_node(parent.body, stopIterAst, returnStmtAst);
                                }
                                break;
                                case 'ContinueStatement':
                                if (continueMod.isReplaceReq()) {
                                replace_node(node, returnStmtAst);
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
                                --breakMod.stackIndex;
                                }
                                });
            var iter = esprima.parse(
                                     forOfNode.right.name +
                                     '.iter(function (' + (forOfNode.left.name ? forOfNode.left.name : forOfNode.left.declarations[0].id.name) + '){});'
                                     ).body[0];
            iter.expression.arguments[0].body = nodeCopy.body;
            
            return iter;
        }
        
        throw 'Invalid arg ' + mode + ' for get_iter_ast';
    }
    
    // Returns iterator consturct with dynamic type checking.
    function get_iter_compatible_ast(forOfNode) {
        // Make a copy of the 'for ... of ...' loop.
        var nodeCopy = deep_copy(forOfNode);
        
        // Iterator AST.
        var iterAst = get_iter_ast(nodeCopy, 'for_of');
        
        // 'if ... else ...' with dynamic type checking.
        var ifElseAst = esprima.parse('if(' + forOfNode.right.name + '.isInstance){}else{}').body[0];
        // Push the iterator AST into 'if' block.
        ifElseAst.consequent.body.push(iterAst);
        // Push the user-written 'for ... of ...' loop into 'else' block.
        ifElseAst.alternate.body.push(forOfNode);
        
        estraverse.traverse(ifElseAst, {
                            enter: function (node) {
                            node.isGen = true;
                            }
                            });
        
        // Traverse all the 'for ... of ...' loops in the 'else' block and mark them as visited - so that we don't recursively convert these into iterator constructs.
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
        
        // Match the regex against the query to find all the variables that are used.
        var matches = query.match(re);
        
        // Replace the :<var> with proper substitution.
        query = query.replace(re, '" + $1 + "');
        query = 'new N1qlQuery("' + query + '");';
        
        // Return the ast.
        return esprima.parse(query).body[0].expression;
    }
    
    function is_n1ql_node(node) {
        return /NewExpression/.test(node.type) &&
        /N1qlQuery/.test(node.callee.name);
    }
    
    var breakMod = new LoopModifier('break');
    var continueMod = new LoopModifier('continue');
    
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
                        // Modifies all the 'for ... of ...' constructs to work with iteration. Takes care to see to it that it visits the node only once.
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
