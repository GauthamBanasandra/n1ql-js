%{
	/*
	* Input:	JavaScript code embedded with N1QL statements.

	* Output:	Syntactically and semantically valid JavaScript code.

	* Caveats:	The N1QL statement must end with a semi-colon ';'.
				Otherwise, the lex program can't detect detect the end of the query.

	* TODO:		At the moment, the assumption is that all N1QL queries begin with 'select'. Need to handle the case to correctly detect the start of the N1QL query.
	* TODO:		Escape the DSTR strings - necessary for esprima.
	*/
	#include <iostream>
	#include <fstream>
	#include "jsify.hpp"

	std::string parse(const char*);
	void handle_str_start(int state);
	void handle_str_stop(int state);
	bool is_esc();

	using namespace std;

	// Contains the output plain JavaScript code.
	string plain_js;
	int previous_state;
%}
%x N1QL MLCMT SLCMT DSTR SSTR TSTR
%%
	previous_state=YYSTATE;
"/*"	{
			previous_state=YYSTATE;
			BEGIN MLCMT;
			plain_js+="/*";
		}
<MLCMT>"*/"	{
				plain_js+="*/";
				BEGIN previous_state;
			}
<MLCMT>\n	{plain_js+="\n";}
"//"	{
			previous_state=YYSTATE;
			BEGIN SLCMT;
			plain_js+="//";
		}
<SLCMT>\n	{
				BEGIN previous_state;
				plain_js+="\n";
			}
["]	{handle_str_start(DSTR);}
<DSTR>["]	{handle_str_stop(DSTR);}
[']	{handle_str_start(SSTR);}
<SSTR>[']	{handle_str_stop(SSTR);}
[`]	{handle_str_start(TSTR);}
<TSTR>[`]	{handle_str_stop(TSTR);}
(var|function)[ \t\n]+[sS][eE][lL][eE][cC][tT][ \t\n;=(]|[sS][eE][lL][eE][cC][tT][ \t\n]*:[ \t\n]*\{	{return KWD_SELECT;}
(var|function)[ \t\n]+[cC][rR][eE][aA][tT][eE][ \t\n;=(]|[cC][rR][eE][aA][tT][eE][ \t\n]*:[ \t\n]*\{	{return KWD_CREATE;}
(var|function)[ \t\n]+[uU][pP][sS][eE][rR][tT][ \t\n;=(]|[uU][pP][sS][eE][rR][tT][ \t\n]*:[ \t\n]*\{	{return KWD_UPSERT;}
(var|function)[ \t\n]+[iI][nN][sS][eE][rR][tT][ \t\n;=(]|[iI][nN][sS][eE][rR][tT][ \t\n]*:[ \t\n]*\{	{return KWD_INSERT;}
(var|function)[ \t\n]+[dD][eE][lL][eE][tT][eE][ \t\n;=(]|[dD][eE][lL][eE][tT][eE][ \t\n]*:[ \t\n]*\{	{return KWD_DELETE;}
(var|function)[ \t\n]+[uU][pP][dD][aA][tT][eE][ \t\n;=(]|[uU][pP][dD][aA][tT][eE][ \t\n]*:[ \t\n]*\{	{return KWD_UPDATE;}
(var|function)[ \t\n]+[mM][eE][rR][gG][eE][ \t\n;=(]|[mM][eE][rR][gG][eE][ \t\n]*:[ \t\n]*\{	{return KWD_MERGE;}
[sS][eE][lL][eE][cC][tT][ \t\n][ \t\n]?	{
											BEGIN N1QL;
											plain_js+="new N1qlQuery(`select ";
										}

[cC][rR][eE][aA][tT][eE][ \t\n][ \t\n]?	{
											BEGIN N1QL;
											plain_js+="new N1qlQuery(`create ";
										}

[uU][pP][sS][eE][rR][tT][ \t\n][ \t\n]?	{
											BEGIN N1QL;
											plain_js+="new N1qlQuery(`upsert ";
										}
[iI][nN][sS][eE][rR][tT][ \t\n][ \t\n]?	{
											BEGIN N1QL;
											plain_js+="new N1qlQuery(`insert ";
										}
[dD][eE][lL][eE][tT][eE][ \t\n][ \t\n]?	{
											BEGIN N1QL;
											plain_js+="new N1qlQuery(`delete ";
										}
[uU][pP][dD][aA][tT][eE][ \t\n][ \t\n]?	{
											BEGIN N1QL;
											plain_js+="new N1qlQuery(`update ";
										}
[mM][eE][rR][gG][eE][ \t\n][ \t\n]?	{
										BEGIN N1QL;
										plain_js+="new N1qlQuery(`merge ";
									}
<N1QL>";"	{
				BEGIN INITIAL;
				plain_js+="`);";
			}
<N1QL>.	{
			if(yytext[0]=='`' && !is_esc())
				plain_js+="\\";
			plain_js+=string(yytext);
		}
<MLCMT,SLCMT,DSTR,SSTR,TSTR>.	{plain_js+=string(yytext);}
.	{plain_js+=string(yytext);}
\n	{plain_js+="\n";}
%%
// Parses the given input string.
int parse(const char* input, string *output)
{
	// Set the input stream.
	yy_scan_string(input);

	// Begin lexer.
	int code=yylex();

	// Clear the buffer allocation.
	yy_delete_buffer(YY_CURRENT_BUFFER);

	// Copy the output;
	*output=plain_js;

	// Clear the global variable for the next input.
	plain_js="";

	return code;
}

void handle_str_start(int state)
{
	previous_state=YYSTATE;

	switch (state)
	{
	case DSTR:
		BEGIN DSTR;
		plain_js+="\"";
		break;

	case SSTR:
		BEGIN SSTR;
		plain_js+="'";
		break;

	case TSTR:
		BEGIN TSTR;
		plain_js+="`";
		break;
	}
}

void handle_str_stop(int state)
{
	if(!is_esc())
		BEGIN previous_state;

	switch(state)
	{
	case DSTR:
		plain_js+="\"";
		break;

	case SSTR:
		plain_js+="'";
		break;

	case TSTR:
		plain_js+="`";
		break;
	}
}

bool is_esc()
{
	return plain_js.length()>0?plain_js[plain_js.length()-1]=='\\':0;
}
