var fs = require('fs'),
	esprima = require('esprima'),
	estraverse = require('estraverse'),
	escodegen = require('escodegen');

var filename = process.argv[2],
	code = fs.readFileSync(filename, 'utf-8'),
	transpiledCode = transpile(code, filename);

console.log(transpiledCode);
esprima.parse(transpiledCode);

function saveTranspiledCode() {
	var tCodePath = filename.slice(0, filename.lastIndexOf('.js')) + '.t.js';
	fs.writeFileSync(tCodePath, transpiledCode.code);

	var smPath = filename.slice(0, filename.lastIndexOf('.js')) + '.map.json';
	fs.writeFileSync(smPath, transpiledCode.map);
}
// Uncomment this line to save the transpiled code and the corresponding source map for it.
// saveTranspiledCode();

function compile(code) {
	var ast = esprima.parse(code);
	return escodegen.generate(ast);	
}

function transpile(code, sourceFileName) {
	var ast = getAst(code, sourceFileName);
	return escodegen.generate(ast, {
		comment: true
	});
}

function jsFormat(code) {
	var ast = esprima.parse(code);
	return escodegen.generate(ast);
}

function isTimerCalled(code) {
	return isFuncCalled('docTimer', code) || isFuncCalled('cronTimer', code);
}

function getSourceMap(code, sourceFileName) {
	var ast = getAst(code, sourceFileName);
	return escodegen.generate(ast, {
		sourceMap: true,
		sourceMapWithCode: true
	}).map;
}

// Checks if a function is called.
function isFuncCalled(methodName, code) {
	// Get the Abstract Syntax Tree (ast) of the input code.
	var ast = esprima.parse(code, {
		attachComment: true,
		sourceType: 'script'
	});

	var methodExists = false;
	estraverse.traverse(ast, {
		enter: function (node) {
			if (!methodExists && /CallExpression/.test(node.type)) {
				methodExists = node.callee.name === methodName;
			}
		}
	});

	return methodExists;
}

// TODO : Handle the case when comment appears inside a string - /* this is 'a comm*/'ent */ - must be
// handled in the lex.
// TODO : Variables created in the iterator must be made available outside its scope.
// TODO : Need to call execQuery() if the query isn't a select query.
function getAst(code, sourceFileName) {
	// A utility class for handling nodes of an AST.
	function NodeUtils() {
		var self = this;
		// Performs deep copy of the given node.
		this.deepCopy = function (node) {
			return JSON.parse(JSON.stringify(node));
		};

		// Deletes a node from the body.
		this.deleteNode = function (parentBody, nodeToDel) {
			console.assert(parentBody instanceof Array, 'parentBody must be an Array');
			console.assert(parentBody.indexOf(nodeToDel) !== -1, 'node not found in the parent body');

			var deleteIndex = parentBody.indexOf(nodeToDel);
			parentBody.splice(deleteIndex, 1);
		};

		// Replaces source node with the target node and returns a reference to the new node.
		this.replaceNode = function (source, target, context) {
			var sourceCopy = self.deepCopy(source);

			Object.keys(source).forEach(function (key) {
				delete source[key];
			});
			Object.keys(target).forEach(function (key) {
				source[key] = target[key];
			});

			// Using this check temporarily.
			// if (!self.hasLocNode(sourceCopy)) {
			// 	return source;
			// }

			// Attach the loc nodes based on the context.
			switch (context) {
				// Mapping of loc nodes for N1qlQuery happens during the substitution of variables in the N1QL query string.
				/*
					Before:
					var res1 = new N1qlQuery('select * from :bucket LIMIT 10;');
					After:
					var res1 = new N1qlQuery('select * from $1 LIMIT 10;', {posParams : [bucket]});
				*/
				case Context.N1qlQuery:
					source.loc = self.deepCopy(sourceCopy.loc);
					source.callee.loc = self.deepCopy(sourceCopy.callee.loc);
					source.arguments[0].loc = self.deepCopy(sourceCopy.arguments[0].loc);
					break;

				// Mapping of loc nodes when a N1QL Query instantiation is reverted back to a JavaScript expression.
				/*
					Before:
					new N1qlQuery('delete bucket["key"]');
					After:
					delete bucket["key"];
				*/
				case Context.N1qlQueryRevert:
					self.setLocForAllNodes(sourceCopy.loc, source);
					break;
					// Mapping of if-else block to for-of loop.
					/*
						Before:
						for (var r of res1){...}
						After:
						if (res1.isInstance) {
							res1.iter(function (r) {...}
						} else {...}
					*/
				case Context.IterTypeCheck:
					source.loc = self.deepCopy(sourceCopy.loc);
					source.consequent.loc = self.deepCopy(sourceCopy.body.loc);
					source.test.loc = self.deepCopy(sourceCopy.right.loc);
					source.test.object.loc = self.deepCopy(sourceCopy.right.loc);
					source.test.property.loc = self.deepCopy(sourceCopy.right.loc);

					// TODO: Currently, after breaking out from labeled break statement, it goes to the beginning of the for-of loop.
					//		Ideally, it should go to the end of the labeled block. This looks quite ideal to show the iteration behaviour -
					//		It stops at the enclosing for-of loops (iterators) before coming out and thus, demonstrating the stopping
					//		of iteration. Need to ask whether this is ok or if the default behaviour is needed.
					if (source.consequent.body.length > 1 && /SwitchStatement/.test(source.consequent.body[1].type)) {
						self.forceSetLocForAllNodes(sourceCopy.loc, source.consequent.body[1]);
					}
					break;

					// The following case handles two-way mapping of loc nodes between continue and return statements.
				case Context.ContinueStatement:
					source.loc = self.deepCopy(sourceCopy.loc);
					switch (source.type) {
						// Return to continue statement mapping - source: return, target: continue
						case 'ContinueStatement':
							console.assert(/ReturnStatement/.test(sourceCopy.type), 'sourceCopy type must be ReturnStatement');
							if (source.label) {
								source.label.loc = self.deepCopy(sourceCopy.loc);
							}
							break;

							// Continue to return statement mapping - source: continue, target: return
						case 'ReturnStatement':
							console.assert(/ContinueStatement/.test(sourceCopy.type), 'sourceCopy type must be ContinueStatement');
							if (source.argument && sourceCopy.label.loc) {
								source.argument = self.setLocForAllNodes(sourceCopy.label.loc, source.argument);
							}
							break;

						default:
							throw 'Not yet handled for ' + source.type;
					}
					break;

					// The following case handles two-way mapping of loc nodes between break and return statements.
				case Context.BreakStatement:
					source.loc = self.deepCopy(sourceCopy.loc);
					switch (source.type) {
						// Return to break statement mapping - source: return, target: break
						case 'BreakStatement':
							console.assert(/ReturnStatement/.test(sourceCopy.type), 'sourceCopy type must be ReturnStatement');
							source.label.loc = self.deepCopy(sourceCopy.argument.loc);
							break;

							// Break to return statement mapping - source: break, target: return
						case 'ReturnStatement':
							console.assert(/BreakStatement/.test(sourceCopy.type), 'sourceCopy type must be BreakStatement');
							source.argument = self.setLocForAllNodes(sourceCopy.loc, source.argument);
							break;

						default:
							throw 'Not yet handled for ' + source.type;
					}
					break;

					// The following case handles mapping of loc nodes between two different 'stopIter' calls.
					/*
						Before:
						return res2.stopIter({
							'code': 'labeled_break',
							'args': 'x'
						});
						After:
						return res1.stopIter({
							'code': 'labeled_break',
							'args': 'x'
						});
					*/
				case Context.BreakAltInterrupt:
					console.assert(escodegen.generate(source.argument.arguments[0]) === escodegen.generate(sourceCopy.argument.arguments[0]), 'stopIter args must match');
					self.setLocMatchingNodes(sourceCopy, source);
					break;

					// The following case handles the mapping of loc nodes between stopIter and
					// return statement or between two stopIter statements as the above case.
					/*
						Before:
						return res2.stopIter({
								'code': 'labeled_continue',
								'args': 'x'
							});
						After:
						return;
					 */
				case Context.ContinueAltInterrupt:
					if (source.argument) {
						console.assert(escodegen.generate(source.argument.arguments[0]) === escodegen.generate(sourceCopy.argument.arguments[0]), 'stopIter args must match');
						self.setLocMatchingNodes(sourceCopy, source);
					} else {
						source.loc = sourceCopy.loc;
					}
					break;
			}

			return source;
		};

		// Checks if atleast one loc node is present in the AST.
		this.hasLocNode = function (ast) {
			var hasLoc = false;
			estraverse.traverse(ast, {
				enter: function (node) {
					if (hasLoc) {
						return;
					}

					hasLoc = node.loc;
				}
			});

			return hasLoc;
		};

		// Adds loc node for all the nodes in the AST.
		// Thus, all the nodes of AST might end up having unnecessary loc nodes.
		// Though this method wouldn't modify the parsing behaviour, it must be used as a last resort.
		this.forceSetLocForAllNodes = function (loc, ast) {
			estraverse.traverse(ast, {
				enter: function (node) {
					if (!node.loc) {
						node.loc = self.deepCopy(loc);
					}
				}
			});
		};

		// This is a safe method for adding loc nodes for a given AST.
		// The disadvantage is that it can not be used with all the AST.
		this.setLocForAllNodes = function (loc, ast) {
			// Generate the code snippet for the given AST.
			var codeSnippet = escodegen.generate(ast);
			// Parse with loc enabled to determine the nodes to which we can attach loc node.
			var astWithLoc = esprima.parse(codeSnippet, {
				loc: true
			});

			// We now traverse astWithLoc and replace all the loc nodes.
			estraverse.traverse(astWithLoc, {
				enter: function (node) {
					node.loc = node.loc ? nodeUtils.deepCopy(loc) : null;
				}
			});

			return astWithLoc.body[0].expression;
		};

		// This is a safe method for performing a one-to-one copy of the loc nodes from AST1 to AST2.
		// The two ASTs must have the same structure.
		this.setLocMatchingNodes = function (source, target) {
			var sourceNodeStack = new Stack(),
				targetNodeStack = new Stack();

			// Linearizes the given AST into a stack.
			function convertTreeToStack(ast, stack) {
				estraverse.traverse(ast, {
					enter: function (node) {
						stack.push(node);
					}
				});
			}

			convertTreeToStack(source, sourceNodeStack);
			convertTreeToStack(target, targetNodeStack);

			console.assert(sourceNodeStack.getSize() === targetNodeStack.getSize(), 'stack size must be same');
			// Pop all nodes from the sourceNodeStack and if an element contains loc node,
			// copy it to the corresponding element in the targetNodeStack.
			while (!sourceNodeStack.isEmpty()) {
				var sourceNode = sourceNodeStack.pop();
				var targetNode = targetNodeStack.pop();
				if (sourceNode.loc) {
					targetNode.loc = self.deepCopy(sourceNode.loc);
				}
			}
		};

		// Inserts the given node to the given parentBody at the specified index.
		this.insertNode = function (parentBody, refNode, nodeToInsert, insertAfter) {
			console.assert(parentBody instanceof Array, 'parentBody must be an Array');
			console.assert(parentBody.indexOf(refNode) !== -1, 'node not found in the parent body');
			var insertIndex = insertAfter ? parentBody.indexOf(refNode) + 1 : parentBody.indexOf(refNode);
			parentBody.splice(insertIndex, 0, nodeToInsert);
		};

		// A N1QL node is a statement of the form new N1qlQuery('...');
		this.isN1qlNode = function (node) {
			return /NewExpression/.test(node.type) && /N1qlQuery/.test(node.callee.name);
		};

		this.convertToBlockStmt = function (node) {
			switch (node.type) {
				case 'ForOfStatement':
					// Transform the previous single-line statement into a block.
					node.body.body = [nodeUtils.deepCopy(node.body)];
					node.body.type = 'BlockStatement';
					break;
				case 'IfStatement':
					node.consequent.body = [nodeUtils.deepCopy(node.consequent)];
					node.consequent.type = 'BlockStatement';
					// If the 'else' part exists, convert it to a block statement.
					if (node.alternate) {
						node.alternate.body = [nodeUtils.deepCopy(node.alternate)];
						node.alternate.type = 'BlockStatement';
					}
					break;
				default:
					throw 'unhandled case for: ' + node.type;
			}
		};

		// Inserts an array of AST nodes into parentBody at the specified index.
		this.insertNodeArray = function (parentBody, insAfterNode, arrayToInsert) {
			console.assert(parentBody instanceof Array, 'parentBody must be an Array');
			console.assert(arrayToInsert instanceof Array, 'arrayToInsert must be an Array');
			var insertIndex = parentBody.indexOf(insAfterNode) + 1;
			parentBody.splice.apply(parentBody, [insertIndex, 0].concat(arrayToInsert));
		};

		// Build an ast node for N1QL function call from the query.
		this.getQueryAst = function (query) {
			var qParser = new QueryParser(),
				subs = qParser.parse(query);
			return new N1QLQueryAst(subs.query, subs.placeholders);
		};

		// Checks if the global scope contains only function declarations.
		this.checkGlobals = function (ast) {
			for (var node of ast.body) {
				if (!/FunctionDeclaration/.test(node.type)) {
					throw 'Only function declaration are allowed in global scope';
				}
			}
		};

		// Checks if the N1QL query must be reverted back to JavaScript expression.
		this.isRevertReq = function (query) {
			// Checks if the given statement is a valid JavaScript expression.
			function isJsExpression(stmt) {
				try {
					esprima.parse(stmt);
					return true;
				} catch (e) {
					return false;
				}
			};

			// Check whether N1QL query begins with delete and is parsable as a
      // JavaScript expression.
			var tokens = query.split(/\s/g);
			if (tokens.length && tokens[0] === 'delete') {
				return isJsExpression(query);
			}

			return false;
		};
	}

	// A non-full-fledged parser to convert N1QL queries to parameterized queries.
	function QueryParser() {
		function isQuote(c) {
			return c === '\'' || c === '"' || c === '`';
		}

		// Back-tracks and finds if the i-th character is escaped.
		function isEscaped(query, i) {
			var escCount = 0;
			for (var j = i - 1; j >= 0; --j) {
				if (query[j] !== '\\') {
					break;
				}

				++escCount;
			}

			// A character is escaped if it has odd number of escape character preceding it.
			return escCount & 1;
		}

		// Parses placeholder having regex - :IDENT from the i-th character.
		function parsePlaceholder(query, i) {
			var re = /:([a-zA-Z_$][a-zA-Z_$0-9]*)/;
			var qMatch = re.exec(query.slice(i));
			if (qMatch && qMatch.index === 0) {
				return qMatch[1];
			}

			return null;
		}

		// Utility method to maintain quote stack.
		// Essentially, it keeps track of whether the i-th character is inside N1QL
		// string or not and executes the callback upon entry / exit of N1QL string.
		function manageQuoteStack(quoteStack, query, i, callback) {
			if (isQuote(query[i]) && !isEscaped(query, i)) {
				if (quoteStack.isEmpty()) {
					// Enter N1QL string.
					quoteStack.push(query[i]);
					if (callback && callback.enter) {
						callback.enter();
					}
				} else if (quoteStack.peek() === query[i]) {
					quoteStack.pop();
					// Exit N1QL string.
					if (callback && callback.exit) {
						callback.exit();
					}
				}
			}
		}

		this.parse = function (query) {
			var quoteStack = new Stack(),
				substitutedQuery = '',
				placeholders = [];

			// Parse and substitute placeholders with $NUM.
			for (var i = 0; i < query.length; ++i) {
				var substituted = false;
				manageQuoteStack(quoteStack, query, i);
				if (query[i] === ':' && quoteStack.isEmpty()) {
					var placeholder = parsePlaceholder(query, i);
					if (placeholder) {
						placeholders.push(placeholder);
						substitutedQuery += '$' + placeholders.length;
						substituted = true;
						i += placeholder.length;
					}
				}

				if (!substituted) {
					substitutedQuery += query[i];
				}
			}

			// Quote stack must be empty.
			console.assert(quoteStack.isEmpty(), 'Quote stack must be empty, otherwise, the string is malformed in the query');
			return {
				query: substitutedQuery,
				placeholders: placeholders
			};
		};
	}

	// A general purpose stack.
	function Stack() {
		var stack = [];

		this.stackCopy = stack;

		this.push = function (item) {
			stack.push(item);
		};

		this.pop = function () {
			if (stack.length === 0) {
				throw 'Stack underflow exception';
			}
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
				clone.push(nodeUtils.deepCopy(item));
			}

			return clone;
		};

		this.reverseElements = function () {
			stack.reverse();
		};

		this.isEmpty = function () {
			return stack.length === 0;
		};

		// debug.
		this.printAll = function () {
			for (var item of stack) {
				console.log(item);
				console.log();
			}
		}
	}

	// A sub-class of Stack. Its purpose it to maintain the ancestral information
	// of nodes : node-parent relationship.
	function AncestorStack() {
		Stack.call(this);

		// Returns or pops a node that satisfies the comparator.
		function getOrPopAncestor(_this, comparator, pop) {
			var tempStack = new Stack();
			var found = false;

			// Run through all the elements in the stack against the comparator and break out once the element is found.
			while (_this.getSize() > 0 && !found) {
				var node = _this.pop();
				tempStack.push(node);
				found = comparator(nodeUtils.deepCopy(node));
			}

			if (pop) {
				tempStack.pop();
			}

			// Restore everything back to the stack.
			while (tempStack.getSize() > 0) {
				_this.push(tempStack.pop());
			}

			return found ? node : null;
		}

		this.getAncestor = function (comparator) {
			return getOrPopAncestor(this, comparator, false);
		};

		this.popNode = function (comparator) {
			return getOrPopAncestor(this, comparator, true);
		};

		// Returns the topmost node of the given type. Need not necessarily be the top of stack.
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
		};
	}

	// Class to maintain the association of loop modifiers - break, continue, return etc. with JavaScript loops.
	function LoopModifier(modifier) {
		var ancestorStack = new AncestorStack();
		this.modType = modifier;
		this.stackIndex = -1;

		// debug.
		this.stackCopy = ancestorStack.stackCopy;

		// Initializing association information.
		// Obtained from JavaScript reference - https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference
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

			default:
				throw 'Invalid modifier';
		}

		this.checkAssoc = function (nodeType) {
			return associations.has(nodeType);
		};

		// Push the node if it associates with the loop modifier instance.
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

					default:
						throw 'Invalid modifier type';
				}
			}
		};


		this.getSize = function () {
			return ancestorStack.getSize();
		};

		// Returns a boolean suggesting whether the loop modifier needs to be replaced.
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

				default:
					throw 'Invalid modifier type';
			}
		};

		// debug.
		this.printAll = function () {
			ancestorStack.printAll();
		}
	}

	// LoopModifier types.
	LoopModifier.CONST = {
		BREAK: 'break',
		CONTINUE: 'continue',
		LABELED_BREAK: 'labeled_break',
		RETURN: 'return',
		LABELED_CONTINUE: 'labeled_continue'
	};

	Context = {
		N1qlQuery: 'n1ql_query',
		N1qlQueryRevert: 'n1ql_query_revert',
		IterTypeCheck: 'iter_type_check',
		BreakStatement: 'break_statement',
		BreakAltInterrupt: 'break_alt_interrupt',
		ContinueStatement: 'continue_statement',
		ContinueAltInterrupt: 'continue_alt_interrupt',
		ReturnStatement: 'return_statement',
		ReturnAltFound: 'return_alt_found',
		ReturnAltInterrupt: 'return_alt_interrupt',
		IterConsequent: 'iter_consequent'
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

		// Targeted search - search the stack for the target, but stop and return if the stop-condition is met.
		// Comparator must define targetComparator and stopComparator and each of them should return a boolean.
		// Setting searchAll to true will return all the stopNodes till the targetNode is encountered.
		function search(_this, comparator, searchAll) {
			var tempStack = new Stack(),
				stopNodes = [],
				returnArgs = {
					targetFound: false
				};

			while (_this.ancestorStack.getSize() > 0) {
				var node = _this.ancestorStack.pop();
				tempStack.push(node);

				if (comparator.targetComparator(nodeUtils.deepCopy(node))) {
					returnArgs = {
						targetFound: true,
						stopNode: nodeUtils.deepCopy(node),
						searchInterrupted: false
					};
					break;
				} else if (comparator.stopComparator(nodeUtils.deepCopy(node))) {
					returnArgs = {
						targetFound: false,
						stopNode: nodeUtils.deepCopy(node),
						searchInterrupted: true
					};
					if (searchAll) {
						stopNodes.push(returnArgs.stopNode);
					} else {
						break;
					}
				}
			}

			// Restore the elements back to the stack.
			while (tempStack.getSize() > 0) {
				_this.ancestorStack.push(tempStack.pop());
			}

			// A check to validate that targetFound and searchInterrupted are mutually exclusive.
			if (returnArgs.targetFound && returnArgs.searchInterrupted) {
				throw 'Invalid case: targetFound=' + returnArgs.targetFound +
					'\tsearchInterrupted=' + returnArgs.searchInterrupted;
			}

			if (searchAll && stopNodes.length > 0) {
				returnArgs = {
					targetFound: false,
					stopNodes: stopNodes,
					searchInterrupted: true
				};
			}

			return returnArgs;
		}

		this.searchStack = function (comparator) {
			return search(this, comparator);
		};

		this.searchAllStopNodes = function (comparator) {
			return search(this, comparator, true);
		}
	}

	// Data types JavaScript AST nodes - http://esprima.readthedocs.io/en/latest/syntax-tree-format.html
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

	function MemExprAst(objName) {
		Ast.call(this, 'MemberExpression');
		this.computed = false;
		this.object = {
			"type": "Identifier",
			"name": objName
		};
		this.property = {
			"type": "Identifier",
			"name": "isInstance"
		};
	}

	function IfElseAst(memExprAst) {
		Ast.call(this, 'IfStatement');
		this.test = memExprAst;
		this.consequent = {
			"type": "BlockStatement",
			"body": []
		};
		this.alternate = {
			"type": "BlockStatement",
			"body": []
		};
	}

	// Returns AST of the form 'iterVar'.'prop()'.data
	function ReturnDataAst(iterVar, prop) {
		Ast.call(this, 'ExpressionStatement');
		this.expression = {
			"type": "MemberExpression",
			"computed": false,
			"object": {
				"type": "CallExpression",
				"callee": {
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
				"arguments": []
			},
			"property": {
				"type": "Identifier",
				"name": "data"
			}
		};
	}

	// Returns AST of the form -
	/*
		res.iter(function(row){...});
	 */
	function IteratorSkeletonAst(iterVar, arg) {
		Ast.call(this, 'ExpressionStatement');
		this.expression = {
			"type": "CallExpression",
			"callee": {
				"type": "MemberExpression",
				"computed": false,
				"object": {
					"type": "Identifier",
					"name": iterVar
				},
				"property": {
					"type": "Identifier",
					"name": "iter"
				}
			},
			"arguments": [{
				"type": "FunctionExpression",
				"id": null,
				"params": [{
					"type": "Identifier",
					"name": arg
				}],
				"body": {
					"type": "BlockStatement",
					"body": []
				},
				"generator": false,
				"expression": false,
				"async": false
			}]
		}
	}

	function BlockStatementAst(body) {
		Ast.call(this, 'BlockStatement');
		this.body = [body];
	}

	function N1QLQueryAst(query, posParams) {
		Ast.call(this, 'NewExpression');
		this.callee = {
			"type": "Identifier",
			"name": "N1qlQuery"
		};
		this.arguments = [{
				"type": "Literal",
				"value": query
			},
			{
				"type": "ObjectExpression",
				"properties": [{
					"type": "Property",
					"key": {
						"type": "Identifier",
						"name": "posParams"
					},
					"computed": false,
					"value": {
						"type": "ArrayExpression",
						"elements": []
					},
					"kind": "init",
					"method": false,
					"shorthand": false
				}]
			}
		];

		for (var param of posParams) {
			this.arguments[1].properties[0].value.elements.push({
				"type": "Identifier",
				"name": param
			});
		}
	}

	// Class for maintaining the object that will be passed to 'stopIter'.
	function Arg(arg) {
		this.code = arg.code;
		this.args = arg.args;

		this.getAst = function () {
			// Need to wrap 'arg' inside '()' to turn it into a statement - it becomes a JSON object otherwise.
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

	// Class to generate post iteration steps - switch-case block.
	function PostIter(iterProp, returnBubbleFunc) {
		var stmts = [];

		this.iterProp = iterProp;
		this.returnBubbleFunc = returnBubbleFunc;

		this.push = function (arg) {
			// Avoid duplicates while insertion.
			// TODO :   replace the list with a set in the final version - for faster lookup.
			if (stmts.indexOf(arg) === -1) {
				stmts.push(arg);
			}
		};

		// Returns a switch-case block to perform post-iteration steps.
		this.getAst = function (iterVar, stackHelper) {
			var discriminantAst = esprima.parse(iterVar + '.' + this.iterProp).body[0].expression,
				switchAst = new SwitchAst(discriminantAst),
				postIter, caseAst, lookup, stopIterAst, arg, returnStmtAst, pushCase;

			// Loop over all the stmts and generate the corresponding 'case' block.
			for (var postIterStmt of stmts) {
				pushCase = true;
				// TODO :   Changing 'var postIter' to 'const postIter' causes a unit test to fail. Investigate this issue.
				postIter = JSON.parse(postIterStmt);
				caseAst = new CaseAst(postIter.code + postIter.args);

				switch (postIter.code) {
					case LoopModifier.CONST.BREAK:
					case LoopModifier.CONST.CONTINUE:
						break;
					case LoopModifier.CONST.LABELED_BREAK:
						// Search the ancestor stack for the label. Interrupt the search if a for-of node is found.
						lookup = stackHelper.searchStack({
							targetComparator: function (node) {
								return /LabeledStatement/.test(node.type) && node.label.name === postIter.args;
							},
							stopComparator: function (node) {
								return /ForOfStatement/.test(node.type);
							}
						});
						// If the label is found and doesn't point to the for-of node, then add a break <label>.
						if (lookup.targetFound) {
							console.assert(lookup.stopNode.label.name === postIter.args, 'labels must match');

							if (/ForOfStatement/.test(lookup.stopNode.body.type)) {
								pushCase = false;
							} else {
								caseAst.consequent.push(new LabeledBreakAst(postIter.args));
							}
						}
						// If the search was interrupted, then it means that it encountered a for-of node. So, add a
						// 'return stopIter' node.
						if (lookup.searchInterrupted) {
							console.assert(/ForOfStatement/.test(lookup.stopNode.type), 'must be a for-of node');

							stopIterAst = new StopIterAst(lookup.stopNode.right.name);
							arg = new Arg({
								code: LoopModifier.CONST.LABELED_BREAK,
								args: postIter.args
							});
							returnStmtAst = new ReturnAst(stopIterAst);
							stopIterAst.arguments.push(arg.getAst());

							// Annotate the node as it is subjected to change in the next iteration.
							returnStmtAst.isAnnotated = true;
							returnStmtAst.metaData = postIter;

							caseAst.consequent.push(returnStmtAst);
						}
						break;
					case LoopModifier.CONST.LABELED_CONTINUE:
						// This is very similar to Labeled break case.
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
								caseAst.consequent.push(new LabeledContinueAst(postIter.args));
							}
						}
						if (lookup.searchInterrupted) {
							console.assert(/ForOfStatement/.test(lookup.stopNode.type), 'must be a for-of node');

							if (lookup.stopNode.parentLabel === postIter.args) {
								returnStmtAst = new ReturnAst(null);
							} else {
								stopIterAst = new StopIterAst(lookup.stopNode.right.name);
								arg = new Arg({
									code: LoopModifier.CONST.LABELED_CONTINUE,
									args: postIter.args
								});
								returnStmtAst = new ReturnAst(stopIterAst);
								stopIterAst.arguments.push(arg.getAst());
							}

							returnStmtAst.isAnnotated = true;
							returnStmtAst.metaData = postIter;

							caseAst.consequent.push(returnStmtAst);
						}
						break;
					case LoopModifier.CONST.RETURN:
						// Target is to find the function name that binds to the 'return' node. Search is interrupted by
						// a for-of node.
						lookup = stackHelper.searchStack({
							targetComparator: function (item) {
								return (/FunctionDeclaration/.test(item.type) || /FunctionExpression/.test(item.type)) &&
									(item.id ? item.id.name : null) === postIter.targetFunction;
							},
							stopComparator: function (item) {
								return /ForOfStatement/.test(item.type);
							}
						});
						if (lookup.targetFound) {
							console.assert(/FunctionDeclaration/.test(lookup.stopNode.type) || /FunctionExpression/.test(lookup.stopNode.type), 'node must be a function');

							returnStmtAst = new ReturnAst(new ReturnDataAst(postIter.iterVar, this.returnBubbleFunc));
						}
						if (lookup.searchInterrupted) {
							console.assert(/ForOfStatement/.test(lookup.stopNode.type), 'must be a for-of node');

							stopIterAst = new StopIterAst(lookup.stopNode.right.name);
							arg = new Arg({
								code: LoopModifier.CONST.RETURN,
								args: postIter.iterVar + '.' + this.returnBubbleFunc + '().data',
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
	}

	function Iter(forOfNode) {
		var _this = this;
		var breakMod = new LoopModifier(LoopModifier.CONST.BREAK);
		var continueMod = new LoopModifier(LoopModifier.CONST.CONTINUE);
		var lblBreakMod = new LoopModifier(LoopModifier.CONST.LABELED_BREAK);
		var returnMod = new LoopModifier(LoopModifier.CONST.RETURN);
		var lblContinueMod = new LoopModifier(LoopModifier.CONST.LABELED_CONTINUE);

		this.nodeCopy = nodeUtils.deepCopy(forOfNode);

		this.incrAndPush = function (node) {
			++breakMod.stackIndex;
			++continueMod.stackIndex;
			++lblBreakMod.stackIndex;
			++returnMod.stackIndex;
			++lblContinueMod.stackIndex;

			breakMod.pushIfAssoc(node);
			continueMod.pushIfAssoc(node);
			lblBreakMod.pushIfAssoc(node);
			returnMod.pushIfAssoc(node);
			lblContinueMod.pushIfAssoc(node);
		};

		this.decrAndPop = function () {
			breakMod.popIfAssoc();
			continueMod.popIfAssoc();
			lblBreakMod.popIfAssoc();
			returnMod.popIfAssoc();
			lblContinueMod.popIfAssoc();

			--breakMod.stackIndex;
			--continueMod.stackIndex;
			--lblBreakMod.stackIndex;
			--returnMod.stackIndex;
			--lblContinueMod.stackIndex;
		};

		this.traverse = function (traversal) {
			estraverse.traverse(_this.nodeCopy, {
				enter: function (node) {
					traversal(node, _this.nodeCopy, breakMod, continueMod, lblBreakMod, lblContinueMod, returnMod);
				},
				leave: function (node) {
					_this.decrAndPop();
				}
			});
		};

		// debug.
		this.assertEmpty = function () {
			console.assert(breakMod.getSize() === 0, 'breakMod must be empty');
			console.assert(continueMod.getSize() === 0, 'continueMod must be empty');
			console.assert(lblBreakMod.getSize() === 0, 'lblBreakMod must be empty');
			console.assert(returnMod.getSize() === 0, 'returnMod must be empty');
			console.assert(lblContinueMod.getSize() === 0, 'lblContinueMod must be empty');
		}
	}

	// Returns if-else AST having iterator in consequent and for-of in alternate (dynamic type checking).
	function IterCompatible(forOfNode) {
		var self = this;
		var stackHelper = new StackHelper(globalAncestorStack);

		// Returns an iterator construct for a given for-of loop ast.
		this.getIterConsequentAst = function () {
			var iterator = new Iter(forOfNode);

			// This is the property that will be set on the N1qlQuery instance - contains return value of iterator.
			var iterProp = 'getReturnValue(true)';
			var returnBubbleFunc = 'getReturnValue';

			// List to store post iteration exit statements.
			var postIter = new PostIter(iterProp, returnBubbleFunc);

			iterator.traverse(function (node, nodeCopy, breakMod, continueMod, lblBreakMod, lblContinueMod, returnMod) {
				iterator.incrAndPush(node);

				var arg,
					stopIterAst,
					returnStmtAst;

				// Annotated nodes are those nodes that have been marked to be changed by the previous iteration.
				if (node.isAnnotated) {
					switch (node.metaData.code) {
						case LoopModifier.CONST.RETURN:
							// For 'return', the 'iterVar' must be set to the current for-of loop's source.
							node.metaData.iterVar = nodeCopy.right.name;
							arg = JSON.stringify(node.metaData);
							break;

						case LoopModifier.CONST.LABELED_BREAK:
						case LoopModifier.CONST.LABELED_CONTINUE:
							arg = new Arg({
								code: node.metaData.code,
								args: node.metaData.args
							});
							break;

						default:
							throw 'Unhandled case: ' + node.metaData.code;
					}

					postIter.push(arg.toString());

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
						/*
							Before:
							break x;
							After:
							return res.stopIter({
								'code': 'labeled_break',
								'args': 'x'
							});
					 	*/
						if (node.label && lblBreakMod.isReplaceReq(node.label.name)) {
							stopIterAst = new StopIterAst(nodeCopy.right.name);
							arg = new Arg({
								code: LoopModifier.CONST.LABELED_BREAK,
								args: node.label.name
							});
							postIter.push(arg.toString());
						} else if (!node.label && breakMod.isReplaceReq()) {
							// Unlabeled break statement.
							/*
								Before:
								break;
								After:
								return res.stopIter({ 'code': 'break' });
							 */
							stopIterAst = new StopIterAst(nodeCopy.right.name);
							arg = new Arg({
								code: LoopModifier.CONST.BREAK
							});
						}

						if (stopIterAst && arg) {
							returnStmtAst = new ReturnAst(stopIterAst);
							// Add 'arg' as the argument to 'stopIter()'.
							stopIterAst.arguments.push(arg.getAst());
							nodeUtils.replaceNode(node, returnStmtAst, Context.BreakStatement);
						}
						break;

					case 'ContinueStatement':
						// Labeled continue statement.
						/*
							Before:
							continue x;
							After:
							return res.stopIter({
								'code': 'labeled_continue',
								'args': 'x'
							});
						 */
						if (node.label && lblContinueMod.isReplaceReq(node.label.name)) {
							if (nodeCopy.parentLabel === node.label.name) {
								// If the target of labeled continue is its immediate parent, then just 'return'.
								returnStmtAst = new ReturnAst(null);
							} else {
								arg = new Arg({
									code: LoopModifier.CONST.LABELED_CONTINUE,
									args: node.label.name
								});
								stopIterAst = new StopIterAst(nodeCopy.right.name);
								returnStmtAst = new ReturnAst(stopIterAst);
								stopIterAst.arguments.push(arg.getAst());

								postIter.push(arg.toString());
							}

							nodeUtils.replaceNode(node, returnStmtAst, Context.ContinueStatement);
						} else if (continueMod.isReplaceReq()) {
							// Unlabeled continue statement.
							/*
								Before:
								continue;
								After:
								return;
							 */
							nodeUtils.replaceNode(node, new ReturnAst(null), Context.ContinueStatement);
						}
						break;

					case 'ReturnStatement':
						/*
							Before:
							return a + b;
							After:
							return res.stopIter({
								'code': 'return',
								'args': '(a + b)',
								'data': a + b
							});
						 */
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
							self.mapSourceNode(node, returnStmtAst, Context.ReturnStatement);

							var postIterArgs = JSON.stringify({
								code: LoopModifier.CONST.RETURN,
								args: arg.args,
								iterVar: nodeCopy.right.name,
								targetFunction: node.targetFunction
							});

							postIter.push(postIterArgs);
							nodeUtils.replaceNode(node, returnStmtAst);
						}
						break;

					case 'IfStatement':
						if (!/BlockStatement/.test(node.consequent.type)) {
							nodeUtils.convertToBlockStmt(node);
						}
						break;
				}
			});

			var iter = new IteratorSkeletonAst(forOfNode.right.name, (forOfNode.left.name ? forOfNode.left.name : forOfNode.left.declarations[0].id.name));
			iter.expression.arguments[0].body = iterator.nodeCopy.body;
			self.mapSourceNode(forOfNode, iter, Context.IterConsequent);

			var iterBlockAst = new BlockStatementAst(iter);

			// Pop the top for-of node.
			stackHelper.popTopForOfNode();

			var postIterAst = postIter.getAst(forOfNode.right.name, stackHelper);
			if (postIterAst) {
				iterBlockAst.body.push(postIterAst);
			}

			iterator.assertEmpty();

			return iterBlockAst;
		};

		// Maps loc nodes of source to target according to the context.
		this.mapSourceNode = function (source, target, context) {
			switch (context) {
				// Maps the source to target loc during the following kind of transformation -
				/*
					Before:
					for (var r of res3){...}
					After:
					res.iter(function (r) {...}
				 */
				case Context.IterConsequent:
					target.loc = nodeUtils.deepCopy(source.loc);
					target.expression.loc = nodeUtils.deepCopy(source.loc);
					target.expression.callee.loc = nodeUtils.deepCopy(source.loc);
					target.expression.callee.object.loc = nodeUtils.deepCopy(source.right.loc);
					target.expression.callee.property.loc = nodeUtils.deepCopy(source.right.loc);
					target.expression.arguments[0].loc = nodeUtils.deepCopy(source.right.loc);
					target.expression.arguments[0].params[0].loc = nodeUtils.deepCopy(source.left.declarations[0].id.loc);
					break;

					// Maps the source to target loc during the following kind of transformation -
					/*
						source: return function () {
							return inner;
						};
						target: return res1.stopIter({
							'code': 'return',
							'args': '(function () {\n    return inner;\n})',
							'data': function () {
								return inner;
							}
						});
					 */
				case Context.ReturnStatement:
					console.assert(target.argument, 'target must have an argument');
					target.loc = source.loc;
					nodeUtils.forceSetLocForAllNodes(source.loc, target.argument);
					if (source.argument) {
						for (var prop of target.argument.arguments[0].properties) {
							if (prop.key.value === 'data') {
								console.assert(escodegen.generate(source.argument) === escodegen.generate(prop.value), 'replacing unequal values');
								prop.value = nodeUtils.deepCopy(source.argument);
								break;
							}
						}
					}
					break;

					// Maps the source to target loc during the following kind of transformation -
					/*
						source: return res.stopIter({
							'code': 'return',
							'args': 'res.getReturnValue().data',
							'data': res.getReturnValue().data
						});
						target: return res.getReturnValue().data;
					 */
				case Context.ReturnAltFound:
					console.assert(source.argument, 'source must have an argument');
					target.loc = source.loc;
					for (var prop of source.argument.arguments[0].properties) {
						if (prop.key.value === 'data') {
							console.assert(escodegen.generate(target.argument) === escodegen.generate(prop.value), 'replacing unequal values');
							target.argument = nodeUtils.deepCopy(prop.value);
							break;
						}
					}
					break;

					// Maps the source to target loc during the following kind of transformation -
					/*
						source: return res1.stopIter({
							'code': 'return',
							'args': 'res.getReturnValue().data',
							'data': res.getReturnValue().data
						});
						target: return res2.stopIter({
							'code': 'return',
							'args': 'res.getReturnValue().data',
							'data': res.getReturnValue().data
						});
					 */
				case Context.ReturnAltInterrupt:
					console.assert(escodegen.generate(source.argument.arguments[0]) === escodegen.generate(target.argument.arguments[0]), 'stopIter args must match');
					nodeUtils.setLocMatchingNodes(source, target);
					break;
				default:
					throw 'Unhandled case: ' + context;
			}
		};

		// Returns AST for 'else' block.
		this.getIterAlternateAst = function () {
			var iterator = new Iter(forOfNode);
			iterator.traverse(function (node, nodeCopy, breakMod, continueMod, lblBreakMod, lblContinueMod, returnMod) {
				var lookup, stopIterAst, arg, returnStmtAst, stopNode = null,
					context;

				if (node.isAnnotated) {
					// Targeted lookup for annotated nodes.
					lookup = stackHelper.searchStack({
						targetComparator: function (item) {
							switch (node.metaData.code) {
								case LoopModifier.CONST.RETURN:
									// For a 'return' statement, the target is to find the function that the 'return'
									// statement was associated with, before transpilation.
									return (/FunctionDeclaration/.test(item.type) || /FunctionExpression/.test(item.type)) &&
										item.id.name === node.metaData.targetFunction;
								case LoopModifier.CONST.LABELED_CONTINUE:
								case LoopModifier.CONST.LABELED_BREAK:
									return /LabeledStatement/.test(item.type) && item.label.name === node.metaData.args;
								default:
									throw 'Unhandled case: ' + node.metaData.code;
							}
						},
						stopComparator: function (item) {
							return /ForOfStatement/.test(item.type);
						}
					});
					if (lookup.targetFound) {
						switch (node.metaData.code) {
							case LoopModifier.CONST.LABELED_BREAK:
								nodeUtils.replaceNode(node, new LabeledBreakAst(node.metaData.args), Context.BreakStatement);
								break;
							case LoopModifier.CONST.LABELED_CONTINUE:
								nodeUtils.replaceNode(node, new LabeledContinueAst(node.metaData.args), Context.ContinueStatement);
								break;
							case LoopModifier.CONST.RETURN:
								arg = new Arg({
									code: LoopModifier.CONST.RETURN,
									args: node.metaData.args,
									appendData: true
								});
								returnStmtAst = new ReturnAst(arg.getDataAst());
								self.mapSourceNode(node, returnStmtAst, Context.ReturnAltFound);
								nodeUtils.replaceNode(node, returnStmtAst);
								break;
						}
					}
					if (lookup.searchInterrupted) {
						console.assert(/ForOfStatement/.test(lookup.stopNode.type), 'must be a for-of node');
						switch (node.metaData.code) {
							case LoopModifier.CONST.LABELED_BREAK:
								stopIterAst = new StopIterAst(lookup.stopNode.right.name);
								arg = new Arg({
									code: node.metaData.code,
									args: node.metaData.args
								});
								stopIterAst.arguments.push(arg.getAst());
								returnStmtAst = new ReturnAst(stopIterAst);
								context = Context.BreakAltInterrupt;
								break;

							case LoopModifier.CONST.LABELED_CONTINUE:
								if (lookup.stopNode.parentLabel === node.metaData.args) {
									returnStmtAst = new ReturnAst(null);
								} else {
									stopIterAst = new StopIterAst(lookup.stopNode.right.name);
									arg = new Arg({
										code: node.metaData.code,
										args: node.metaData.args
									});
									stopIterAst.arguments.push(arg.getAst());
									returnStmtAst = new ReturnAst(stopIterAst);
								}
								context = Context.ContinueAltInterrupt;
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
								self.mapSourceNode(node, returnStmtAst, Context.ReturnAltInterrupt);
								break;
						}

						returnStmtAst.isAnnotated = true;
						returnStmtAst.metaData = node.metaData;

						nodeUtils.replaceNode(node, returnStmtAst, context);

					}

					return;
				}

				iterator.incrAndPush(node);

				switch (node.type) {
					case 'BreakStatement':
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
								arg = new Arg({
									code: LoopModifier.CONST.LABELED_BREAK,
									args: node.label.name
								});
								returnStmtAst = new ReturnAst(stopIterAst);
								stopIterAst.arguments.push(arg.getAst());

								returnStmtAst.isAnnotated = true;
								returnStmtAst.metaData = {
									code: LoopModifier.CONST.LABELED_BREAK,
									args: node.label.name
								};
								nodeUtils.replaceNode(node, returnStmtAst, Context.BreakStatement);
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
									arg = new Arg({
										code: LoopModifier.CONST.LABELED_CONTINUE,
										args: node.label.name
									});
									returnStmtAst = new ReturnAst(stopIterAst);
									stopIterAst.arguments.push(arg.getAst());
								}

								returnStmtAst.isAnnotated = true;
								returnStmtAst.metaData = {
									code: LoopModifier.CONST.LABELED_CONTINUE,
									args: node.label.name
								};
								nodeUtils.replaceNode(node, returnStmtAst, Context.ContinueStatement);
							}
						}
						break;
					case 'ReturnStatement':
						if (node.targetFunction) {
							lookup = stackHelper.searchStack({
								targetComparator: function (item) {
									return (/FunctionDeclaration/.test(item.type) || /FunctionExpression/.test(item.type)) &&
										item.id.name === node.targetFunction;
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
								self.mapSourceNode(node, returnStmtAst, Context.ReturnStatement);

								returnStmtAst.isAnnotated = true;
								returnStmtAst.metaData = {
									code: LoopModifier.CONST.RETURN,
									args: arg.args,
									iterVar: nodeCopy.right.name,
									targetFunction: node.targetFunction
								};

								nodeUtils.replaceNode(node, returnStmtAst);
							}
						}
						break;
				}
			});

			iterator.assertEmpty();

			return iterator.nodeCopy.parentLabel ? new LabeledStmtAst(iterator.nodeCopy.parentLabel, iterator.nodeCopy) : iterator.nodeCopy;
		};

		this.getAst = function () {
			// if-else block which perform dynamic type checking.
			var ifElseAst = new IfElseAst(new MemExprAst(forOfNode.right.name));

			// Iterator AST.
			var iterConsequentAst = this.getIterConsequentAst();
			// Push the iterator AST into 'if' block.
			ifElseAst.consequent.body = iterConsequentAst.body;
			// Push the user-written 'for ... of ...' loop into 'else' block.
			ifElseAst.alternate.body.push(this.getIterAlternateAst());

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
		};
	}

	var nodeUtils = new NodeUtils();
	var globalAncestorStack = new AncestorStack();

	// Get the Abstract Syntax Tree (ast) of the input code.
	var ast = esprima.parse(code, {
		range: true,
		tokens: true,
		comment: true,
		sourceType: 'script',
		loc: true,
		source: sourceFileName
	});

	// Attaching comments is a separate step.
	ast = escodegen.attachComments(ast, ast.comments, ast.tokens);

	// nodeUtils.checkGlobals(ast);

	estraverse.traverse(ast, {
		enter: function (node, parent) {
			globalAncestorStack.push(node);

			// Grab the for-of statement's label and mark the label for deletion.
			if (/ForOfStatement/.test(node.type) && !node.isVisited && /LabeledStatement/.test(parent.type)) {
				node.parentLabel = parent.label.name;
				parent.remLabel = true;
			}

			// Find the function that the 'return' statement associates with.
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
			if (nodeUtils.isN1qlNode(node) && node.arguments.length > 0) {
				var query = node.arguments[0].value;
				if (nodeUtils.isRevertReq(query)) {
					// Revert the query back to JavaScript expression if necessary.
					var ast = esprima.parse(query).body[0].expression;
					nodeUtils.replaceNode(node, nodeUtils.deepCopy(ast), Context.N1qlQueryRevert);
				} else {
					var ast = nodeUtils.getQueryAst(query);
					nodeUtils.replaceNode(node, nodeUtils.deepCopy(ast), Context.N1qlQuery);
				}
			}

			// TODO : Handle the case when the source of for-of loop is of type x.y
			// Modifies all the for-of statements to support iteration.
			// Takes care to see to it that it visits the node only once.
			if (/ForOfStatement/.test(node.type) && !node.isVisited) {
				if (!/BlockStatement/.test(node.body.type)) {
					nodeUtils.convertToBlockStmt(node);
				}

				var iterator = new IterCompatible(node);
				var iterAst = iterator.getAst();
				nodeUtils.replaceNode(node, nodeUtils.deepCopy(iterAst), Context.IterTypeCheck);
			} else if (/LabeledStatement/.test(node.type) && node.remLabel) {
				// Delete the label.
				nodeUtils.replaceNode(node, node.body);
			}

			globalAncestorStack.pop();
		}
	});

	return ast;
}