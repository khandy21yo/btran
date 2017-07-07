/*
 * Parse.l - basic lexical analyzer
 *
 *	Vax/Basic to C++ lexical analyzer
 *
 * Description:
 *
 *	This procedure is used to break Vax/Basic source code
 *	into individual tokens, when are passed on to 'parse.y'.
 *
 */


%%
	/*
	 * White-space characters
	 */
[\r\0]			;
[\n\f]			{ CountLine(); return('\n'); }
"&"[\n\f]		CountLine();
[ \t]+			;

	/*
	 * Qutted Strings
	 */
[ADBOX]?"\""([^\"]*)"\""[CBWLQFDGHPSTX]?	return SetStringReturn(BAS_V_TEXTSTRING);
[ADBOX]?"'"([^']*)"'"[CBWLQFDGHPSTX]?	return SetStringReturn(BAS_V_TEXTSTRING);

"!"[^\n\f\!]*"!"	{ SetComment(BAS_S_REMARK); }
"!"[^\n\f\!]*/"&"[\n\f]	{ SetComment(BAS_S_REMARK); }
"!"[^\n\f\!]*/[\n\f]	{ SetComment(BAS_S_REMARK); }
"REM"			{ BEGIN(xremark); }

<xremark>[^\n\f]*/"&"[\n\f]	{ SetComment(BAS_S_REMARK); BEGIN(INITIAL); }
<xremark>[^\n\f]*/[\n\f]	{ SetComment(BAS_S_REMARK); BEGIN(INITIAL); }

"::"		return SetNullReturn(BAS_X_STRREF);

	/*
	 * Single character operations
	 */
"(" |
")" |
"*" |
"/" |
"\\" |
"+" |
"-" |
":" |
";" |
"," |
"#" |
"^" |
"=" |
"<" |
">"		return SetReturn(yytext[0]);

"=="		return SetReturn(BAS_X_EQ);

"["		return SetReturn('(');
"]"		return SetReturn(')');

"**"		return SetNullReturn('^');
"><" |
"<>"		return SetNullReturn(BAS_X_NEQ);
">=" |
"=>"		return SetNullReturn(BAS_X_GE);
"<=" |
"=<"		return SetNullReturn(BAS_X_LE);

	/*
	 * Numbers, variables, etc.
	 */
(([0-9]+"."[0-9]*)|("."[0-9]+))([ED]"-"?[0-9]+)?	return SetReturn(BAS_V_FLOAT);
[0-9]+[ED]"-"?[0-9]+	return SetReturn(BAS_V_FLOAT);
[0-9]+		return SetReturn(BAS_V_INTEGER);
[0-9]+[%]	return SetReturn(BAS_V_INT);
[%]?[A-Z][A-Z0-9._$%]	{ return SetReturn(BAS_P_NAME); }

%%
