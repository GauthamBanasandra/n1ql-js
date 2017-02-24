/**
 * Created by gautham on 14/02/17.
 */
var fs = require('fs'),
    esprima = require('esprima'),
    estraverse = require('estraverse'),
    escodegen = require('escodegen');

var filename = process.argv[2];
var code = fs.readFileSync(filename, 'utf-8');

// Get the Abstract Syntax Tree (ast) of the input code.
var ast = esprima.parse(code, {attachComment: true});

// This is the stack of the variables in the corresponding scope.
var scopeStack = [];
var stackIdx = -1;

// Traverses the ast and builds the N1QL query wherever it spots them.
function traverse(mode) {
    // Represents the structure for each element of the stack.
    var stackElem = {};

    if (mode === 'gen_code')
        stackIdx = -1;

    /*
     * The ast will be traversed twice and hence, there are two traversal modes - var_scope and gen_code.
     * var_scope -   Traverse the ast once to get to know all the variables and their scopes.
     *               The scopeStack gets populated in this traversal.
     * gen_code -    Traverse the ast for the second time to spot the N1QL embedded queries and transform them to N1QL
     *               calls. The variables in the N1QL queries get substituted in this traversal, by making use of the
     *               stackScope populated in the previous traversal
     */
    estraverse.traverse(ast, {
        // Callback upon entering a node of the ast.
        enter: function (node) {
            /*
             * Function in JavaScript creates a new scope.
             * So, upon entering a function, push an element onto stack if the mode of traversal is 'var_scope'.
             */
            if (/Function/.test(node.type)) {
                if (mode === 'var_scope') {
                    stackElem = {name: node.id ? node.id.name : 'anonymous', vars: {}};
                    scopeStack.push(stackElem);
                    ++stackIdx;
                }
                else if (mode === 'gen_code')
                    ++stackIdx;
            }

            /*
             * Program in JavaScript is the global scope.
             * So, push an initial element onto stack if the mode of traversal is 'var_scope'.
             */
            if (/Program/.test(node.type)) {
                if (mode === 'var_scope') {
                    stackElem = {name: 'global', vars: {}};
                    scopeStack.push(stackElem);
                    ++stackIdx;
                }
                else if (mode === 'gen_code')
                    ++stackIdx;
            }

            /*
             * This 'if' block contains all possible ways in which a variable gets declared in JavaScript.
             * These variable names are pushed onto the stack corresponding to its scope.
             */
            if (mode === 'var_scope') {
                // Variables declared using assignment expression.
                if (node.type === 'AssignmentExpression')
                    scopeStack[stackIdx].vars[node.left.name] = null;

                // Variables declared using 'var' keyword.
                // The stack element will contain 'true' for a variable that is a N1QL variable.
                // TODO:    Check if this works for those declared using 'const' keyword.
                // TODO:    Assumption - N1QL queries always appear in the form of variable declaration.
                if (node.type === 'VariableDeclaration')
                    for (var i in node.declarations)
                        scopeStack[stackIdx].vars[node.declarations[i].id.name] = check_n1ql(node.declarations[i].init) ? true : null;

                // Variables that are passed as parameters to a function.
                if (node.type === 'FunctionDeclaration')
                    if (node.params && node.params.length > 0)
                        for (var i in node.params)
                            scopeStack[stackIdx].vars[node.params[i].name] = null;
            }

            // The following 'if' block is for the 'gen_code' mode.
            if (mode === 'gen_code') {
                // Check the node if it's a N1QL query.
                if (check_n1ql(node)) {
                    // Get the variables in the current scope.
                    var varsInScope = get_vars_in_scope(stackIdx);

                    // Extract the N1QL query from the node.
                    var query = node.body.innerComments[0].value.trim();

                    // Get ast node for the query.
                    var queryAst = get_query_ast(query, varsInScope);

                    // Replace the N1QL statement node with the call to N1QL function node.
                    replace_node(node, queryAst);
                }

                // TODO:    If the user himself has called exec_query(), do nothing.
                // TODO:    Handle 'return' statements occurring in the 'for ... of ...' construct.
                // TODO:    Handle when to replace 'break' with '.stop_iter()'- only if the break statement actually stops the loop.
                // TODO:    Bug - Handle the case when there are nested 'for ... of ...' loops of iterators - Currently crashes.
                // Handling the "for ... of ..." construct here.
                if (/ForOfStatement/.test(node.type) && /Identifier/.test(node.right.type))
                    if (is_n1ql_var(stackIdx, node.right.name)) {
                        /*
                         * If the body of the "for ... of ..." is not a block statement,
                         * then convert it to a block statement.
                         */
                        if (!/BlockStatement/.test(node.body.type))
                            convert_to_block_stmt(node);
                        // For every element in the body of the "for ... of ..."
                        for (var i in node.body.body)
                            if (/BreakStatement/.test(node.body.body[i].type)) {
                                var callAst = esprima.parse(node.right.name + '.stop_iter()').body[0];
                                replace_node(node.body.body[i], callAst);
                            }
                        // If the source is a N1QL variable, then replace it with a call to exec_query().
                        var queryExecAst = get_iter_ast(node, 'for_of');

                        replace_node(node, queryExecAst);
                    }
            }
        },
        leave: function (node) {
            if (/Function/.test(node.type)) {
                // Uncomment to display functions and the variables that are active in them.
                /*if (mode === 'var_scope')
                 {
                 var functionName = node.id ? node.id.name : 'anonymous';
                 var vars = get_vars_in_scope(stackIdx);

                 console.log(functionName, vars.join(', '));
                 }*/

                --stackIdx;
            }
        }
    });
}

function convert_to_block_stmt(node) {
    var forBodyAst = {};

    // Make a deep copy of the body.
    Object.keys(node.body).forEach(function (key) {
        forBodyAst[key] = node.body[key];
    });

    // Transform the previous single-line statement into a block.
    node.body.body = [forBodyAst];
    node.body.type = 'BlockStatement';
}

// Replaces source node with the target node.
function replace_node(source, target) {
    Object.keys(source).forEach(function (key) {
        delete source[key];
    });

    Object.keys(target).forEach(function (key) {
        source[key] = target[key];
    });
}

// Checks if the given node is a N1QL query.
function check_n1ql(node) {
    return (
        node &&
        node.type === 'FunctionExpression' &&
        node.id === null &&
        typeof node.params === typeof [] &&
        node.params.length === 0 &&
        node.body.type === 'BlockStatement' &&
        typeof node.body.body === typeof [] &&
        node.body.body.length === 0 &&
        node.body.innerComments.length === 1 &&
        node.body.innerComments[0].type === 'Block'
    );
}

// Build an ast node for N1QL function call from the query.
function get_query_ast(query, vars) {
    // Identifier regex.
    var re = /:([a-zA-Z_$][a-zA-Z_$0-9]*)/g;

    // Match the regex against the query to find all the variables that are used.
    var matches = query.match(re);

    for (var i in matches) {
        // Get the variable from the query that has to be substituted.
        var _var = matches[i].slice(1);

        // Check if that variable exists in the list of the variables that are in the scope.
        if (vars.indexOf(_var) == -1)
            throw "'" + _var + "' not found in scope'";
    }

    // Replace the :<var> with proper substitution.
    query = query.replace(re, '" + $1 + "');
    query = 'new N1qlQuery("' + query + '");';

    // Return the ast.
    return esprima.parse(query).body[0].expression;
}

function get_iter_ast(node, mode) {
    if (mode === 'for_of') {
        var body = escodegen.generate(node.body, {comment: true});
        return esprima.parse(
            node.right.name +
            '.iter(function (' + (node.left.name ? node.left.name : node.left.declarations[0].id.name) + ')' +
            body + ')'
        ).body[0];
    }

    throw 'Invalid arg ' + mode + ' for get_iter_ast';
}

// Get the variables at the given stack index.
function get_vars_in_scope(idx) {
    var vars = [];

    for (var i = 0; i <= idx; ++i)
        Object.keys(scopeStack[i].vars).forEach(function (key) {
            vars[key] = null;
        });

    return Object.keys(vars);
}

// Checks if the given variable is the result of a N1QL query.
function is_n1ql_var(idx, name) {
    while (idx) {
        for (var i in scopeStack[idx].vars)
            if (scopeStack[idx].vars[name])
                return true;
        --idx;
    }

    return false;
}
// First traversal - detect all the variables in their corresponding scopes.
traverse('var_scope');

// Second traversal - replace all N1QL queries with N1QL function calls.
traverse('gen_code');

console.log(escodegen.generate(ast, {comment: true}));
