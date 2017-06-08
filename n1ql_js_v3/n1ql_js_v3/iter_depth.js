function get_max_iter_depth(code) {
    var ast = esprima.parse(code, {
            sourceType: 'script'
        }),
        max_depth = 0,
        curr_depth = 0;
    estraverse.traverse(ast, {
        enter: function(node) {
            if (/ForOfStatement/.test(node.type)) {
                ++curr_depth;
                if (curr_depth > max_depth) {
                    max_depth = curr_depth;
                }
            }
        },
        leave: function(node) {
            if (/ForOfStatement/.test(node.type)) {
                --curr_depth;
            }
        }
    });

    return max_depth;
}
