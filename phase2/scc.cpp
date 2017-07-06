#include "lexer.cpp"
void match(int t);
void matchEXPRESSIONL();
void matchEXPRESSION();
void matchAND();
void matchEQUIVALENCE();
void matchCOMPARE();
void matchADDSUB();
void matchMULDIV();
void matchCAST();
void matchADDREF();
void matchARROWS();
void matchIDS();

void matchSTMT();
void matchSTMTS();
void matchDCLT();
void matchDCLTL();

void matchDECLTN();
void matchDECLTNS();
void matchPAR();
void matchPARL();
void matchPARS();

void matchPOINTER();
void matchSPECIFIER();
bool NEXTISSPECIFIER();
bool ISSPECIFIER();

void matchFDEF();
void matchGDCLT();
void matchGDCLTL();
void matchGDECLTN();
void matchTYPEDEF();
void matchTRAN();

string lexbuf;
int look=lexan(lexbuf);
int ahead=lexan(lexbuf);
int main(){

	matchTRAN();

	return 0;
}

void match(int t){
//	cout << look << ahead << endl;
	if(look==t){
		look=ahead;
		ahead=lexan(lexbuf);
	}
	
	else{ 
		cout << "error" << endl;
		cout << "stopped at" << look << "t is" << t  << endl; 
	}
	
}

void matchEXPRESSIONL(){
	matchEXPRESSION();
	while(look== COMMA){
		match(COMMA);
		matchEXPRESSION();
	}
}
void matchEXPRESSION(){
	matchAND();
	while(look == OR){
		match(look);
		matchAND();
		cout << "or" << endl;
	}
}

void matchAND(){
	matchEQUIVALENCE();
	while(look == AND){
		match(look);
		matchEQUIVALENCE();
		cout << "and" << endl;
	}
}

void matchEQUIVALENCE(){
	matchCOMPARE();
	while(look == EQV || look == NEQV){
		if(look == EQV){
			match(look);
			matchCOMPARE();
			cout << "eql" << endl;
		}
		else{
			match(look);
			matchCOMPARE();
			cout << "neq" << endl;
		}
	}
}
void matchCOMPARE(){
	matchADDSUB();
	while(look== LESS || look == GREATER || look == LESSEQ || look == GREATEQ){
		if(look == LESS){
			match(look);
			matchADDSUB();
			cout << "ltn" << endl;
		}
		else if(look == GREATER){
			match(look);
			matchADDSUB();
			cout << "gtn" << endl;
		}		
		else if(look == LESSEQ){	
			match(look);
			matchADDSUB();
			cout << "leq" << endl;
		}
		else if(look == GREATEQ){
			match(look);
			matchADDSUB();
			cout << "geq" << endl;
		}
	}
}

void matchADDSUB(){
	matchMULDIV();
	while(look== ADD || look == SUB){
		if(look == ADD){
			match(look);
			matchMULDIV();
			cout << "add" << endl;
		}

		else if(look == SUB){
			match(look);
			matchMULDIV();
			cout << "sub" << endl;
		}
	}
}
			
void matchMULDIV(){
	matchCAST();
	while(look == MUL || look == DIV || look == MOD){
		if(look == MUL){
			match(look);
			matchCAST();
			cout << "mul" << endl;
		}
		else if(look == DIV){
			match(look);
			matchCAST();
			cout << "div" << endl;
		}
		else if(look == MOD){
			match(look);
			matchCAST();
			cout << "rem" << endl;
		}
	}
}

void matchCAST(){
	if(look==LPAREN && NEXTISSPECIFIER()==true){
		match(LPAREN);
		matchSPECIFIER();
		matchPOINTER();
		match(RPAREN);
		matchCAST();
		cout << "cast" << endl;
	}

	else
	matchADDREF();
}

void matchADDREF(){
	if(look==AMP || look == MUL || look == EXC || look == SUB || look == SIZEOF){

	   if(look == SIZEOF && ahead ==LPAREN){
		   match(look);
		   matchCAST();
		   cout << "sizeof" << endl;
	   }
	   if(look==AMP){
		   match(look);
		   matchADDREF();
		   cout << "adr" << endl;
	   }
	   else  if(look==MUL){
		   match(look);
		   matchADDREF();
		   cout << "deref" << endl;
	   }
	   else if(look==EXC){
		   match(look);
		   matchADDREF();
		   cout << "not" << endl;
	   }
	   else  if(look==SUB){
		   match(look);
		   matchADDREF();
		   cout << "neg" << endl;
	   }
	   else if(look==SIZEOF){
		   match(look);
		   matchADDREF();
		   cout << "sizeof" << endl;
	   }	   
	}	

	else
		matchARROWS();	

}

void matchARROWS(){
	matchIDS();
	while(look== LBRACE || look== PERIOD || look == ARROW){
		if(look==LBRACE){
			match(LBRACE);
			matchEXPRESSION();
			match(RBRACE);
			cout << "index" << endl;
		}

		else if(look == PERIOD){
			match(look);
			match(ID);
			cout << "dot" << endl;
		}
		else if(look == ARROW){
			match(look);
			match(ID);
			cout << "arrow" << endl;
		}
	}
}
void matchIDS(){
	if(look == ID){
		match(ID);
		if(look==LPAREN){
			match(LPAREN);
			if(look==RPAREN)
				match(RPAREN);
			else{
				matchEXPRESSIONL();
				match(RPAREN);
			}
		}
	}
	
	else if(look== LPAREN){
		match(LPAREN);
		matchEXPRESSION();
		match(RPAREN);
	}
	else if(look == NUMBER || look == STRING || look== CHARACTER){
		match(look);
	}
}

void matchSTMT(){
	if(look== LBRACKET){
		match(LBRACKET);
		matchDECLTNS();
		matchSTMTS();
		match(RBRACKET);
	}
	else if(look==RETURN){
		match(RETURN);
		matchEXPRESSION();
		match(SEMI);
	}
	else if(look==WHILE){
		match(WHILE);
		match(LPAREN);
		matchEXPRESSION();
		match(RPAREN);
		matchSTMT();
	}
	else if(look==IF){
		match(IF);
		match(LPAREN);
		matchEXPRESSION();
		match(RPAREN);
		matchSTMT();
		while(look==ELSE){
			match(ELSE);
			matchSTMT();
		}
	}
	else{
		matchEXPRESSION();
		if(look == EQL){
			match(EQL);
			matchEXPRESSION();
			match(SEMI);
		}
		else{
			match(SEMI);
		}
	}
}

void matchSTMTS(){
	while(look!= RBRACKET)
	matchSTMT();
}
void matchDCLT(){
	matchPOINTER();
	match(ID);
	if(look==LBRACE){
		match(LBRACE);
		match(NUMBER);
		match(RBRACE);
	}
}
void matchDCLTL(){
	matchDCLT();
	while(look == COMMA){
		match(COMMA);
		matchDCLT();
	}
}

void matchDECLTN(){
	matchSPECIFIER();
	matchDCLTL();
	match(SEMI);
}

void matchDECLTNS(){
	while(ISSPECIFIER()==true)
		matchDECLTN();
}
void matchPAR(){
	matchSPECIFIER();
	matchPOINTER();
	match(ID);
}

void matchPARL(){
	matchPAR();
	while(look == COMMA){
		match(COMMA);
		matchPAR();
	}
}
void matchPARS(){
	if(look == VOID)
		match(VOID);
	else
		matchPARL();
}
void matchPOINTER(){
	while(look==MUL)
	match(MUL);
}
void matchSPECIFIER(){
	if(look == INT || look == CHAR || look == STRUCT){
		if(look == INT)
			match(INT);
		else if(look == CHAR)
			match(CHAR);
		else if(look == STRUCT){
			match(STRUCT);
			match(ID);
		}
	}
}

bool ISSPECIFIER(){
	if(look == INT || look == CHAR || look == STRUCT){
		if(look == STRUCT && ahead == ID){
			return true;
		}
		return true;
	}
	else
		return false;
}

bool NEXTISSPECIFIER(){
	if(ahead == INT || ahead == CHAR || ahead == STRUCT)
		return true;

	else
		return false;
}



void matchFDEF(){
	matchPARS();
	match(RPAREN);
	match(LBRACKET);
	matchDECLTNS();
	matchSTMTS();
	match(RBRACKET);
}
void matchGDCLT(){
	matchPOINTER();
	match(ID);
	if(look==LPAREN){
		match(LPAREN);
		match(RPAREN);
	}
	else if(look==LBRACE){
		match(LBRACE);
		match(NUMBER);
		match(RBRACE);
	}
}
void matchGDCLTL(){
	while(look== COMMA){
		match(COMMA);
		matchGDCLT();
	}
	match(SEMI);
}

void matchTYPEDEF(){
	matchSPECIFIER();
	match(LBRACKET);
	matchDECLTN();
	matchDECLTNS();
	match(RBRACKET);
	match(SEMI);
}
void matchTRAN(){
	while(look!=END){
		if(look==STRUCT && ahead == ID){
			matchTYPEDEF();
		}

		else{
			matchSPECIFIER();
			matchPOINTER();
			match(ID);
			if(look==LPAREN){
				if(ahead ==RPAREN){
					match(LPAREN);
					match(RPAREN);
					matchGDCLTL();
				}

				else{
					match(LPAREN);
					matchFDEF();
				}
			}	
		}
	}
}
