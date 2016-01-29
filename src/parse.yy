/* parse.yy
 *
 *	The parser for the Vax/Basic to C++ translator.
 *
 * Description:
 *
 *	This routine (along with lex.l) is used to translate a
 *	Vax/Basic program's source code into an initial parse
 *	tree. This tree is a rough translation that is further
 *	transformed in program.cc, and finally written out in
 *	nodes.cc
 *
 * History:
 *
 *	08/30/1996 - Kevin Handy
 *		Set version number to 1.1, and install on internet.
 *
 *	09/04/1996 - Kevin Handy
 *		Added "BLOCK" for get clause.
 *
 *	09/26/1996 - Kevin Handy
 *		Lose two 'bstruct' and 'tempch' variables that were
 *		defined but never used, and two debug macros that
 *		are no longer used.
 *
 *	10/09/1996 - Kevin Handy
 *		Added BAS_V_PREDEF constants.
 *
 *	10/22/1996 - Kevin Handy
 *		Handle new variables 'IntegerType' and 'RealType',
 *		as well as new variable types 'SINGLE', 'GFLOAT',
 *		and 'HFLOAT'.
 *
 *	11/12/1996 - Kevin Handy
 *		Have "close" generate a 'downlink' of closes instead
 *		of a close linked to a list, so can lose function from
 *		nodes.cc.
 *		Same for "read".
 *
 *	11/12/1996 - Kevin Handy
 *		"forassignment" shouldn't allow a list for the lvalue.
 *
 *	02/06/1997 - Kevin Handy
 *		Redo DECLARE definition.
 *		Rework definition codes.
 *
 *	02/09/1997 - Kevin Handy
 *		Work on handling of 'MAP' and 'COMMON' variables.
 *
 *	02/12/1997 - Kevin Handy
 *		Fix DECLARE so it can handle more code.
 *
 *	03/31/1997 - Kevin Handy
 *		Handle ':' like '/' as a statement seperator.
 *
 *	04/01/1997 - Kevin Handy
 *		Fix problem with "THEN 100" type of statements.
 *		Needed to set type to LABEL.
 *
 *	04/02/1997 - Kevin Handy
 *		Add CLUSTERSIZE option to open.
 *		Add UNLESS xxx THEN statement.
 *		Handle "CHAIN A$ B%".
 *
 *	04/06/1997 - Kevin Handy
 *		Added code for BAS_X_LNOT.
 *
 *	04/21/1997 - Kevin Handy
 *		Added several additional BASIC Constants.
 *
 *	04/26/1997 - Kevin Handy
 *		Set 'NeedPuse' flag as needed.
 *
 *	06/09/1997 - Kevin Handy
 *		Strip leading zero's from line numbers.
 *
 *	06/09/1997 - Kevin Handy
 *		Added 'OPTION TYPE = EXPLICIT' (do nothing)
 *		Added keyword '%LIBRARY'
 *
 *	06/16/1997 - Kevin Handy
 *		Set version to 1.4
 *
 *	07/09/1997 - Kevin Handy
 *		Added 'BAS_S_MAT', and MAT READ.
 *
 *	11/17/1997 - Kevin Handy
 *		Added RMS command SCRATCH.
 *		Added "CAUSE ERROR" command.
 *
 *	11/17/1997 - Kevin Handy
 *		Added keyword NOECHO
 *
 *	11/17/1997 - Kevin Handy
 *		Added MOD function.
 *
 *	01/28/1998 - Kevin Handy
 *		Created BAS_S_DEFSTAR, but it is exactly
 *		like BAS_S_DEF for the mement
 *
 *	02/02/1998 - Kevin Handy
 *		Create 0 as NULL casted as (Node*) to lose a lot
 *		of C++ warning messages
 *
 *	02/07/1998 - Kevin Handy
 *		Revorked expressions trying to eliminate many of the
 *		shift/reduce and reduce/reduce errors.
 *
 *	12/03/1998 - Kevin Handy
 *		Support 'IF xxx GOTO' and 'IF xxx GOSUB' variants.
 *
 *	12/06/1998 - Kevin Handy
 *		Fix 'CLOSE' statement generated code
 *
 *	12/23/1998 - Kevin Handy
 *		Added MAP <assignment> code
 *
 *	12/26/1998 - Kevin Handy
 *		Lose special chaacteristics of FILL variables.
 *
 *	03/06/1999 - Kevin Handy
 *		Added lexicals for "WHEN ERROR IN ... USE ... END WHEN",
 *		EXIT HANDLER, CONTINUE
 *
 *	08/19/1999 - Kevin Handy
 *		Several modifications to all definitions to make them more
 *		consistant.
 *
 *	03/17/2000 - Kevin Handy
 *		Handle print statements like 'PRINT "TEST"A"ING"'.
 *
 *	04/02/2000 - Kevin Handy
 *		Handle input statements line 'INPUT "TEST"A'.
 *
 *	04/10/2000 - Kevin Handy
 *		Handle "RECORD  SIZE" as "RECORDSIZE"
 *
 *	06/28/2000 - Kevin Handy
 *		Fix handling of OTHERWISE in ON statements.
 *
 *	07/03/2000 - Kevin Handy
 *		Modification to allow arrays in map statements.
 *
 *	08/03/2000 - Kevin Handy
 *		Added MAT definitions.
 *
 *	09/27/2000 - Kevin Handy
 *		Added empty "CASE" to allow for "VARIANT" records
 *		return $1 in vardeflist instead of 0.
 *
 *	10/10/2000 - Kevin Handy
 *		Allow BAS_V_NAME in a dimension to allow for constants.
 *		Fix 'dimcomma' to handle comma's properly (no more crash)
 *
 *	10/27/2000 - Kevin Handy
 *		Allow for "ONERROR" in place of "ON ERROR"
 *
 *	11/07/2000 - Kevin Handy
 *		Allow '%PRINT'
 *
 *	12/07/2000 - Kevin Handy
 *		Add STEP to FOR..STEP..UNTIL loop
 */

%{
#include <cstdio>
#include "vartype.h"
#include "basic.h"
#include "variable.h"
#include "varlist.h"
#include "nodes.h"

#ifndef YYSTYPE
#define YYSTYPE Node *
#endif

#ifndef YYDEBUG
#define YYDEBUG 0
#endif

#define YYINITDEPTH 6000	/* Only defined because of broken BISON */
#define YYMAXDEPTH 23000
%}

%start program

%token BAS_S_ACCESS
%token BAS_S_ACTIVE
%token BAS_S_ALLOW
%token BAS_S_ALTERNATE
%token BAS_S_ANY
%token BAS_S_APPEND
%token BAS_S_AS
%token BAS_S_BACK
%token BAS_S_BLOCK
%token BAS_S_BLOCKSIZE
%token BAS_S_BUCKETSIZE
%token BAS_S_BUFFER
%token BAS_S_BY
%token BAS_S_BYTE
%token BAS_S_CALL
%token BAS_S_CASE
%token BAS_S_CAUSE
%token BAS_S_CHAIN
%token BAS_S_CHANGE
%token BAS_S_CHANGE1
%token BAS_S_CHANGE2
%token BAS_S_CHANGES
%token BAS_S_CLOSE
%token BAS_S_CLUSTERSIZE
%token BAS_S_COMMON
%token BAS_S_CON
%token BAS_S_CONSTANT
%token BAS_S_CONTIGUOUS
%token BAS_S_CONTINUE
%token BAS_S_CONNECT
%token BAS_S_COUNT
%token BAS_S_DATA
%token BAS_S_DECIMAL
%token BAS_S_DECLARE
%token BAS_S_DEF
%token BAS_S_DEFSTAR
%token BAS_S_DEFAULTNAME
%token BAS_S_DELETE
%token BAS_S_DESC
%token BAS_S_DIM
%token BAS_S_DOUBLE
%token BAS_S_DUPLICATES
%token BAS_S_ELSE
%token BAS_S_END
%token BAS_S_EQ
%token BAS_S_ERL
%token BAS_S_ERROR
%token BAS_S_EXIT
%token BAS_S_EXPLICIT
%token BAS_S_EXTEND
%token BAS_S_EXTENDSIZE
%token BAS_S_EXTERNAL
%token BAS_S_FIELD
%token BAS_S_FILE
%token BAS_S_FILESIZE
%token BAS_S_FIND
%token BAS_S_FIXED
%token BAS_S_FNEND
%token BAS_S_FNEXIT
%token BAS_S_FOR
%token BAS_S_FORTRAN
%token BAS_S_FREE
%token BAS_S_FROM
%token BAS_S_FUNCTION
%token BAS_S_MAINFUNCTION
%token BAS_S_FUNCTIONEND
%token BAS_S_FUNCTIONEXIT
%token BAS_S_GE
%token BAS_S_GET
%token BAS_S_GFLOAT
%token BAS_S_GO
%token BAS_S_GOSUB
%token BAS_S_GOTO
%token BAS_S_GT
%token BAS_S_HANDLER
%token BAS_S_HFLOAT
%token BAS_S_IDN
%token BAS_S_IF
%token BAS_S_IN
%token BAS_S_INACTIVE
%token BAS_S_INV
%token BAS_S_INDEXED
%token BAS_S_INPUT
%token BAS_S_INTEGER
%token BAS_S_ITERATE
%token BAS_S_KEY
%token BAS_S_KILL
%token BAS_S_LET
%token BAS_S_LINE
%token BAS_S_LINPUT
%token BAS_S_LIST
%token BAS_S_LONG
%token BAS_S_LSET
%token BAS_S_MAP
%token BAS_S_MARGIN
%token BAS_S_MAT
%token BAS_X_MATREAD
%token BAS_S_MOD
%token BAS_S_MODE
%token BAS_S_MODIFY
%token BAS_S_MOVE
%token BAS_S_NAME
%token BAS_S_NEXT
%token BAS_S_NOCHANGES
%token BAS_S_NODUPLICATES
%token BAS_S_NOECHO
%token BAS_S_NONE
%token BAS_S_NOREWIND
%token BAS_S_NOSPAN
%token BAS_S_ON
%token BAS_S_OPEN
%token BAS_S_OPTION
%token BAS_S_ORGANIZATION
%token BAS_S_OTHERWISE
%token BAS_S_OUTPUT
%token BAS_S_OVERFLOW
%token BAS_S_PRIMARY
%token BAS_S_PRINT
%token BAS_S_PUT
%token BAS_S_RANDOM
%token BAS_S_READ
%token BAS_S_REAL
%token BAS_S_RECORD
%token BAS_S_RECORDTYPE
%token BAS_S_RECORDSIZE
%token BAS_S_REF
%token BAS_S_REGARDLESS
%token BAS_S_RELATIVE
%token BAS_S_REMAP
%token BAS_S_REMARK
%token BAS_S_RESTORE
%token BAS_S_RESET
%token BAS_S_RESUME
%token BAS_S_RETRY
%token BAS_S_RETURN
%token BAS_S_REWIND
%token BAS_S_RFA
%token BAS_S_ROUNDING
%token BAS_S_RSET
%token BAS_S_SCRATCH
%token BAS_S_SELECT
%token BAS_S_SEQUENTIAL
%token BAS_S_SINGLE
%token BAS_S_SIZE
%token BAS_S_SLEEP
%token BAS_S_SPAN
%token BAS_S_STEP
%token BAS_S_STOP
%token BAS_S_STREAM
%token BAS_S_STRING
%token BAS_S_SUB
%token BAS_S_SUBEND
%token BAS_S_SUBEXIT
%token BAS_S_SUBSCRIPT
%token BAS_S_TEMPORARY
%token BAS_S_THEN
%token BAS_S_TO
%token BAS_S_TRN
%token BAS_S_TYPE
%token BAS_S_UNDEFINED
%token BAS_S_UNLESS
%token BAS_S_UNLOCK
%token BAS_S_UNTIL
%token BAS_S_UPDATE
%token BAS_S_USE
%token BAS_S_USEROPEN
%token BAS_S_USING
%token BAS_S_VALUE
%token BAS_S_VARIABLE
%token BAS_S_VARIANT
%token BAS_S_VIRTUAL
%token BAS_S_WAIT
%token BAS_S_WHEN
%token BAS_S_WHILE
%token BAS_S_WINDOWSIZE
%token BAS_S_WORD
%token BAS_S_WRITE
%token BAS_S_ZER

%token BAS_P_INCLUDE
%token BAS_P_FROM
%token BAS_P_CDD
%token BAS_P_TITLE
%token BAS_P_SBTTL
%token BAS_P_IDENT
%token BAS_P_LIBRARY
%token BAS_P_ABORT
%token BAS_P_PRINT

%token BAS_V_FUNCTION
%token BAS_V_FLOAT
%token BAS_V_INTEGER
%token BAS_V_INT
%token BAS_V_LABEL
%token BAS_V_USELABEL
%token BAS_V_NAME
%token BAS_V_TEXTSTRING
%token BAS_V_RMSRECORD
%token BAS_V_PREDEF
%token BAS_V_MATREAD
%token BAS_V_DEFINEVAR
%token BAS_V_DEFINEFUN
%token BAS_V_DECLARECONSTANT
%token BAS_V_DECLAREFUN
%token BAS_V_ENDHANDLER

%token BAS_X_UNARYLT
%token BAS_X_UNARYLE
%token BAS_X_UNARYGT
%token BAS_X_UNARYGE
%token BAS_X_UNARYNEQ
%token BAS_X_LNOT
%token BAS_X_LAND
%token BAS_X_LOR

%left BAS_X_NEQ BAS_X_GE BAS_X_LE BAS_S_EQV BAS_X_EQ
%left BAS_S_AND BAS_S_OR BAS_S_IMP BAS_S_XOR
%left BAS_S_NOT
%left '=' '>' '<'
%left '+' '-'
%left '*' '/'
%right '^'
%left BAS_X_STRREF

%left ','

%token BAS_N_UMINUS
%token BAS_N_UPLUS
%token BAS_N_ASSIGN
%token BAS_N_FORASSIGN
%token BAS_N_ASSIGNLIST
%token BAS_N_LIST
%token BAS_N_ENDIF
%token BAS_N_ONGOTO
%token BAS_N_ONGOSUB
%token BAS_N_ONERROR
%token BAS_N_ENDSELECT
%token BAS_N_ENDRECORD
%token BAS_N_ENDWHEN
%token BAS_N_EXTERNALFUNCTION
%token BAS_N_EXTERNALSUB
%token BAS_N_EXTERNALCONSTANT
%token BAS_N_FORUNTIL
%token BAS_N_FORWHILE
%token BAS_N_CASEELSE
%token BAS_N_NULL
%token BAS_N_VARTYPE
%token BAS_N_ENDVARIANT
%token BAS_N_RECORD
%token BAS_N_STRUCTNAME
%token BAS_N_TYPEFUN
%token BAS_N_WHENERRORIN
%token BAS_N_EXITHANDLER
%token BAS_N_CAUSEERROR

%%
 /* Grammer rules */
program:	doprogram { DoProgram($1); }
;

doprogram:	/* Empty */ { $$ = 0; }
		| nline doprogram { $$ = $1->DownLink($2); }
;

nline:		linenum label statement '\n' { $$ =
			CommentList->DownLink($1->DownLink($2->DownLink($3)));
			CommentList = 0; }
;

linenum:	/* Empty */ {$$ = 0;}
		| BAS_V_INTEGER { $1->SetType(BAS_V_LABEL);
			$1->StripLeadZero(); $$ = $1; }
;

label:		/* Empty */ {$$ = 0;}
		| BAS_V_LABEL { $$ = $1; }
;

statement:	statementx
		| statement '\\' statementx { delete $2;
			$$ = $1->DownLink($3); }
		| statement ':' statementx { delete $2;
			$$ = $1->DownLink($3); }
		| statement BAS_S_THEN statementw {
			$$ = $1->DownLink($2->DownLink($3)); }
		| statement BAS_S_ELSE statementw {
			$$ = $1->DownLink($2->DownLink($3)); }
		| statement BAS_N_ENDIF statementx {
			$$ = $1->DownLink($2->DownLink($3)); }
;

statementw:	statementx
		| BAS_V_INTEGER { $1->SetType(BAS_V_USELABEL);
			$$ = (new Node(BAS_S_GOTO,
			"", 0))->Link($1); }
;

statementx:	/* Empty */ {$$ = 0;}
		| stmtnomod
		| stmtmodlist
;

stmtmodlist:	stmtmod
		| stmtmodlist ifmod { $$ = $2->DownLink($1, 1); }
		| stmtmodlist unlessmod { $$ = $2->DownLink($1, 1); }
		| stmtmodlist formod { $$ = $2->DownLink($1, 1); }
		| stmtmodlist whilemod { $$ = $2->DownLink($1, 1); }
		| stmtmodlist untilmod { $$ = $2->DownLink($1, 1); }
;

ifmod:		BAS_S_IF expression  { $$ = $1->Link($2); }
;

unlessmod:	BAS_S_UNLESS expression  { $$ = $1->Link($2); }
;

whilemod:	BAS_S_WHILE expression  { $$ = $1->Link($2); }
;

untilmod:	BAS_S_UNTIL expression  { $$ = $1->Link($2); }
;

formod:		BAS_S_FOR forassignment BAS_S_TO expression { delete $3;
			$$ = $1->Link($2, $4); }
		| BAS_S_FOR forassignment BAS_S_TO expression BAS_S_STEP expression {delete $3;
			delete $5;
			$$ = $1->Link($2, $4, $6); }
		| BAS_S_FOR forassignment BAS_S_UNTIL expression { delete $3;
			$1->SetType(BAS_N_FORUNTIL);
			$$ = $1->Link($2, $4); }
		| BAS_S_FOR forassignment BAS_S_STEP expression BAS_S_UNTIL expression { delete $3;
			delete $5; $1->SetType(BAS_N_FORUNTIL);
			$$ = $1->Link($2, $6, $4); }
		| BAS_S_FOR forassignment BAS_S_WHILE expression { delete $3;
			$1->SetType(BAS_N_FORWHILE);
			$$ = $1->Link($2, $4); }
;

stmtmod:	assignment
		| BAS_S_CALL variable { $2->SetType(BAS_V_FUNCTION);
			$$ = $1->Link($2); }
		| BAS_S_CAUSE BAS_S_ERROR expression { $$ = $1->Link($3);
			$1->SetType(BAS_N_CAUSEERROR); delete $2;  }
		| BAS_S_CHAIN expression {$$ = $1->Link($2); }
		| BAS_S_CHAIN expression BAS_S_LINE expression {
			$$ = $1->Link($2, $4); delete $3; }
		| BAS_S_CHAIN expression expression {
			$$ = $1->Link($2, $3); }
		| BAS_S_CHANGE expression BAS_S_TO expression {
			$$ = $1->Link($2, $4); delete $3; }
		| BAS_S_CLOSE chnllist {$$ = $1->Link($2); NeedChannel = 1; }
		| BAS_S_DELETE chnlexp { $$ = $1->Link($2);
			NeedChannel = 1; }
		| BAS_S_SCRATCH chnlexp { $$ = $1->Link($2);
			NeedChannel = 1; }
		| BAS_S_EXIT BAS_S_FUNCTION { $1->SetType(BAS_S_FUNCTIONEXIT);
			$$ = $1; delete $2; }
		| BAS_S_EXIT BAS_S_HANDLER { $1->SetType(BAS_N_EXITHANDLER);
			$$ = $1; delete $2; }
		| BAS_S_EXIT BAS_S_SUB { $1->SetType(BAS_S_SUBEXIT);
			$$ = $1; delete $2; }
		| BAS_S_EXIT lineno {$1->SetType(BAS_S_GOTO); $$ = $1->Link($2); }
		| BAS_S_FIELD chnlexp ',' fieldlist {
			$$ = $1->Link($2, $4); delete $3;
			NeedChannel = 1; }
		| BAS_S_FIND chnlexp getclause { $$ = $1->Link($2, $3);
			NeedChannel = 1; }
		| BAS_S_FOR forassignment BAS_S_TO expression { delete $3;
			$$ = $1->Link($2, $4); }
		| BAS_S_FOR forassignment BAS_S_TO expression BAS_S_STEP expression { delete $3;
			delete $5; $$ = $1->Link($2, $4, $6); }
		| BAS_S_FOR forassignment BAS_S_UNTIL expression { delete $3;
			$1->SetType(BAS_N_FORUNTIL);
			$$ = $1->Link($2, $4); }
		| BAS_S_FOR forassignment BAS_S_STEP expression BAS_S_UNTIL expression { delete $3;
			delete $5; $1->SetType(BAS_N_FORUNTIL);
			$$ = $1->Link($2, $6, $4); }
		| BAS_S_FOR forassignment BAS_S_WHILE expression { delete $3;
			$1->SetType(BAS_N_FORWHILE);
			$$ = $1->Link($2, $4); }
		| BAS_S_FNEXIT
		| BAS_S_FREE chnlexp { $$ = $1->Link($2);
			NeedChannel = 1; }
		| BAS_S_FUNCTIONEXIT { $1->SetType(BAS_S_FNEXIT); $$ = $1; }
		| BAS_S_GET chnlexp getclause { $$ = $1->Link($2, $3);
			NeedChannel = 1; }
		| BAS_S_GOSUB lineno {$$ = $1->Link($2); }
		| BAS_S_GOTO lineno {$$ = $1->Link($2); }
		| BAS_S_CONTINUE {$$ = $1; }
		| BAS_S_CONTINUE lineno {$$ = $1->Link($2); }
		| BAS_S_GO BAS_S_SUB lineno { $1->SetType(BAS_S_GOSUB);
			$$ = $1->Link($3); delete $2; }
		| BAS_S_GO BAS_S_TO lineno { $1->SetType(BAS_S_GOTO);
			$$ = $1->Link($3); delete $2; }
		| BAS_S_INPUT optinput {
			$$ = $1->Link($2); NeedIostreamH = 1; }
		| BAS_S_INPUT BAS_S_LINE optinput { $1->SetType(BAS_S_LINPUT);
			$$ = $1->Link($3);
			delete $2; NeedIostreamH = 1; }
		| BAS_S_ITERATE
		| BAS_S_ITERATE lineno { $1->SetType(BAS_S_GOTO);
			$$ = $1->Link($2); }
		| BAS_S_KILL expression {$$ = $1->Link($2); }
		| BAS_S_LET assignment {$$ = $2; delete $1; }
		| BAS_S_LINPUT optinput {
			$$ = $1->Link($2); NeedIostreamH = 1; }
		| BAS_S_LSET assignment {$$ = $1->Link($2); }
		| BAS_S_MARGIN chnlexp ',' expression {
			$$ = $1->Link($2, $4); delete $3;
			NeedChannel = 1; }
		| BAS_S_MOVE BAS_S_TO chnlexp ',' expression {
			$$ = $1->Link($3, $5); delete $2;
			delete $4; NeedChannel = 1; }
		| BAS_S_MOVE BAS_S_FROM chnlexp ',' expression {
			$$ = $1->Link($3, $5); delete $2;
			delete $4; NeedChannel = 1; }
		| BAS_S_NAME expression BAS_S_AS expression { delete $3;
			$$ = $1->Link($2, $4); }
		| BAS_S_NEXT
		| BAS_S_NEXT variable { $$ = $1->Link($2); }
		| BAS_S_ON BAS_S_ERROR BAS_S_GO BAS_S_TO lineno { $1->SetType(BAS_N_ONERROR);
			$$ = $1->Link($5); delete $2;
			delete $3; delete $4; }
		| BAS_S_ON BAS_S_ERROR BAS_S_GO BAS_S_TO { $1->SetType(BAS_N_ONERROR);
			$$ = $1; delete $2;
			delete $3; delete $4; }
		| BAS_N_ONERROR BAS_S_GO BAS_S_TO lineno { $$ = $1->Link($4);
			delete $2; delete $3; }
		| BAS_N_ONERROR BAS_S_GO BAS_S_TO { $$ = $1;
			delete $2; delete $3; }
		| BAS_S_WHEN BAS_S_ERROR BAS_S_IN { delete $2; delete $3;
			$1->SetType(BAS_N_WHENERRORIN); $$ = $1; }
		| BAS_S_ON BAS_S_ERROR BAS_S_GOTO lineno { $1->SetType(BAS_N_ONERROR);
			$$ = $1->Link($4); delete $2; delete $3; }
		| BAS_S_ON BAS_S_ERROR BAS_S_GOTO { $1->SetType(BAS_N_ONERROR);
			$$ = $1; delete $2; delete $3; }
		| BAS_N_ONERROR BAS_S_GOTO lineno { $$ = $1->Link($3); delete $2; }
		| BAS_N_ONERROR BAS_S_GOTO { $$ = $1; delete $2; }
		| BAS_S_ON BAS_S_ERROR BAS_S_GO BAS_S_BACK { $1->SetType(BAS_N_ONERROR);
			$$ = $1->Link($4); delete $2; delete $3; }
		| BAS_N_ONERROR BAS_S_GO BAS_S_BACK { $$ = $1->Link($3); delete $2; }
		| BAS_S_ON expression BAS_S_GOTO linelist opother {
			$1->SetType(BAS_N_ONGOTO);
			$$ = $1->Link($2, $4, $5); delete $3; }
		| BAS_S_ON expression BAS_S_GO BAS_S_TO linelist opother {
			$1->SetType(BAS_N_ONGOTO);
			$$ = $1->Link($2, $5, $6); delete $3; delete $4; }
		| BAS_S_ON expression BAS_S_GOSUB linelist opother {
			$1->SetType(BAS_N_ONGOSUB);
			$$ = $1->Link($2, $4, $5); delete $3; }
		| BAS_S_ON expression BAS_S_GO BAS_S_SUB linelist opother {
			$1->SetType(BAS_N_ONGOSUB);
			$$ = $1->Link($2, $5, $6); delete $3; delete $4; }
		| BAS_S_OPEN expression optfor BAS_S_AS BAS_S_FILE chexpression openlist {
			$$ = $1->Link($2, $3, $6, $7);
			delete $4; delete $5; NeedChannel = 1; }
		| BAS_S_PRINT optprint { $$ = $1->Link($2); NeedIostreamH = 1; }
		| BAS_S_PUT chnlexp getclause { $$ = $1->Link($2, $3);
			NeedChannel = 1; }
		| BAS_S_RANDOM
		| BAS_S_READ readlist { $$ = $2; delete $1; }
		| BAS_S_MAT BAS_S_READ matreadlist { $$ = $3; delete $1;
			$1->SetType(BAS_X_MATREAD); delete $2; }
		| BAS_S_MAT BAS_V_NAME '=' matexpression { $$ = $1->Link($2, $4);
			delete $3; }
		| BAS_S_RESET { $$ = $1; NeedDataList = 1; }
		| BAS_S_RESET chnlexp getclause { $$ = $1->Link($2, $3);
			NeedChannel = 1; }
		| BAS_S_RESTORE { $$ = $1; NeedDataList = 1; }
		| BAS_S_RESTORE chnlexp getclause { $$ = $1->Link($2, $3);
			NeedChannel = 1; }
		| BAS_S_RESUME
		| BAS_S_RETRY
		| BAS_S_RESUME lineno { $$ = $1->Link($2); }
		| BAS_S_RETURN
		| BAS_S_RSET assignment { $$ = $1->Link($2); }
		| BAS_S_SLEEP expression { $$ = $1->Link($2); NeedUnistd = 1; }
		| BAS_S_STOP
		| BAS_S_SUBEXIT
		| BAS_S_UNLOCK chnlexp { $$ = $1->Link($2);
			NeedChannel = 1; }
		| BAS_S_UNTIL expression { $$ = $1->Link($2); }
		| BAS_S_UPDATE chnlexp { $$ = $1->Link($2);
			NeedChannel = 1; }
		| BAS_S_WAIT expression { $$ = $1->Link($2); }
		| BAS_S_WHILE expression { $$ = $1->Link($2); }
;

stmtnomod:	BAS_S_CASE BAS_S_ELSE { $1->SetType(BAS_N_CASEELSE); delete $2; }
		| BAS_S_CASE caseexprlist { $$ = $1->Link($2); }
		| BAS_S_CASE { $$ = $1; }
		| BAS_S_USE
		| BAS_S_COMMON '(' BAS_V_NAME ')' formalplist { delete $2;
			delete $4; $5->SmoothTypes(); $$ = $1->Link($3, $5); }
		| BAS_S_DATA datalist { NeedDataList = 1; $$ = $1->Link($2); }
		| BAS_S_DECLARE formalplist { $2->SmoothTypes();
			$$ = $1->Link($2); }
		| BAS_S_DECLARE vartype BAS_S_CONSTANT constlist {
			$4->SmoothTypes($2); delete $2; delete $3;
			$1->SetType(BAS_V_DECLARECONSTANT);
			$$ = $1->Link($4); }
		| BAS_S_DECLARE vartype BAS_S_FUNCTION fundeflist {
			$4->SmoothTypes($2); delete $2; delete $3;
			$1->SetType(BAS_V_DECLAREFUN);
			$$ = $1->Link($4); }
		| BAS_S_DEF datatype formalname formalparam funequ {
			$$ = $1->Link($2, $3, 0, $4);
			$1->DownLink($5, 1); }
		| BAS_S_DEF '*' datatype formalname formalparam funequ {
			$1->SetType(BAS_S_DEFSTAR); delete $2;
			$$ = $1->Link($3, $4, 0, $5);
			$1->DownLink($6, 1); }
		| BAS_S_DIM opchan dimplist { $3->SmoothTypes();
			$$ = $1->Link($3, $2);
			if ($2 != 0) { NeedVirtual = 1; }}
		| BAS_S_END BAS_S_FUNCTION { $1->SetType(BAS_S_FUNCTIONEND);
			$$ = $1; delete $2; }
		| BAS_S_END BAS_S_SELECT { $1->SetType(BAS_N_ENDSELECT);
			delete $2; $$ = $1; }
		| BAS_S_END BAS_S_WHEN { $1->SetType(BAS_N_ENDWHEN);
			delete $2; $$ = $1; }
		| BAS_S_END BAS_S_SUB { $1->SetType(BAS_S_SUBEND);
			delete $2; }
		| BAS_S_END BAS_S_DEF { $1->SetType(BAS_S_FNEND);
			delete $2; }
		| BAS_S_END BAS_S_RECORD {$1->SetType(BAS_N_ENDRECORD);
			delete $2; $$ = $1; }
		| BAS_S_END BAS_S_RECORD BAS_V_NAME {$1->SetType(BAS_N_ENDRECORD);
			delete $2; $$ = $1; delete $3; }
		| BAS_S_END BAS_S_HANDLER { $1->SetType(BAS_V_ENDHANDLER);
			delete $2; $$ = $1; }
		| BAS_S_END
		| BAS_S_EXTEND { delete $1; $$ = 0; }
		| BAS_S_EXTERNAL datatype BAS_S_FUNCTION formalname passmech formalparam {
			$1->SetType(BAS_N_EXTERNALFUNCTION);
			delete $3;
			$$ = $1->Link($2, $4, $5, $6); }
		| BAS_S_EXTERNAL vartype BAS_S_CONSTANT constlist {
			$4->SmoothTypes($2); delete $2; delete $3;
			$1->SetType(BAS_N_EXTERNALCONSTANT);
			$$ = $1->Link($4); }
		| BAS_S_EXTERNAL BAS_S_SUB formalname passmech formalparam {
			$1->SetType(BAS_N_EXTERNALSUB);
			delete $2; $$ = $1->Link(0, $3, $4, $5); }
		| BAS_S_EXTERNAL formalplist { $2->SmoothTypes();
			$$ = $1->Link($2); }
		| BAS_S_FNEND
		| BAS_S_FUNCTION datatype formalname passmech formalparam {
			$$ = $1->Link($2, $3, $4, $5); }
		| BAS_S_FUNCTIONEND
		| BAS_S_IF expression { $$ = $1->Link($2); }
		| BAS_S_IF expression BAS_S_GOTO lineno { $$ = $1->Link($2);
			$1->DownLink($3->Link($4)); }
		| BAS_S_IF expression BAS_S_GOSUB lineno { $$ = $1->Link($2);
			$1->DownLink($3->Link($4)); }
		| BAS_S_IF expression BAS_S_GO BAS_S_TO lineno {
			$$ = $1->Link($2);
			$3->SetType(BAS_S_GOTO);
			$1->DownLink($3->Link($5)); delete $4; }
		| BAS_S_IF expression BAS_S_GO BAS_S_SUB lineno {
			$$ = $1->Link($2);
			$3->SetType(BAS_S_GOSUB);
			$1->DownLink($3->Link($5)); delete $4; }
		| BAS_S_UNLESS expression { $$ = $1->Link($2); }
		| BAS_S_MAP '(' BAS_V_NAME ')' formalplist { delete $2;
			delete $4; $5->SmoothTypes(); $$ = $1->Link($3, $5); }
		| BAS_S_OPTION BAS_S_SIZE '=' optiontype { $$ = 0; delete $1;
			delete $2; delete $3; }
		| BAS_S_OPTION BAS_S_TYPE '=' BAS_S_EXPLICIT { $$ = 0;
			delete $1; delete $2; delete $3; delete $4; }
		| BAS_S_SELECT expression { $$ = $1->Link($2); }
		| BAS_S_SUB formalname passmech formalparam {
			$$ = $1->Link(0, $2, $3, $4); }
		| BAS_S_SUBEND
		| BAS_S_HANDLER formalname { $$ = $1->Link(0, $2); }
		| BAS_P_INCLUDE BAS_V_TEXTSTRING { $$ = $1->Link($2); }
		| BAS_P_INCLUDE BAS_V_TEXTSTRING BAS_P_FROM BAS_P_LIBRARY BAS_V_TEXTSTRING {
			$$ = $1->Link($2, $5); delete $3; delete $4; }
		| BAS_P_INCLUDE BAS_P_FROM BAS_P_CDD BAS_V_TEXTSTRING { delete $1;
			delete $2; $$ = $3->Link($4); }
		| BAS_S_RECORD BAS_V_NAME { $1->Link($2); }
		| BAS_P_TITLE BAS_V_TEXTSTRING { $$ = $1->Link($2); }
		| BAS_P_SBTTL BAS_V_TEXTSTRING { $$ = $1->Link($2); }
		| BAS_P_IDENT BAS_V_TEXTSTRING { $$ = $1->Link($2); }
		| BAS_P_ABORT BAS_V_TEXTSTRING { $$ = $1->Link($2); }
		| BAS_P_PRINT BAS_V_TEXTSTRING { $$ = $1->Link($2); }
		| BAS_S_VARIANT
		| BAS_S_END BAS_S_VARIANT { $2->SetType(BAS_N_ENDVARIANT);
			delete $1; $$ = $2; }
		| vardeflist
;

opchan:		/* Empty */ { $$ = 0; }
		| chnlexp ',' { $$ = $1; delete $2; }
;

vardeflist:	vardef { $$ = $1; }
		| vardef ',' vardeflist { $2->SetType(BAS_N_LIST);
			$$ = $2->Link($1, $3); }
;

vardef:		vartype variablex { 
			$$ = ($1->Link(0, 0, $2)); }
		| BAS_V_NAME variablex { $1->SetType(BAS_N_VARTYPE);
			$$ = ($1->Link(0, 0, $2)); }
		| vartype variablex '=' expression {
			$$ = $1->Link(0, 0, $2->Link(0, 0, $4));
			delete $3; }
		| BAS_V_NAME variablex '=' expression {
			$1->SetType(BAS_N_VARTYPE);
			$$ = $1->Link(0, 0, $2->Link(0, 0, $4));
			delete $3; }
;

dimby:		{ $$ = NULL; }
		| BAS_S_DIM '(' dimcomma ')' { $$ = $3; delete $1; delete $2; delete $4; }
		| '(' dimcomma ')' { $$ = $2; delete $1; delete $3; }
;

dimcomma:	{ $$ = NULL; }
		| dimcomma ',' dimcomma { $$ = $2->Link($1, $3); }
		| BAS_V_INTEGER { $$ = $1; }
		| BAS_V_INT { $$ = $1; }
		| BAS_V_NAME { $$ = $1; }
;

opformalname:	{ $$ = NULL; }
		| BAS_V_NAME { $$ = $1; }
;

opdefault:	{ $$ = NULL; }
		| '=' expression { $$ = $2; delete $1; }
;

formalvardef:	vartype opformalname dimby opdefault funby { $$ =
			(new Node(BAS_V_DEFINEVAR))->Link($2,
			$1, $3, $4, $5); }
		| BAS_V_NAME BAS_V_NAME dimby opdefault funby {
			$1->SetType(BAS_N_VARTYPE);
			$$ = (new Node(BAS_V_DEFINEVAR))->Link($2,
			$1, $3, $4, $5); }
		| BAS_V_NAME dimby opdefault funby {
			$$ = (new Node(BAS_V_DEFINEVAR))->Link($1,
			0, $2, $3, $4); }
;

constlist:	constassign
		| constassign ',' constlist { delete $2;
			$$ = $1->DownLink($3); }
;

constassign:	variable '=' expression { delete $2;
			$$ = (new Node(BAS_V_DEFINEVAR))->Link($1,
			0, 0, 0, $3); }
		| variable { $$ = (new Node(BAS_V_DEFINEVAR))->Link($1); }
;

assignment:	aslist '=' expression { $2->SetType(BAS_N_ASSIGN);
			$$ = $2->Link($1, $3); }
;

aslist:		variable ',' aslist { $2->SetType(BAS_N_ASSIGNLIST);
			$$ = $2->Link($1, $3); }
		| variable
;


forassignment:	variable '=' expression { $2->SetType(BAS_N_FORASSIGN);
			$$ = $2->Link($1, $3); }
;

chnlexp:	'#' expression { $$ = $1->Link($2); }
;

chexpression:	expression
		| chnlexp { $$ = $1; }
;

matexpression: BAS_S_ZER { $$ = $1; }
		| BAS_S_CON { $$ = $1; }
		| BAS_S_IDN { $$ = $1; }
		| BAS_S_NAME '+' BAS_S_NAME { $$ = $2->Link($1, $3); }
		| BAS_S_NAME '-' BAS_S_NAME { $$ = $2->Link($1, $3); }
		| BAS_S_NAME '*' BAS_S_NAME { $$ = $2->Link($1, $3); }
		| '(' constant ')' '*' BAS_S_NAME { $$ = $1->Link($2, $5);
			delete $3; delete $4; }
		| BAS_S_TRN '(' BAS_S_NAME ')' { $$ = $1->Link($3);
			delete $2; delete $4; }
		| BAS_S_INV '(' BAS_S_NAME ')' { $$ = $1->Link($3);
			delete $2; delete $4; }
;

expression:	expression BAS_S_EQV exp1 { $$ = $2->Link($1, $3); }
		| exp1
;

exp1:		exp1 BAS_S_IMP exp3 { $$ = $2->Link($1, $3); }
		| exp3
;

exp3:		exp3 BAS_S_OR exp4  { if ($1->IsLogical() &&
			$3->IsLogical()){$2->SetType(BAS_X_LOR);}
			$$ = $2->Link($1, $3); }
		| exp3 BAS_S_XOR exp4 { $$ = $2->Link($1, $3); }
		| exp4
;

exp4:		exp4 BAS_S_AND exp5 { if ($1->IsLogical() &&
			$3->IsLogical()){$2->SetType(BAS_X_LAND);}
			$$ = $2->Link($1, $3); }
		| exp5
;

exp5:		BAS_S_NOT exp5 { if($2->IsLogical()){$1->SetType(BAS_X_LNOT); }
			$$ = $1->Link($2); }
		| exp6
;

exp6:		exp6 '>' exp7  { $$ = $2->Link($1, $3); }
		| exp6 '<' exp7  { $$ = $2->Link($1, $3); }
		| exp6 '=' exp7  { $$ = $2->Link($1, $3); }
		| exp6 BAS_X_NEQ exp7  { $$ = $2->Link($1, $3); }
		| exp6 BAS_X_GE exp7  { $$ = $2->Link($1, $3); }
		| exp6 BAS_X_LE exp7  { $$ = $2->Link($1, $3); }
		| exp6 BAS_X_EQ exp7  { $$ = $2->Link($1, $3); }
		| exp7
;

exp7:		exp7 '+' exp8  { $$ = $2->Link($1, $3); }
		| exp7 '-' exp8  { $$ = $2->Link($1, $3); }
		| exp8
;

exp8:		exp8 '*' exp9  { $$ = $2->Link($1, $3); }
		| exp8 '/' exp9  { $$ = $2->Link($1, $3); }
		| exp9
;

exp9:		'-' exp9 { $1->SetType(BAS_N_UMINUS);
			$$ = $1->Link($2); }
		| '+' exp9 { $1->SetType(BAS_N_UPLUS);
			$$ = $1->Link($2); }
		| exp10
;

exp10:		exp10 '^' exp11  { $$ = $2->Link($1, $3);
			NeedMathH = 1;}
		| exp11
;

exp11:		'(' expression ')' { delete $3; $$ = $1->Link($2); }
		| expvalue
;

expvalue:	variable
		| constant
		| function
		| BAS_V_PREDEF
;

prenexprlist:	variable
		| '(' exprlist ')' {$$ = $2; delete $1; delete $3; }
;

exprlist:	/* Empty */ { $$ = 0; }
		| expression
		| expression ',' exprlist { $2->SetType(BAS_N_LIST);
			$$ = $2->Link($1, $3); }
		| nullexpr ',' exprlist { $2->SetType(BAS_N_LIST);
			$$ = $2->Link($1, $3); }
;

chnllist:	'#' chnllist { $$ = $2; delete $1; }
		| expression ',' chnllist { $$ = $2->Link($1, $3);
			$2->SetType(BAS_N_LIST); }
		| expression
;

datalist:	/* Empty */ { $$ = 0; }
		| dataitem
		| dataitem ',' datalist { $2->SetType(BAS_N_LIST);
			$$ = $2->Link($1, $3); }
		| nullexpr ',' datalist { $2->SetType(BAS_N_LIST);
			$$ = $2->Link($1, $3); }
;

dataitem:	constant
		| '-' constant { $1->SetType(BAS_N_UMINUS);
			$$ = $1->Link($2); }
		| BAS_V_NAME
;

paramlist:	/* Empty */ { $$ = 0; }
		| expressionx
		| expressionx ',' paramlist { $2->SetType(BAS_N_LIST);
			$$ = $2->Link($1, $3); }
		| nullexpr ',' paramlist { $2->SetType(BAS_N_LIST);
			$$ = $2->Link($1, $3); }
;

caseexprlist:	caseexpression
		| caseexpression ',' caseexprlist { $2->SetType(BAS_N_LIST);
			$$ = $2->Link($1, $3); }
;

caseexpression: expression
		| '=' expression { delete $1; $$=$2; }
		| '<' expression { $1->SetType(BAS_X_UNARYLT);
			$$ = $1->Link($2); }
		| BAS_X_LE expression { $1->SetType(BAS_X_UNARYLE);
			$$ = $1->Link($2); }
		| '>' expression { $1->SetType(BAS_X_UNARYGT);
			$$ = $1->Link($2); }
		| BAS_X_GE expression { $1->SetType(BAS_X_UNARYGE);
			$$ = $1->Link($2); }
		| BAS_X_NEQ expression { $1->SetType(BAS_X_UNARYNEQ);
			$$ = $1->Link($2); }
		| expression BAS_S_TO expression { $$ = $2->Link($1, $3); }
;

expressionx:	expression
		| expression BAS_S_BY tranmech { $$ = $2->Link($1, $3); }
;


nullexpr:	/* Empty */ {$$ = 0; }
;

linelist:	lineno
		| lineno ',' linelist { $2->SetType(BAS_N_LIST);
			$$ = $2->Link($1, $3); }
;

lineno:		BAS_V_NAME { $1->SetType(BAS_V_USELABEL); $$ = $1; }
		| BAS_V_INTEGER { $1->SetType(BAS_V_USELABEL); $$ = $1; }
		| BAS_V_INT { $1->SetType(BAS_V_USELABEL); $$ = $1; }
;

opother:	{ $$ = 0; } /* Empty */
		| BAS_S_OTHERWISE lineno { $$ = $2; delete $1; }
;

readlist:	readlist ',' readlist { delete $2;
			$$ = $1->DownLink($3); }
		| variable {$$ = (new Node(BAS_S_READ, "", 0))->Link($1); }
;

matreadlist:	matreadlist ',' matreadlist { delete $2;
			$$ = $1->DownLink($3); }
		| variable {$$ = (new Node(BAS_V_MATREAD, "", 0))->Link($1); }
;

formalname:	BAS_V_NAME
;

formalparam:	{ $$ = 0; }
		| '(' formalplist ')' { if ($2 != 0) { $2->SmoothTypes(); }
			$$ = $2; delete $1; delete $3; }
;

formalplist:	{ $$ = 0; }
		| formalvardef ',' formalplist {
		  delete $2; $$ = $1->DownLink($3); }
		| formalvardef
;

dimplist:	dimvardef ',' dimplist { delete $2; $$ = $1->DownLink($3); }
		| dimvardef
;

dimvardef:	vartype formalname dimension opdefault { $$ =
			(new Node(BAS_V_DEFINEVAR))->Link($2,
			$1, $3, $4); }
		| BAS_V_NAME BAS_V_NAME dimension opdefault {
			$1->SetType(BAS_N_VARTYPE);
			$$ = (new Node(BAS_V_DEFINEVAR))->Link($2,
			$1, $3, $4); }
		| BAS_V_NAME dimension opdefault {
			$$ = (new Node(BAS_V_DEFINEVAR))->Link($1,
			0, $2, $3); }
;

dimension:	'(' dimensionlist ')' { $$ = $2; delete $3; delete $1; }
;

dimensionlist:	expression { $$ = $1; }
		| expression ',' dimensionlist { delete $2;
			$$ = $1->DownLink($3); }
;

variable:	variablex
		| variable BAS_X_STRREF variabley { $$ = $2->Link($1, $3); }
;

variablex:	BAS_V_NAME
		| BAS_V_NAME '(' paramlist ')' { if ($3 == 0)
			{ $2->SetType(BAS_N_NULL);
			  $$ = $1->Link($2);
			} else
			{ $$ = $1->Link($3);
			  delete $2; }
			delete $4; }
;

variabley:	BAS_V_NAME { $1->SetType(BAS_N_STRUCTNAME); $$ = $1; }
		| BAS_V_NAME '(' paramlist ')' { $1->SetType(BAS_N_STRUCTNAME);
			if ($3 == 0)
			{ $2->SetType(BAS_N_NULL);
			  $$ = $1->Link($2);
			} else
			{ $$ = $1->Link($3);
			  delete $2; }
			delete $4; }
;

constant:	BAS_V_INT
		| BAS_V_INTEGER
		| BAS_V_FLOAT
		| BAS_V_TEXTSTRING
;

function:	BAS_V_FUNCTION '(' paramlist ')' { $$ = $1->Link($3);
			delete $2; delete $4; }
		| BAS_S_MOD '(' expressionx ',' expressionx ')' {
			$$ = $1->Link($3, $5);
			delete $2; delete $4; delete $6; }
		| vartypetwo '(' expression ',' vartypeone ')' {
			$1->SetType(BAS_N_TYPEFUN);
			$$ = $1->Link($3, $5);
			delete $2; delete $4; delete $6; }
;

optfor:		/* null */ { $$ = 0; }
		| BAS_S_FOR BAS_S_INPUT { $$ = $2; delete $1; }
		| BAS_S_FOR BAS_S_OUTPUT { $$ = $2; delete $1; }
;

optiontype:	'(' optionlist ')' { delete $1; delete $3; $$ = 0; }
		| BAS_S_INTEGER BAS_S_BYTE { IntegerType = VARTYPE_BYTE;
			delete $1; delete $2; $$ = 0; }
		| BAS_S_INTEGER BAS_S_WORD { IntegerType = VARTYPE_WORD;
			delete $1; delete $2; $$ = 0; }
		| BAS_S_INTEGER BAS_S_LONG { IntegerType = VARTYPE_LONG;
			delete $1; delete $2; $$ = 0; }
		| BAS_S_REAL BAS_S_SINGLE { RealType = VARTYPE_SINGLE;
			delete $1; delete $2; $$ = 0; }
		| BAS_S_REAL BAS_S_DOUBLE { RealType = VARTYPE_DOUBLE;
			delete $1; delete $2; $$ = 0; }
		| BAS_S_REAL BAS_S_GFLOAT { RealType = VARTYPE_GFLOAT;
			delete $1; delete $2; $$ = 0; }
		| BAS_S_REAL BAS_S_HFLOAT { RealType = VARTYPE_HFLOAT;
			delete $1; delete $2; $$ = 0; }
;

optionlist:	optiontype ',' optionlist { delete $2; $$ = 0; }
		| optiontype { $$ = 0; }
;

datatype:	/* Null */ { $$ = 0; }
		| vartype
;

vartype:	vartypeone
		| vartypetwo
;

vartypeone:	BAS_S_BYTE
		| BAS_S_DOUBLE
		| BAS_S_GFLOAT
		| BAS_S_HFLOAT
		| BAS_S_LONG
		| BAS_S_WORD
;

vartypetwo:	BAS_S_INTEGER
		| BAS_S_REAL
		| BAS_S_STRING
		| BAS_S_RFA
		| BAS_S_DECIMAL '(' exprlist ')' { delete $2;
			delete $4; delete $3; $$ = $1; }
;

fundeflist:	fundef
		| fundeflist ',' fundef { $$ = $1->DownLink($3); delete $2; }
;

fundef:		BAS_V_NAME funby { $1->SetType(BAS_V_FUNCTION);
			$$ = (new Node(BAS_V_DEFINEFUN))->Link($1, 0, 0, $2); }
		| BAS_V_NAME funby '(' funparlist ')' { $1->SetType(BAS_V_FUNCTION);
			$$ = (new Node(BAS_V_DEFINEFUN))->Link($1, 0, $4, $2);
			delete $3; delete $5; }
;

funby:		{ $$ = 0; } /* This stuff Ignored for now */
		| BAS_S_BY tranmech { delete $1; delete $2; $$ = 0; }
;

funparlist:	{ $$ = 0; }
		| funpar
		| funparlist ',' funpar { $2->SetType(BAS_N_LIST);
			$$ = $2->Link($1, $3); }
; 

funpar:		funpara { $$ = $1; }
		| funpara BAS_S_BY tranmech { $$ = $2->Link($1, $3); }
;

funpara:	vartype fundim funsiz { $$ = $1->Link(0, $2, $3); }
		| vartype BAS_V_NAME fundim funsiz {
			$$ = $1->Link($4, $3, $2); }
		| BAS_V_NAME fundim funsiz {
			$$ = $1->Link(0, $2, $3); }
		| BAS_V_NAME BAS_V_NAME fundim funsiz { $1->SetType(BAS_N_VARTYPE);
			$$ = $1->Link($4, $3, $2); }
;

fundim:		/* Empty */ { $$ = 0; }
		| BAS_S_DIM '(' fundimcom ')' { delete $2; delete $4;
			$$ = $1->Link($3); }
		| '(' fundimcom ')' { delete $1; delete $2;
			$$ = $2; }
;

fundimcom:	/* empty */ { $$ = 0; }
		| expression { $$ = $1; }
		| ',' fundimcom { $1->SetType(BAS_N_LIST);
			$$ = $1->Link(0, $2); }
		| expression ',' fundimcom { $2->SetType(BAS_N_LIST);
			$$ = $2->Link($1, $3); }
;

funsiz:		/* empty */ { $$ = 0; }
		| '=' expression { $$ = $2; delete $1; }
;

funequ:		/* Empty */ { $$ = 0; }
		| '=' expression { $1->SetType(BAS_N_ASSIGN);
			$$ = $1->Link(0, $2); }
;

getclause:	/* empty */	{ $$ = 0; }
		| ',' BAS_S_RECORD expression getclause { 
			$1->SetType(BAS_N_LIST);
			$2->SetType(BAS_V_RMSRECORD);
			$$ = $1->Link($2->Link($3), $4); }
		| ',' BAS_S_REGARDLESS getclause { $1->SetType(BAS_N_LIST);
			$$ = $1->Link($2, $3); }
		| ',' BAS_S_KEY '#' expression getclause { $1->SetType(BAS_N_LIST);
			delete $3;
			$$ = $1->Link($2->Link($4), $5); }
		| ',' BAS_S_KEY '#' expression BAS_S_EQ expression getclause
			{ $1->SetType(BAS_N_LIST);
			delete $3;
			$$ = $1->Link($2->Link($4, $5, $6), $7); }
		| ',' BAS_S_KEY '#' expression BAS_S_GE expression getclause
			{ $1->SetType(BAS_N_LIST);
			delete $3;
			$$ = $1->Link($2->Link($4, $5, $6), $7); }
		| ',' BAS_S_KEY '#' expression BAS_S_GT expression getclause
			{ $1->SetType(BAS_N_LIST);
			delete $3;
			$$ = $1->Link($2->Link($4, $5, $6), $7); }
		| ',' BAS_S_RFA expression getclause { $1->SetType(BAS_N_LIST);
			$$ = $1->Link($2->Link($3), $4); }
		| ',' BAS_S_COUNT expression getclause { $1->SetType(BAS_N_LIST);
			$$ = $1->Link($2->Link($3), $4); }
		| ',' BAS_S_BLOCK expression getclause { $1->SetType(BAS_N_LIST);
			$$ = $1->Link($2->Link($3), $4); }
;

optprint:	optprint1 { $$ = $1; }
		| optprint1 hemisemi optprint { $$ = $2->Link($1, $3); }
;

optprint1:	/* Empty */	{ $$ = 0; }
		| chnlexp { $$ = $1; NeedChannel = 1; }
		| BAS_S_USING expression { NeedPuse = 1; $$ = $1->Link($2); }
		| chnlexp BAS_S_USING expression {
			NeedPuse = 1; $2->Link($3);
			$$ = (new Node(BAS_N_LIST, ";", 0))->Link($1, $2);
			NeedChannel = 1; }
		| BAS_S_RECORD expression { $1->SetType(BAS_N_RECORD);
			$$ = $1->Link($2); }
		| BAS_S_COUNT expression { $$ = $1->Link($2); }
		| optprintexpr { $$ = $1; }
;

optprintexpr:	expression	{ $$ = $1; }
		| optprintexpr expression { $$ = (new Node(BAS_N_LIST,
			";", 0))->Link($1, $2); }
;

optinput:	optinput1 { $$ = $1; }
		| optinput1 hemisemi optinput { $$ = $2->Link($1, $3); }
;

optinput1:	/* Empty */	{ $$ = 0; }
		| chnlexp { $$ = $1; }
		| optinputexpr { $$ = $1; }
;

optinputexpr:	expression	{ $$ = $1; }
		| optinputexpr expression { $$ = (new Node(BAS_N_LIST,
			";", 0))->Link($1, $2); }
;

hemisemi:	',' { $1->SetType(BAS_N_LIST); $$ = $1; }
		| ';' { $1->SetType(BAS_N_LIST); $$ = $1; }
;

passmech:	{ $$ = 0; }
		| BAS_S_BY tranmech { $$ = $2; delete $1; }
;

tranmech:	BAS_S_VALUE
		| BAS_S_DESC
		| BAS_S_REF
;

openlist:	/* Empty */	{ $$ = 0; }
		| openitem openlist { $$ = $1->DownLink($2); }
;

openitem:	',' BAS_S_ORGANIZATION orgclause { $$ = $2->Link($3);
			delete $1; }
		| ',' BAS_S_ALLOW allowtype { $$ = $2->Link($3);
			delete $1; }
		| ',' BAS_S_ACCESS allowtype { $$ = $2->Link($3);
			delete $1; }
		| ',' BAS_S_CLUSTERSIZE expression { $$ = $2->Link($3);
			delete $1; }
		| ',' BAS_S_RECORDTYPE recordtype { $$ = $2->Link($3);
			delete $1; }
		| ',' BAS_S_RECORDSIZE expression { $$ = $2->Link($3);
			delete $1; }
		| ',' BAS_S_RECORD BAS_S_SIZE expression { $$ = $2->Link($4);
			delete $1; delete $3; $2->SetType(BAS_S_RECORDSIZE); }
		| ',' BAS_S_FILESIZE expression { $$ = $2->Link($3);
			delete $1; }
		| ',' BAS_S_WINDOWSIZE expression { $$ = $2->Link($3);
			delete $1; }
		| ',' BAS_S_TEMPORARY { $$ = $2; delete $1; }
		| ',' BAS_S_CONTIGUOUS { $$ = $2; delete $1; }
		| ',' BAS_S_MAP variable { $$ = $2->Link($3);
			delete $1; }
		| ',' BAS_S_CONNECT expression { $$ = $2->Link($3);
			delete $1; }
		| ',' BAS_S_BUFFER expression { $$ = $2->Link($3);
			delete $1; }
		| ',' BAS_S_DEFAULTNAME expression { $$ = $2->Link($3);
			delete $1; }
		| ',' BAS_S_EXTENDSIZE expression { $$ = $2->Link($3);
			delete $1; }
		| ',' BAS_S_BLOCKSIZE expression { $$ = $2->Link($3);
			delete $1; }
		| ',' BAS_S_NOREWIND { $$ = $2; delete $1; }
		| ',' BAS_S_NOSPAN { $$ = $2; delete $1; }
		| ',' BAS_S_SPAN { $$ = $2; delete $1; }
		| ',' BAS_S_BUCKETSIZE expression { $$ = $2->Link($3);
			delete $1; }
		| ',' BAS_S_PRIMARY BAS_S_KEY prenexprlist dupcha { delete $1;
			delete $3; $$ = $2->Link($4); }
		| ',' BAS_S_PRIMARY prenexprlist dupcha { delete $1;
			$$ = $2->Link($3); }
		| ',' BAS_S_ALTERNATE BAS_S_KEY prenexprlist dupcha { delete $1;
			delete $3; $$ = $2->Link($4); }
		| ',' BAS_S_MODE expression { delete $1;
			$$ = $2->Link($3); }
		| ',' BAS_S_VIRTUAL { $$ = $2; delete $1; }
;

orgclause:	BAS_S_INDEXED orgtype {$$ = $1->Link($2); }
		| BAS_S_RELATIVE orgtype {$$ = $1->Link($2); }
		| BAS_S_VIRTUAL orgtype {$$ = $1->Link($2); }
		| BAS_S_SEQUENTIAL orgtype {$$ = $1->Link($2); }
		| BAS_S_UNDEFINED orgtype {$$ = $1->Link($2); }
;

orgtype:	/* Empty */	{ $$ = 0; }
		| BAS_S_STREAM
		| BAS_S_VARIABLE
		| BAS_S_FIXED
;

allowtype:	BAS_S_NONE
		| BAS_S_READ
		| BAS_S_WRITE
		| BAS_S_MODIFY
		| BAS_S_APPEND
		| BAS_S_SCRATCH
;

recordtype:	BAS_S_LIST
		| BAS_S_FORTRAN
		| BAS_S_NONE
		| BAS_S_ANY
;

dupcha:		/* Empty */ { $$ = 0; }
		| BAS_S_NODUPLICATES dupcha { $$ = $1->Link($2); }
		| BAS_S_DUPLICATES dupcha { $$ = $1->Link($2); }
		| BAS_S_CHANGES dupcha { $$ = $1->Link($2); }
		| BAS_S_NOCHANGES dupcha { $$ = $1->Link($2); }
;

fieldlist:	/* Empty */ { $$ = 0; }
		| fielditem
		| fielditem ',' fieldlist { $2->SetType(BAS_N_LIST);
			$$ = $2->Link($1, $3); }
;

fielditem:	expression BAS_S_AS variablex { $$ = $2->Link($1, $3); }
;
%%

/*
 * Display error messages
 */
int yyerror(char *s)
{
	std::cerr << "Parse error: " << s << " near line " << xline << std::endl;
	if (include_stack_pointer > 0)
	{
		std::cerr << " in included file " <<
			include_name[include_stack_pointer - 1] << std::endl;
	}
	return 0;
}

