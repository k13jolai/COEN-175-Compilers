/*
 * File:	parser.c
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the recursive-descent parser for
 *		Simple C.
 */

# include <cstdlib>
# include <iostream>
# include "tokens.h"
# include "lexer.h"
# include "lexer.cpp"
# include "scope.h"
# include <string>

std::stack<SCOPE> SYMTABLE;
SCOPE SCOPE0;
string strlist[100];
string strname;
int listsize=0;
SCOPE* currentscope=&SCOPE0;
SCOPE* outtermost=&SCOPE0;
//This is the global scope, which remains until EOF
static int lookahead, nexttoken;
static string lexbuf, nextbuf;

static void expression();
static void statement();


bool declarevar(SYMBOL* CHECK){
	SYMBOL* CHECKAGAINST;
	if(currentscope->enclosing==NULL){
		CHECKAGAINST=currentscope->FIND(CHECK->name);
		if(CHECKAGAINST!=NULL && CHECK->stype!=CHECKAGAINST->stype)
			report("conflicting types for '%s'",CHECK->name);
			return false;		
	}
	else{
		if(currentscope->FIND(CHECK->name)!=NULL)
			report("redeclaration of '%s'",CHECK->name);
			return false;
	}
	return true;
}
bool definefun(SYMBOL* CHECK){
	SYMBOL* CHECKAGAINST;
	CHECKAGAINST=currentscope->FIND(CHECK->name);
	if(CHECKAGAINST!=NULL){
		if(CHECKAGAINST->stype!=CHECK->stype){
			report("conflicting types for '%s'",CHECK->name);
			return false;
		}
		else if(CHECKAGAINST->defined!=0){
			report("redefinition of '%s'",CHECK->name);
			return false;
		}
	}
	else{
		CHECK->defined=1;
		return true;
	}
}
		



bool definedlist(string name){
	int i;
	for(i=0;i<listsize;i++){
		if(strlist[i].compare(name)==0)
			return true;
	}
	return false;
}
static void openscope(){
	SCOPE NSCOPE;
	NSCOPE.enclosing=&SYMTABLE.top();
	SYMTABLE.push(NSCOPE);	
	currentscope=&SYMTABLE.top();
}

static void closescope(){
	SYMTABLE.pop();
	currentscope=&SYMTABLE.top();
}
/*
 * Function:	error
 *
 * Description:	Report a syntax error to standard error.
 */

static void error()
{
    if (lookahead == DONE)
	report("syntax error at end of file");
    else
	report("syntax error at '%s'", lexbuf);

    exit(EXIT_FAILURE);
}


/*
 * Function:	match
 *
 * Description:	Match the next token against the specified token.  A
 *		failure indicates a syntax error and will terminate the
 *		program since our parser does not do error recovery.
 */

static void match(int t)
{
    if (lookahead != t)
	error();

    if (nexttoken) {
	lookahead = nexttoken;
	lexbuf = nextbuf;
	nexttoken = 0;
    } else
	lookahead = lexan(lexbuf);
}


/*
 * Function:	peek
 *
 * Description:	Return the next token in the input stream and save it so
 *		that match() will later return it.
 */

static int peek()
{
    if (!nexttoken)
	nexttoken = lexan(nextbuf);

    return nexttoken;
}


/*
 * Function:	isSpecifier
 *
 * Description:	Return whether the given token is a type specifier.
 */

static bool isSpecifier(int token)
{
    return token == INT || token == CHAR || token == STRUCT;
}


/*
 * Function:	specifier
 *
 * Description:	Parse a type specifier.  Simple C has only ints, chars, and
 *		structure types.
 *
 *		specifier:
 *		  int
 *		  char
 *		  struct identifier
 */

static string specifier()
{
    if (lookahead == INT) {
	match(INT);
	return "INT";
    }

    if (lookahead == CHAR) {
	match(CHAR);
	return "CHAR";
    }

    match(STRUCT);
	strname=lexbuf;
    match(ID);
    return "STRUCT";
}


/*
 * Function:	pointers
 *
 * Description:	Parse pointer declarators (i.e., zero or more asterisks).
 *
 *		pointers:
 *		  empty
 *		  * pointers
 */

static int pointers()
{
	int count=0;
    while (lookahead == '*'){
		count++;
		match('*');	
	}
	return count;
}


/*
 * Function:	declarator
 *
 * Description:	Parse a declarator, which in Simple C is either a scalar
 *		variable or an array, with optional pointer declarators.
 *
 *		declarator:
 *		  pointers identifier
 *		  pointers identifier [ num ]
 */

static void declarator(string s)
{
	int n;
	int m;
	string k;
    n=pointers();
	k=lexbuf;
    match(ID);
	string j;
	if(definedlist(k)==false && n==0){
		report("incomplete type for '%s'",k);
	}
    if (lookahead == '[') {
		match('[');
		j="ARRAY";
		m=atoi(lexbuf.c_str());
		TYPE T(s,n,j,m);
		SYMBOL S(k,T);
		match(NUM);
		match(']');
		if(currentscope->FIND(k)!=NULL)
			report("redeclaration of '%s'",k);
		else
			currentscope->insert(&S);
    }

	else{
		j="SCALAR";
		TYPE T(s,n,j,m);
		SYMBOL S(k,T);
		if(currentscope->FIND(k)!=NULL)
			report("redeclaration of '%s'",k);
		else
			currentscope->insert(&S);
	}
}


/*
 * Function:	declaration
 *
 * Description:	Parse a local variable declaration.  Global declarations
 *		are handled separately since we need to detect a function
 *		as a special case.
 *
 *		declaration:
 *		  specifier declarator-list ';'
 *
 *		declarator-list:
 *		  declarator
 *		  declarator , declarator-list
 */

static void declaration()
{
	string s;
    s=specifier();
    declarator(s);

    while (lookahead == ',') {
	match(',');
	declarator(s);
    }

    match(';');
}


/*
 * Function:	declarations
 *
 * Description:	Parse a possibly empty sequence of declarations.
 *
 *		declarations:
 *		  empty
 *		  declaration declarations
 */

static void declarations()
{
    while (isSpecifier(lookahead))
	declaration();
}


/*
 * Function:	primaryExpression
 *
 * Description:	Parse a primary expression.
 *
 *		primary-expression:
 *		  ( expression )
 *		  identifier ( expression-list )
 *		  identifier ( )
 *		  identifier
 *		  character
 *		  string
 *		  num
 *
 *		expression-list:
 *		  expression
 *		  expression , expression-list
 */

static void primaryExpression()
{
    if (lookahead == '(') {
	match('(');
	expression();
	match(')');

    } else if (lookahead == CHARACTER) {
	match(CHARACTER);

    } else if (lookahead == STRING) {
	match(STRING);

    } else if (lookahead == NUM) {
	match(NUM);

    } else if (lookahead == ID) {
	string s=lexbuf;
	if(currentscope->FIND(s)==NULL && currentscope->LOOKUP(s)==NULL){
		report("'%s' undeclared",s);
	//If we have to check types we may have to add a bit more here.
	}
	match(ID);

	if (lookahead == '(') {
	    match('(');

	    if (lookahead != ')') {
		expression();

		while (lookahead == ',') {
		    match(',');
		    expression();
		}
	    }

	    match(')');
	}

    } else
	error();
}


/*
 * Function:	postfixExpression
 *
 * Description:	Parse a postfix expression.
 *
 *		postfix-expression:
 *		  primary-expression
 *		  postfix-expression [ expression ]
 *		  postfix-expression . identifier
 *		  postfix-expression -> identifier
 */

static void postfixExpression()
{
    primaryExpression();

    while (1) {
	if (lookahead == '[') {
	    match('[');
	    expression();
	    match(']');
	    cout << "index" << endl;

	} else if (lookahead == '.') {
	    match('.');
	    match(ID);
	    cout << "dot" << endl;

	} else if (lookahead == ARROW) {
	    match(ARROW);
	    match(ID);
	    cout << "arrow" << endl;

	} else
	    break;
    }
}


/*
 * Function:	prefixExpression
 *
 * Description:	Parse a prefix expression.
 *
 *		prefix-expression:
 *		  postfix-expression
 *		  ! prefix-expression
 *		  - prefix-expression
 *		  * prefix-expression
 *		  & prefix-expression
 *		  sizeof prefix-expression
 *		  sizeof ( specifier pointers )
 */

static void prefixExpression()
{
    if (lookahead == '!') {
	match('!');
	prefixExpression();
	cout << "not" << endl;

    } else if (lookahead == '-') {
	match('-');
	prefixExpression();
	cout << "neg" << endl;

    } else if (lookahead == '*') {
	match('*');
	prefixExpression();
	cout << "deref" << endl;

    } else if (lookahead == '&') {
	match('&');
	prefixExpression();
	cout << "addr" << endl;

    } else if (lookahead == SIZEOF) {
	match(SIZEOF);

	if (lookahead == '(' && isSpecifier(peek())) {
	    match('(');
	    specifier();
	    pointers();
	    match(')');
	} else
	    prefixExpression();

	cout << "sizeof" << endl;

    } else
	postfixExpression();
}


/*
 * Function:	castExpression
 *
 * Description:	Parse a cast expression.  If the token after the opening
 *		parenthesis is not a type specifier, we could have a
 *		parenthesized expression instead.
 *
 *		cast-expression:
 *		  prefix-expression
 *		  ( specifier pointers ) cast-expression
 */

static void castExpression()
{
    if (lookahead == '(' && isSpecifier(peek())) {
	match('(');
	specifier();
	pointers();
	match(')');
	castExpression();
	cout << "cast" << endl;

    } else
	prefixExpression();
}


/*
 * Function:	multiplicativeExpression
 *
 * Description:	Parse a multiplicative expression.
 *
 *		multiplicative-expression:
 *		  cast-expression
 *		  multiplicative-expression * cast-expression
 *		  multiplicative-expression / cast-expression
 *		  multiplicative-expression % cast-expression
 */

static void multiplicativeExpression()
{
    castExpression();

    while (1) {
	if (lookahead == '*') {
	    match('*');
	    castExpression();
	    cout << "mul" << endl;

	} else if (lookahead == '/') {
	    match('/');
	    castExpression();
	    cout << "div" << endl;

	} else if (lookahead == '%') {
	    match('%');
	    castExpression();
	    cout << "rem" << endl;

	} else
	    break;
    }
}


/*
 * Function:	additiveExpression
 *
 * Description:	Parse an additive expression.
 *
 *		additive-expression:
 *		  multiplicative-expression
 *		  additive-expression + multiplicative-expression
 *		  additive-expression - multiplicative-expression
 */

static void additiveExpression()
{
    multiplicativeExpression();

    while (1) {
	if (lookahead == '+') {
	    match('+');
	    multiplicativeExpression();
	    cout << "add" << endl;

	} else if (lookahead == '-') {
	    match('-');
	    multiplicativeExpression();
	    cout << "sub" << endl;

	} else
	    break;
    }
}


/*
 * Function:	relationalExpression
 *
 * Description:	Parse a relational expression.  Note that Simple C does not
 *		have shift operators, so we go immediately to additive
 *		expressions.
 *
 *		relational-expression:
 *		  additive-expression
 *		  relational-expression < additive-expression
 *		  relational-expression > additive-expression
 *		  relational-expression <= additive-expression
 *		  relational-expression >= additive-expression
 */

static void relationalExpression()
{
    additiveExpression();

    while (1) {
	if (lookahead == '<') {
	    match('<');
	    additiveExpression();
	    cout << "ltn" << endl;

	} else if (lookahead == '>') {
	    match('>');
	    additiveExpression();
	    cout << "gtn" << endl;

	} else if (lookahead == LEQ) {
	    match(LEQ);
	    additiveExpression();
	    cout << "leq" << endl;

	} else if (lookahead == GEQ) {
	    match(GEQ);
	    additiveExpression();
	    cout << "geq" << endl;

	} else
	    break;
    }
}


/*
 * Function:	equalityExpression
 *
 * Description:	Parse an equality expression.
 *
 *		equality-expression:
 *		  relational-expression
 *		  equality-expression == relational-expression
 *		  equality-expression != relational-expression
 */

static void equalityExpression()
{
    relationalExpression();

    while (1) {
	if (lookahead == EQL) {
	    match(EQL);
	    relationalExpression();
	    cout << "eql" << endl;

	} else if (lookahead == NEQ) {
	    match(NEQ);
	    relationalExpression();
	    cout << "neq" << endl;

	} else
	    break;
    }
}


/*
 * Function:	logicalAndExpression
 *
 * Description:	Parse a logical-and expression.  Note that Simple C does
 *		not have bitwise-and expressions.
 *
 *		logical-and-expression:
 *		  equality-expression
 *		  logical-and-expression && equality-expression
 */

static void logicalAndExpression()
{
    equalityExpression();

    while (lookahead == AND) {
	match(AND);
	equalityExpression();
	cout << "and" << endl;
    }
}


/*
 * Function:	expression
 *
 * Description:	Parse an expression, or more specifically, a logical-or
 *		expression, since Simple C does not allow comma or
 *		assignment as an expression operator.
 *
 *		expression:
 *		  logical-and-expression
 *		  expression || logical-and-expression
 */

static void expression()
{
    logicalAndExpression();

    while (lookahead == OR) {
	match(OR);
	logicalAndExpression();
	cout << "or" << endl;
    }
}


/*
 * Function:	statements
 *
 * Description:	Parse a possibly empty sequence of statements.  Rather than
 *		checking if the next token starts a statement, we check if
 *		the next token ends the sequence, since a sequence of
 *		statements is always terminated by a closing brace.
 *
 *		statements:
 *		  empty
 *		  statement statements
 */

static void statements()
{
    while (lookahead != '}'){
	statement();
	}
	closescope();
}


/*
 * Function:	statement
 *
 * Description:	Parse a statement.  Note that Simple C has so few
 *		statements that we handle them all in this one function.
 *
 *		statement:
 *		  { declarations statements }
 *		  return expression ;
 *		  while ( expression ) statement
 *		  if ( expression ) statement
 *		  if ( expression ) statement else statement
 *		  expression = expression ;
 *		  expression ;
 */

static void statement()
{
    if (lookahead == '{') {
	match('{');
	openscope();
	declarations();
	statements();
	match('}');

    } else if (lookahead == RETURN) {
	match(RETURN);
	expression();
	match(';');

    } else if (lookahead == WHILE) {
	match(WHILE);
	match('(');
	expression();
	match(')');
	statement();

    } else if (lookahead == IF) {
	match(IF);
	match('(');
	expression();
	match(')');
	statement();

	if (lookahead == ELSE) {
	    match(ELSE);
	    statement();
	}

    } else {
	expression();

	if (lookahead == '=') {
	    match('=');
	    expression();
	}

	match(';');
    }
}


/*
 * Function:	parameter
 *
 * Description:	Parse a parameter, which in Simple C is always a scalar
 *		variable with optional pointer declarators.
 *
 *		parameter:
 *		  specifier pointers identifier
 */

static void parameter()
{
	TYPE temp;
	string s;
	int i;
	string parname;
    s=specifier();
    i=pointers();
	parname=lexbuf;
	if(currentscope->FIND(parname)!=NULL){
		report("redeclaration of '%s'",parname);
	}
	else{
		if(s.compare("INT")==0){
			TYPE T(s,i,"SCALAR",0);
			SYMBOL par(parname, T);
			currentscope->insert(&par);
		}else if(s.compare("CHAR")==0){
			TYPE T(s,i,"SCALAR",0);
			SYMBOL par(parname, T);
			currentscope->insert(&par);
		}else if(s.compare("STRUCT")==0){
			if(i==0){
				report("pointer type required for '%s'",parname);
			}
			else{
			TYPE T(s,i,"STRUCTURE",0);
			SYMBOL par(parname, T);
			currentscope->insert(&par);
			}
		}
	}
    match(ID);
}


/*
 * Function:	parameters
 *
 * Description:	Parse the parameters of a function, but not the opening or
 *		closing parentheses.
 *
 *		parameters:
 *		  void
 *		  parameter-list
 *
 *		parameter-list:
 *		  parameter
 *		  parameter , parameter-list
 */

static void parameters()
{
    if (lookahead == VOID)
	match(VOID);

    else {
	parameter();

	while (lookahead == ',') {
	    match(',');
	    parameter();
	}
    }
}


/*
 * Function:	topLevelDeclaration
 *
 * Description:	Parse a top level declaration, which is either a structure
 *		type definition, function definition, or global variable
 *		declaration.
 *
 *		type-definition:
 *		  struct identifier { declaration declarations } ;
 *
 *		global-declaration:
 *		  specifier global-declarator-list ;
 *
 *		global-declarator-list:
 *		  global-declarator
 *		  global-declarator , global-declarator-list
 *
 *		global-declarator:
 *		  pointers identifier
 *		  pointers identifier ( )
 *		  pointers identifier [ num ]
 *
 *		function-definition:
 *		  specifier pointers identifier ( parameters ) { ... }
 */

static void topLevelDeclaration()
{
	SYMBOL* test;
	string toplevelname;
	string toplevelspec=specifier();
	string topkind;
	int s=0;
	int size=0;

	//IF ITS A TYPEDEF
    if (toplevelspec == "STRUCT" && lookahead == '{') {
		if(definedlist(strname)){
			report("redefinition of '%s'",strname);
		}
		else{
		topkind="STRUCTURE";
		TYPE T(toplevelspec,s,topkind,size);
		SYMBOL S(toplevelname,T);
		currentscope->insert(&S);
		strlist[listsize]=toplevelname;
		listsize++;
		}
	match('{');
	declaration();
	declarations();
	match('}');
	match(';');

    } else {		
	s=pointers();
	toplevelname=lexbuf;
	if(toplevelspec == "STRUCT" && s==0){
		report("pointer type required for '%s'",toplevelname);
	}
	match(ID);
//GLOBAL DECLARATOR 
	if (lookahead == '[') {
	    match('[');
		size=atoi(lexbuf.c_str());
		topkind="ARRAY";
	    match(NUM);
	    match(']'); 
		TYPE T(toplevelspec,s,topkind,size);
		SYMBOL S(toplevelname,T);
		test=currentscope->FIND(toplevelname);
		if(test!=NULL && test->stype!=S.stype)
			report("conflicting types for '%s'", toplevelname);
		else
			currentscope->insert(&S);
	}else if(lookahead==';'){
		topkind=="SCALAR";
		TYPE T(toplevelspec,s,topkind,size);
		SYMBOL S(toplevelname,T);
		test=currentscope->FIND(toplevelname);
		if(test!=NULL && test->stype!=S.stype)
			report("conflicting types for '%s'", toplevelname);
		else
			currentscope->insert(&S);
	}else if (lookahead == '(') {
	    match('(');
		topkind="FUNCTION";
		TYPE T(toplevelspec,s,topkind,size);
		SYMBOL S(toplevelname,T);
		test=currentscope->FIND(toplevelname);
		if(test!=NULL && test->stype!=S.stype)
			report("conflicting types for '%s'", toplevelname);
		else
			currentscope->insert(&S);
	    if (lookahead == ')')
		match(')');
	
//FUNCTON DEF
	    else {
		if(toplevelspec== "STRUCT" && s==0){
			report("pointer type required for '%s'",toplevelname);
		}if(definefun(&S)){
		currentscope->insert(&S);
		openscope();
		parameters();
		match(')');
		match('{');
		declarations();
		statements();
		closescope();
		match('}');
		return;
	    }
	}
	}
//GLOBAL DECLARATOR LIST
	while (lookahead == ',') {
	    match(',');
	   s=pointers();

	    match(ID);
	
	} if (lookahead == '[') {
		match('[');
		match(NUM);
		match(']');

	 } else if (lookahead == '(') {
		match('(');
		match(')');
	  }
	

	match(';');
    }
}



/*
 * Function:	main
 *
 * Description:	Analyze the standard input stream.
 */

int main(){
	SCOPE0.enclosing=NULL;
	SYMTABLE.push(SCOPE0);
    lookahead = lexan(lexbuf);

    while (lookahead != DONE){
	topLevelDeclaration();
	}
    exit(EXIT_SUCCESS);
}
