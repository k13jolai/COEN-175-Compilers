/*
 * File:	Tree.cpp
 *
 * Description:	This file contains the constructor and accessor member
 *		function definitions for abstract syntax trees in Simple C.
 *
 *		The tree is actually built during semantic checking, where
 *		type information is readily available.  Any simplifications
 *		or canonicalizations of the tree are performed when it is
 *		constructed.
 *
 *		The functions here are just the constructors and accessors.
 *		The storage allocation and code generation functions are
 *		located elsewhere.  Most of these functions don't do
 *		anything interesting, and could easily be put in the header
 *		file, but we don't like to do that.
 *
 *		Extra functionality:
 *		- everything (it is optional to construct an AST)
 */

# include "Tree.h"
# include "tokens.h"
# include <sstream>
# include <cstdlib>
# include <iostream>
using namespace std;


/*
 * Function:	Expression::Expression (constructor)
 *
 * Description:	Initialize the expression object to not be an lvalue and to
 *		have the specified type.
 */

Expression::Expression(const Type &type)
    : _type(type), _lvalue(false)
{
}


/*
 * Function:	Expression::type (accessor)
 *
 * Description:	Return the type of this expression.
 */

const Type &Expression::type() const
{
    return _type;
}


/*
 * Function:	Expression::lvalue (accessor)
 *
 * Description:	Return whether this expression is an lvalue.
 */

bool Expression::lvalue() const
{
    return _lvalue;
}


/*
 * Function:	String::String (constructor)
 *
 * Description:	Initialize this string literal.  Yes, the expression
 *		for the length of the array is correct.  The literal itself
 *		includes the quotes but the array length should not.
 */

String::String(const string &value)
    : Expression(Type("char", 0, value.size() - 1)), _value(value)
{
}


/*
 * Function:	String::value (accessor)
 *
 * Description:	Return the value of this string.
 */

const string &String::value() const
{
    return _value;
}


/*
 * Function:	Character::Character (constructor)
 *
 * Description:	Initialize this character literal.
 */

Character::Character(const string &value)
    : Expression(Type("int")), _value(value)
{
}


/*
 * Function:	Character::value (accessor)
 *
 * Description:	Return the value of this character.
 */

const string &Character::value() const
{
    return _value;
}


/*
 * Function:	Identifier::Identifier (constructor)
 *
 * Description:	Initialize this identifier object.  An identifier is an
 *		lvalue if its type is a scalar type.
 */

Identifier::Identifier(const Symbol *symbol)
    : Expression(symbol->type()), _symbol(symbol)
{
    _lvalue = symbol->type().isScalar();
}


/*
 * Function:	Identifier::symbol (accessor)
 *
 * Description:	Return the symbol of this identifier.
 */

const Symbol *Identifier::symbol() const
{
    return _symbol;
}


/*
 * Function:	Number::Number (constructor)
 *
 * Description:	Initialize a number, which has type int.
 */

Number::Number(const string &value)
    : Expression(Type("int")), _value(value)
{
}


/*
 * Function:	Number::Number (constructor)
 *
 * Description:	Initialize a number from a value.
 */

Number::Number(unsigned value)
    : Expression(Type("int"))
{
    stringstream ss;

    ss << value;
    _value = ss.str();
}


/*
 * Function:	Number::value (accessor)
 *
 * Description:	Return the value of this number.
 */

const string &Number::value() const
{
    return _value;
}


/*
 * Function:	Call::Call (constructor)
 *
 * Description:	Initialize a function call expression.
 */

Call::Call(const Symbol *id, const Expressions &args, const Type &type)
    : Expression(type), _id(id), _args(args)
{
}


/*
 * Function:	Field::Field (constructor)
 *
 * Description:	Initialize a field reference expression.
 */

Field::Field(Expression *expr, Identifier *id, const Type &type)
    : Expression(type), _expr(expr), _id(id)
{
    _lvalue = expr->lvalue() && id->lvalue();
}


/*
 * Function:	Not::Not (constructor)
 *
 * Description:	Initialize a logical negation expression.
 */

Not::Not(Expression *expr, const Type &type)
    : Expression(type), _expr(expr)
{
}


/*
 * Function:	Negate::Negate (constructor)
 *
 * Description:	Initialize an arithmetic negation expression.
 */

Negate::Negate(Expression *expr, const Type &type)
    : Expression(type), _expr(expr)
{
}


/*
 * Function:	Dereference::Dereference (constructor)
 *
 * Description:	Initialize a dereference expression.
 */

Dereference::Dereference(Expression *expr, const Type &type)
    : Expression(type), _expr(expr)
{
    _lvalue = true;
}


/*
 * Function:	Address::Address (constructor)
 *
 * Description:	Initialize an address expression.
 */

Address::Address(Expression *expr, const Type &type)
    : Expression(type), _expr(expr)
{
}


/*
 * Function:	Cast::Cast (constructor)
 *
 * Description:	Initialize a cast expression.
 */

Cast::Cast(const Type &type, Expression *expr)
    : Expression(type), _expr(expr)
{
}


/*
 * Function:	Multiply::Multiply (constructor)
 *
 * Description:	Initialize a multiplication expression.
 */

Multiply::Multiply(Expression *left, Expression *right, const Type &type)
    : Expression(type), _left(left), _right(right)
{
}


/*
 * Function:	Divide::Divide (constructor)
 *
 * Description:	Initialize a division expression.
 */

Divide::Divide(Expression *left, Expression *right, const Type &type)
    : Expression(type), _left(left), _right(right)
{
}


/*
 * Function:	Remainder::Remainder (constructor)
 *
 * Description:	Initialize a remainder expression.
 */

Remainder::Remainder(Expression *left, Expression *right, const Type &type)
    : Expression(type), _left(left), _right(right)
{
}


/*
 * Function:	Add::Add (constructor)
 *
 * Description:	Initialize an addition expression.
 */

Add::Add(Expression *left, Expression *right, const Type &type)
    : Expression(type), _left(left), _right(right)
{
}


/*
 * Function:	Subtract::Subtract (constructor)
 *
 * Description:	Initialize a subtraction expression.
 */

Subtract::Subtract(Expression *left, Expression *right, const Type &type)
    : Expression(type), _left(left), _right(right)
{
}


/*
 * Function:	LessThan::LessThan (constructor)
 *
 * Description:	Initialize a less-than expression.
 */

LessThan::LessThan(Expression *left, Expression *right, const Type &type)
    : Expression(type), _left(left), _right(right)
{
}


/*
 * Function:	GreaterThan
 *
 * Description:	Initialize a greater-than expression.
 */

GreaterThan::GreaterThan(Expression *left, Expression *right, const Type &type)
    : Expression(type), _left(left), _right(right)
{
}


/*
 * Function:	LessOrEqual
 *
 * Description:	Initialize a less-than-or-equal expression.
 */

LessOrEqual::LessOrEqual(Expression *left, Expression *right, const Type &type)
    : Expression(type), _left(left), _right(right)
{
}


/*
 * Function:	GreaterOrEqual
 *
 * Description:	Initialize a greater-than-or-equal expression.
 */

GreaterOrEqual::GreaterOrEqual(Expression *left, Expression *right, const Type &type)
    : Expression(type), _left(left), _right(right)
{
}


/*
 * Function:	Equal::Equal (constructor)
 *
 * Description:	Initialize an equality expression.
 */

Equal::Equal(Expression *left, Expression *right, const Type &type)
    : Expression(type), _left(left), _right(right)
{
}


/*
 * Function:	NotEqual::NotEqual (constructor)
 *
 * Description:	Initialize an inequality expression.
 */

NotEqual::NotEqual(Expression *left, Expression *right, const Type &type)
    : Expression(type), _left(left), _right(right)
{
}


/*
 * Function:	LogicalAnd::LogicalAnd (constructor)
 *
 * Description:	Initialize a logical-and expression.
 */

LogicalAnd::LogicalAnd(Expression *left, Expression *right, const Type &type)
    : Expression(type), _left(left), _right(right)
{
}


/*
 * Function:	LogicalOr::LogicalOr (constructor)
 *
 * Description:	Initialize a logical-or expression.
 */

LogicalOr::LogicalOr(Expression *left, Expression *right, const Type &type)
    : Expression(type), _left(left), _right(right)
{
}


/*
 * Function:	Assignment::Assignment (constructor)
 *
 * Description:	Initialize an assignment statement.
 */

Assignment::Assignment(Expression *left, Expression *right)
    : _left(left), _right(right)
{
}


/*
 * Function:	Return::Return (constructor)
 *
 * Description:	Initialize a return statement.
 */

Return::Return(Expression *expr)
    : _expr(expr)
{
}


/*
 * Function:	Block::Block (constructor)
 *
 * Description:	Initialize a block statement.
 */

Block::Block(Scope *decls, const Statements &stmts)
    : _decls(decls), _stmts(stmts)
{
}


/*
 * Function:	Block::declarations (accessor)
 *
 * Description:	Return the declarations of this block.
 */

Scope *Block::declarations() const
{
    return _decls;
}


/*
 * Function:	While::While (constructor)
 *
 * Description:	Initialize a while statement.
 */

While::While(Expression *expr, Statement *stmt)
    : _expr(expr), _stmt(stmt)
{
}


/*
 * Function:	If::If (constructor)
 *
 * Description:	Initialize an if-then or if-then-else statement.
 */

If::If(Expression *expr, Statement *thenStmt, Statement *elseStmt)
    : _expr(expr), _thenStmt(thenStmt), _elseStmt(elseStmt)
{
}


/*
 * Function:	Function::Function (constructor)
 *
 * Description:	Initialize a function object.
 */

Function::Function(const Symbol *id, Block *body)
    : _id(id), _body(body)
{
}

void Function::generate(){
	cout<<_id->name()<<":"<<endl;
	cout<< "\tpushl\t"<<"%"<<"ebp"<<endl;
    cout<<"\tmovl\t"<<"%"<<"esp"<<","<<" "<<"%"<<"ebp" << endl;	
	cout<<"\tsubl\t"<<"$"<<_id->name()<<".size"<<","<<"%"<<"esp"<<endl;
	Symbols vars = _body->declarations()->symbols();
	unsigned i=0;
	int offset=0;

	for(i=0;i<_id->type().parameters()->size();i++)
		vars[i]->setOffset(8+4*i);
	for(;i<vars.size();i++){
		offset-=vars[i]->type().size();
		vars[i]->setOffset(offset);
	}
	_body->generate();
	cout<<"\tmovl\t"<<"%"<<"ebp"<<","<<"%"<<"esp"<<endl;
	cout<<"\tpopl\t"<<"%"<<"ebp"<<endl;
	cout<<"\tret"<<endl<<endl;

	cout << "\t.globl\t" << _id->name()<<endl;
	cout << "\t.set\t" << _id->name()<<".size"<< ", " <<offset*-1<<endl;

}

void Assignment::generate(){
	cout << "\tmovl\t";
	_right->generate();
	cout<<","<<" %"<<"eax"<<endl;
	cout <<"\tmovl\t";
	cout<<"%"<<"eax"<<",";
	_left->generate();
	cout << endl;
}


void Block::generate(){
	unsigned i;
	for(i=0;i<_stmts.size();i++)
		_stmts[i]->generate();
}

void Identifier::generate(){
	if(_symbol->offset==0)
		cout<<_symbol->name();
	else
		cout<<_symbol->offset<<"(%ebp)";
}

void Number::generate(){

	cout << "$"<< value(); 
}
	

void Call::generate(){
	int i;
	for(i=_args.size()-1;i>=0;i--){
		cout<<"\tpushl\t";
		_args[i]->generate();
		 cout<<endl;
		}
		
	cout<<"\tcall\t"<<_id->name();
	cout<< endl;

	int s=4*_args.size();
	cout<<"\taddl\t"<<"$"<<s<<","<<" %"<<"esp"<<endl;

}
