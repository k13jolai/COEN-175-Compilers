/*
 * File:	generator.cpp
 *
 * Description:	This file contains the public and member function
 *		definitions for the code generator for Simple C.
 *
 *		Extra functionality:
 *		- putting all the global declarations at the end
 */

# include <sstream>
# include <iostream>
# include "generator.h"
# include "machine.h"
# include "lexer.h"

using namespace std;

int offset;
static unsigned maxargs;

struct Label{
	static unsigned counter;
	int number;
	Label();
};

unsigned Label::counter=0;

Label::Label(){
	number=counter++;
}

Label GLabel;

ostream &operator <<(ostream &ostr, Label L){
	return ostr << ".L"<<L.number;
}

/*
 * Function:	operator <<
 *
 * Description:	Convenience function for writing the operand of an
 *		expression.
 */


ostream &operator <<(ostream &ostr, Expression *expr)
{
    return ostr << expr->_operand;
}

void Expression::generate(){
	cout<<"oops you didnt implement something"<<endl;
}
void Expression::generate(bool &indirect){
	indirect=false;
	generate();
}
/*
 * Function:	Identifier::generate
 *
 * Description:	Generate code for an identifier.  Since there is really no
 *		code to generate, we simply update our operand.
 */

void assignTempOffset(Expression *expr){
	stringstream ss;
	offset-=expr->type().size();
	ss<<offset<<"(%ebp)";
	expr->_operand=ss.str();
}

void Identifier::generate()
{
    stringstream ss;


    if (_symbol->_offset != 0)
	ss << _symbol->_offset << "(%ebp)";
    else
	ss << global_prefix << _symbol->name();

    _operand = ss.str();
}


/*
 * Function:	Number::generate
 *
 * Description:	Generate code for a number.  Since there is really no code
 *		to generate, we simply update our operand.
 */

void Number::generate()
{
    stringstream ss;


    ss << "$" << _value;
    _operand = ss.str();
}

void Character::generate()
{
	stringstream ss;

	ss<< "$" << charval(_value);
	_operand = ss.str();
}

void String::generate(){
	stringstream ss;
	Label B;
	cout<<"\t.data\t"<<endl;
	cout<<B<<":"<<" .asciz"<< value()<<endl;
	cout<<"\t.text\t"<<endl;
	ss<<B;
	_operand=ss.str();
}

# if STACK_ALIGNMENT == 4

/*
 * Function:	Call::generate
 *
 * Description:	Generate code for a function call expression, in which each
 *		argument is simply a variable or an integer literal.
 */

void Call::generate()
{
    unsigned numBytes = 0;

	assignTempOffset(this);
    for (int i = _args.size() - 1; i >= 0; i --) {
	_args[i]->generate();
	cout << "\tpushl\t" << _args[i] << endl;
	numBytes += _args[i]->type().size();
    }

    cout << "\tcall\t" << global_prefix << _id->name() << endl;
	
	cout <<"\tmovl\t"<<"%eax, "<<this<<endl;
    if (numBytes > 0)
	cout << "\taddl\t$" << numBytes << ", %esp" << endl;
}

# else

/*
 * If the stack has to be aligned to a certain size before a function call
 * then we cannot push the arguments in the order we see them.  If we had
 * nested function calls, we cannot guarantee that the stack would be
 * aligned.
 *
 * Instead, we must know the maximum number of arguments so we can compute
 * the size of the frame.  Again, we cannot just move the arguments onto
 * the stack as we see them because of nested function calls.  Rather, we
 * have to generate code for all arguments first and then move the results
 * onto the stack.  This will likely cause a lot of spills.
 *
 * For now, since each argument is going to be either a number of in
 * memory, we just load it into %eax and then move %eax onto the stack.
 */

void Call::generate()
{
    if (_args.size() > maxargs)
	maxargs = _args.size();

    for (int i = _args.size() - 1; i >= 0; i --) {
	_args[i]->generate();
	cout << "\tmovl\t" << _args[i] << ", %eax" << endl;
	cout << "\tmovl\t%eax, " << i * SIZEOF_ARG << "(%esp)" << endl;
    }

  	 cout << "\tcall\t" << global_prefix << _id->name() << endl;
}

# endif


/*
 * Function:	Assignment::generate
 *
 * Description:	Generate code for this assignment statement, in which the
 *		right-hand side is an integer literal and the left-hand
 *		side is an integer scalar variable.  Actually, the way
 *		we've written things, the right-side can be a variable too.
 */

void Assignment::generate()
{	bool indirect=false;
	
	_left->generate(indirect);
    _right->generate();
	if(indirect){
		if(_left->type().size()==4){
			cout<<"\tmovl\t"<<_right<<", %eax"<<endl;
			cout<<"\tmovl\t"<<_left<<", %ecx"<<endl;
			cout<<"\tmovl\t"<<"%eax, " << "(%ecx)"<<endl;
		}

		else{
			cout<<"\tmovl\t"<<_right<<", %eax"<<endl;
			cout<<"\tmovl\t"<<_left<<", %ecx"<<endl;
			cout<<"\tmovb\t"<<"%al, (%ecx)"<<endl;

		}
	}
	else{
		if(_left->type().size()==4){
    		cout << "\tmovl\t" << _right << ", %eax" << endl;
    		cout << "\tmovl\t%eax, " << _left << endl;
		}	

		else{
    		cout << "\tmovl\t" << _right << ", %eax" << endl;
    		cout << "\tmovb\t%eax, " << _left << endl;
		}
	}
}

/*Function Return::generate
 * Description: Generates code a return statement
 *
 */
void Address::generate(){
	bool indirect;
	_expr->generate(indirect);
	if(indirect){
		_operand= _expr->_operand;
	}
	else{
		cout<<"\tleal \t" <<_expr<<", %eax"<<endl;
		assignTempOffset(this);
		cout<<"\tmovl\t%eax, "<<this<<endl;
	}
}


void Dereference::generate(){
	_expr->generate();
	if(_type.size()==1){
		cout<<"\tmovsbl\t(%eax), %eax" <<endl;
	}
	else
		cout<<"\tmovl\t(%eax), %eax" <<endl;
	assignTempOffset(this);
	cout<<"\tmovl\t%eax, "<<this<<endl;
}
void Dereference::generate(bool &indirect){
	indirect=true;
	_expr->generate();
	_operand=_expr->_operand;
}

void Return::generate(){
	_expr->generate();
	cout << "\tmovl\t" <<_expr<<", %eax"<< endl;
	cout << "\tjmp\t"<<GLabel<<endl;
}

/*
 * Function:	Block::generate
 *
 * Description:	Generate code for this block, which simply means we
 *		generate code for each statement within the block.
 */

void Block::generate()
{
    for (unsigned i = 0; i < _stmts.size(); i ++)
	_stmts[i]->generate();
}


/*
 * Function:	Function::generate
 *
 * Description:	Generate code for this function, which entails allocating
 *		space for local variables, then emitting our prologue, the
 *		body of the function, and the epilogue.
 */

void Function::generate()
{
	Label A;
	GLabel=A;

    offset = 0;


    /* Generate our prologue. */

    allocate(offset);
    cout << global_prefix << _id->name() << ":" << endl;
    cout << "\tpushl\t%ebp" << endl;
    cout << "\tmovl\t%esp, %ebp" << endl;
    cout << "\tsubl\t$" << _id->name() << ".size, %esp" << endl;


    /* Generate the body of this function. */

    maxargs = 0;
    _body->generate();

    offset -= maxargs * SIZEOF_ARG;

    while ((offset - PARAM_OFFSET) % STACK_ALIGNMENT)
	offset --;


    /* Generate our epilogue. */
	cout <<GLabel<<":"<<endl;//MAKE GLOBAL LABEL
    cout << "\tmovl\t%ebp, %esp" << endl;
    cout << "\tpopl\t%ebp" << endl;
    cout << "\tret" << endl << endl;

    cout << "\t.globl\t" << global_prefix << _id->name() << endl;
    cout << "\t.set\t" << _id->name() << ".size, " << -offset << endl;

    cout << endl;
}


/*
 * Function:	generateGlobals
 *
 * Description:	Generate code for any global variable declarations.
 */

void generateGlobals(const Symbols &globals)
{
    if (globals.size() > 0)
	cout << "\t.data" << endl;

    for (unsigned i = 0; i < globals.size(); i ++) {
	cout << "\t.comm\t" << global_prefix << globals[i]->name();
	cout << ", " << globals[i]->type().size();
	cout << ", " << globals[i]->type().alignment() << endl;
    }
}

void Add::generate(){
	_left->generate();
	_right->generate();
	assignTempOffset(this);
	cout<<"\tmovl\t"<<_left<<", %eax"<<endl;
	cout<<"\taddl\t"<<_right<<", %eax" <<endl;
	cout<<"\tmovl\t%eax, "<< this << endl;

}
void Subtract::generate(){
	_left->generate();
	_right->generate();
	assignTempOffset(this);
	cout<<"\tmovl\t"<<_left<<", %eax"<<endl;
	cout<<"\tsubl\t"<<_right<<", %eax" <<endl;
	cout<<"\tmovl\t%eax, "<< this << endl;

}

void Negate::generate(){
	_expr->generate();
	assignTempOffset(this);
	cout<<"\tmovl\t"<<_expr<<", %eax"<<endl;
	cout<<"\tnegl\t"<<"%eax"<<endl;
	cout<<"\tmovl\t%eax, "<<this << endl;
}

void Not::generate(){
	_expr->generate();
	assignTempOffset(this);
	cout<<"\tmovl\t"<<_expr<<", %eax"<<endl;
	cout<<"\tcmpl\t"<<"$0, %eax"<<endl;
	cout<<"\tsete\t"<<"%al"<<endl;
	cout<<"\tmovzbl\t"<<"%al, %eax"<<endl;
	cout<<"\tmovl\t"<<"%eax, "<< this <<endl;	
}

void Remainder::generate(){
	_left->generate();
	_right->generate();
	assignTempOffset(this);
	cout<<"\tmovl\t"<<_left<<", %eax"<<endl;
	cout<<"\tcltd\t"<<endl;
	cout<<"\tidiv\t"<<_right<<endl;
	cout<<"\tmovl\t%edx, "<< this << endl;
}


void Divide::generate(){
	_left->generate();
	_right->generate();
	assignTempOffset(this);
	cout<<"\tmovl\t"<<_left<<", %eax"<<endl;
	cout<<"\tcltd\t"<<endl;
	cout<<"\tidiv\t"<<_right<<", %eax" <<endl;
	cout<<"\tmovl\t%eax, "<< this << endl;
}
void Multiply::generate(){
	_left->generate();
	_right->generate();
	assignTempOffset(this);
	cout<<"\tmovl\t"<<_left<<", %eax"<<endl;
	cout<<"\timul\t"<<_right<<", %eax" <<endl;
	cout<<"\tmovl\t%eax, "<< this << endl;
}
void LogicalOr::generate(){
	Label C;
	_left->generate();
	assignTempOffset(this);
	cout<<"\tcmpl\t$0, "<<_left<<endl;
	cout<<"\tjne\t"<< C<<endl;//LABEL
	_right->generate();
	cout<<"\tcmpl\t$0, "<<_right<<endl;
	cout<<C<<":"<<endl;
	cout<<"\tsetne\t%al"<<endl;
	cout<<"\tmovzbl\t%al, %eax"<<endl;
	cout<<"\tmovl\t%eax, "<<this<<endl;
}

void LogicalAnd::generate(){
	Label C;
	_left->generate();
	assignTempOffset(this);
	cout<<"\tcmpl\t$0, "<<_left<<endl;
	cout<<"\tje\t"<< C<< endl;//LABEL
	_right->generate();
	cout<<"\tcmpl\t$0, "<<_right<<endl;
	cout<<C<<":"<<endl;
	cout<<"\tsetne\t%al"<<endl;
	cout<<"\tmovzbl\t%al, %eax"<<endl;
	cout<<"\tmovl\t%eax, "<<this<<endl;
}

void Cast::generate(){
	int src=_expr->type().size();
	int des=_type.size();
	_expr->generate();

	if(des>src){
		assignTempOffset(this);
		cout<<"\tmovb\t"<<_expr<<", %al"<<endl;
		cout<<"\tmovsbl\t"<<" %al, %eax"<<endl;
		cout<<"\tmovl\t"<<"%eax, "<<this<<endl;
	}
	else if(src>des){
		assignTempOffset(this);
		//movl expr eax
		//movb al this
		cout<<"\tmovl\t"<<_expr<<", %eax"<<endl;
		cout<<"\tmovb\t"<<"%al, "<< this<<endl;
	}
	else{
		assignTempOffset(this);
		cout<<"\tmovl\t"<<_expr<<", %eax"<<endl;
		cout<<"\tmovl\t"<<"%eax, "<< this<<endl;
	//	mol expr eax
		//movl eax this
	}
}


void If::generate(){
	Label SKIP;
	//Label *C=new Label();
	Label ELSE;
	_expr->generate();
	cout<<"\tcmpl\t"<<"$0, "<<_expr<<endl;
	if(_elseStmt ==nullptr){
		cout<<"\tje\t"<<SKIP<<endl;
		_thenStmt->generate();
		cout<<SKIP<<":"<<endl;
	}
	else{
		cout<<"\tje\t"<<ELSE<<endl;
		_thenStmt->generate();	
		cout<<"\tjmp\t"<<SKIP<<endl;
		cout<<ELSE<<":"<<endl;
		_elseStmt->generate();
		cout<<SKIP<<":"<<endl;
	}
}

void While::generate(){
	//Label *B=new Label();
//	Label *C=new Label();
	Label LOOP;
	Label EXIT;
	cout<<LOOP<<":"<<endl;
	_expr->generate();
	cout<<"\tcmpl\t"<<"$0, "<<_expr<<endl;
	cout<<"\tje\t"<<EXIT<<endl;
	_stmt->generate();
	cout<<"\tjmp\t"<<LOOP<<endl;
	cout<<EXIT<<":"<<endl;
}

void LessThan::generate(){
	_left->generate();
	_right->generate();
	assignTempOffset(this);
	cout<<"\tmovl\t"<<_left<<", %eax"<<endl;
	cout<<"\tcmpl\t%eax, "<<_right<<endl;
	cout<<"\tsetl\t%al"<<endl;
	cout<<"\tmovsbl\t%al, %eax"<<endl;
	cout<<"\tmovl\t%eax, "<<this<<endl;
}

void GreaterThan::generate(){
	_left->generate();
	_right->generate();
	assignTempOffset(this);
	cout<<"\tmovl\t"<<_left<<", %eax"<<endl;
	cout<<"\tcmpl\t%eax, "<<_right<<endl;
	cout<<"\tsetg\t%al"<<endl;
	cout<<"\tmovsbl\t%al, %eax"<<endl;
	cout<<"\tmovl\t%eax, "<<this<<endl;
}

void GreaterOrEqual::generate(){
	_left->generate();
	_right->generate();
	assignTempOffset(this);
	cout<<"\tmovl\t"<<_left<<", %eax"<<endl;
	cout<<"\tcmpl\t%eax, "<<_right<<endl;
	cout<<"\tsetge\t%al"<<endl;
	cout<<"\tmovsbl\t%al, %eax"<<endl;
	cout<<"\tmovl\t%eax, "<<this<<endl;
}

void LessOrEqual::generate(){
	_left->generate();
	_right->generate();
	assignTempOffset(this);
	cout<<"\tmovl\t"<<_left<<", %eax"<<endl;
	cout<<"\tcmpl\t%eax, "<<_right<<endl;
	cout<<"\tsetle\t%al"<<endl;
	cout<<"\tmovsbl\t%al, %eax"<<endl;
	cout<<"\tmovl\t%eax, "<<this<<endl;
}

void Equal::generate(){
	_left->generate();
	_right->generate();
	assignTempOffset(this);
	cout<<"\tmovl\t"<<_left<<", %eax"<<endl;
	cout<<"\tcmpl\t"<<_right<< ", %eax"<<endl;
	cout<<"\tsete\t%al"<<endl;
	cout<<"\tmovsbl\t%al, %eax"<<endl;
	cout<<"\tmovl\t%eax, "<<this<<endl;
}

void NotEqual::generate(){
	_left->generate();
	_right->generate();
	assignTempOffset(this);
	cout<<"\tmovl\t"<<_left<<", %eax"<<endl;
	cout<<"\tcmpl\t"<<_right<<", %eax"<<endl;
	cout<<"\tsetne\t%al"<<endl;
	cout<<"\tmovsbl\t%al, %eax"<<endl;
	cout<<"\tmovl\t%eax, "<<this<<endl;
}
