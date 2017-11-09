var fs = require('fs'),
	esprima = require('esprima'),
	estraverse = require('estraverse'),
	escodegen = require('escodegen');

var filename = process.argv[2],
	code = fs.readFileSync(filename, 'utf-8');

function compile(code) {
	try {
		var ast = esprima.parse(code);
		return {
			language: 'JavaScript',
			compileSuccess: true
		};
	} catch (e) {
		return {
			language: 'JavaScript',
			compileSuccess: false,
			index: e.index,
			lineNumber: e.lineNumber,
			columnNumber: e.column,
			description: e.description
		}
	}
}

var compileRes = compile(code);
console.log(JSON.stringify(compileRes));