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

function QueryParser() {
	function isQuote(c) {
		return c === '\'' || c === '"' || c === '`';
	}

	function isEscaped(query, i) {
		var escCount = 0;
		for (var j = i - 1; j >= 0; --j) {
			if (query[j] !== '\\') {
				break;
			}

			++escCount;
		}

		return escCount & 1;
	}

	function parsePlaceholder(query, i) {
		var re = /:([a-zA-Z_$][a-zA-Z_$0-9]*)/;
		var qMatch = re.exec(query.slice(i));
		if (qMatch && qMatch.index === 0) {
			return qMatch[1];
		}

		return null;
	}

	function manageQuoteStack(quoteStack, query, i, callback) {
		if (isQuote(query[i]) && !isEscaped(query, i)) {
			if (quoteStack.isEmpty()) {
				quoteStack.push(query[i]);
				if (callback && callback.if) {
					callback.if();
				}
			} else if (quoteStack.peek() === query[i]) {
				quoteStack.pop();
				if (callback && callback.else) {
					callback.else();
				}
			}
		}
	}

	this.parse = function (query) {
		var quoteStack = new Stack(),
			substitutedQuery = '',
			placeholders = [];

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

	this.getStrLoc = function (query) {
		var quoteStack = new Stack();
		var strLoc = [];

		for (var i = 0; i < query.length; ++i) {
			manageQuoteStack(quoteStack, query, i, {
				if: function () {
					strLoc.push({
						start: i
					});
				},
				else: function () {
					strLoc[strLoc.length - 1].stop = i;
				}
			});
		}

		// Quote stack must be empty.
		console.assert(quoteStack.isEmpty(), 'Quote stack must be empty, otherwise, the string is malformed in the query');
		return strLoc;
	};
}

function Bucket(name, operation) {
	this.name = name;
	this.operation = operation;
}

function Warning(operation, loc, desc) {
	this.operation = operation;
	this.loc = loc;
	this.desc = desc;
}

function Query(query) {
	var self = this,
		qParser = new QueryParser(),
		subs = qParser.parse(query);

	this.query = subs.query;
	this.placeholders = subs.placeholders;
	this.strLoc = qParser.getStrLoc(this.query);

	function getAllTokens() {
		var re = /(\b[^\s]+\b)/g,
			tokens = [],
			token = re.exec(self.query);

		while (token) {
			tokens.push(token);
			token = re.exec(self.query);
		}

		return tokens;
	}

	function strLocRange(token, callback) {
		for (var range of self.strLoc) {
			if (range.start <= token.index && token.index <= range.stop) {
				console.assert(token.index + token[0].length - 1 <= range.stop, 'The token must be within string');
				if (callback && callback.in) {
					callback.in();
				}

				return true;
			} else {
				if (callback && callback.out) {
					callback.out();
				}

				return false;
			}
		}
	}

	this.getBuckets = function () {
		var buckets = [],
			warnings = [],
			tokens = getAllTokens(),
			keyspaceRefOffset = {
				INSERT: 2,
				UPSERT: 2,
				DELETE: 2,
				UPDATE: 1,
				MERGE: 2
			};

		for (var i = 0; i < tokens.length; ++i) {
			var token = tokens[i];
			token[0] = token[0].toUpperCase()
			if (token[0] in keyspaceRefOffset) {
				strLocRange(token, {
					out: function () {
						if (i + keyspaceRefOffset[token[0]] < tokens.length) {
							buckets.push(new Bucket(tokens[i + keyspaceRefOffset[token[0]]][0], token[0]));
						} else {
							var loc = {
								index: token.index + token[0].length
							};
							warnings.push(new Warning(token[0], loc, 'Probably missing parameter for ' + token[0] + ' at ' + loc.index));
						}
					}
				});
			}
		}

		buckets.warnings = warnings;
		return buckets;
	};
}

var query = 'INSERT INTO `beer-sample` ( KEY, VALUE ) VALUES ( :docId ,\'Hello World\'); Insert',
	q = new Query(query);

console.log('query:', q.query);
var buckets = q.getBuckets();
for (var bucket of buckets) {
	console.log(JSON.stringify(bucket));
}
for(var warning of buckets.warnings) {
	console.log(warning.desc);
}