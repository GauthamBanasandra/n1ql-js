%x N1QL
%x LINQ
%%
"from"	{
			BEGIN LINQ;
			printf("(function(){/* from");
		}
<LINQ>\t+	;
<LINQ>\n+	{printf(" ");}
<LINQ>";"	{
				BEGIN INITIAL;
				printf("; */});");
			}
<LINQ>.	ECHO;
"select"	{
				BEGIN N1QL;
				printf("(function(){/* select");
			}
<N1QL>[\t]+	;
<N1QL>\n+	{printf(" ");}
<N1QL>";"	{
				BEGIN INITIAL;
				printf("; */});");
			}
<N1QL>.	ECHO;
.	ECHO;
%%
extern FILE* yyin;

int main()
{
	yyin=fopen("inputs/input1.txt", "r");
	yylex();
	fclose(yyin);

	return 0;
}