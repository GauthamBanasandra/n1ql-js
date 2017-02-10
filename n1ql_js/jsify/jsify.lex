%{
	/*
	* Input:	JavaScript code embedded with N1QL statements.

	* Output:	Syntactically and semantically valid JavaScript code.

    * Purpose:  Detects N1QL embedded constructs in the JavaScript code and transforms them into comments and wraps it with an anonymous function.

	* Caveats:	The N1QL statement must end with a semi-colon ';'.
				Otherwise, the lex program can't detect detect the end of the query.

	* TODO:		Handle N1QL queries appearing in comments and string.
	* TODO:		At the moment, the assumption is that all N1QL queries begin with 'select'. Need to handle the case to correctly detect the start of the N1QL query.
	*/
	#include <iostream>
	#include <fstream>

	std::string parse(const char*);

	using namespace std;

	// Contains the output plain JavaScript code.
	string plain_js;
%}
%x N1QL
%%
[sS][eE][lL][eE][cC][tT]	{
								BEGIN N1QL;
								plain_js+="(function(){/* select";
							}
<N1QL>[\t]+	;
<N1QL>\n+	{plain_js+=" ";}
<N1QL>";"	{
				BEGIN INITIAL;
				plain_js+="; */});";
			}
<N1QL>.	{plain_js+=string(yytext);}
.	{plain_js+=string(yytext);}
%%
// Parses the given input string.
string parse(const char* input)
{
	// Set the input stream.
	yy_scan_string(input);

	// Begin lexer.
	yylex();

	// Clear the buffer allocation.
	yy_delete_buffer(YY_CURRENT_BUFFER);

	return string(plain_js);
}
