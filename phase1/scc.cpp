//Jordan Lai COEN 175 Phase 1: Lexical Analysis
//This code serves as a lexical analyzer for the simple c language. C files are read from the 
//file stream and broken down into numbers, strings, characters, operators, keywords, and identifiers.
#include<cctype>
#include<cstdio>
#include<iostream>
#include<string>
using namespace std;

int main(){
	int lex;
	int key=0;
	int flag=0;
	string lstring="";
	lex=cin.get();
	string keywords[32];
		keywords[0]="auto"; keywords[1]="break"; keywords[2]="case"; keywords[3]="char";
		keywords[4]="const"; keywords[5]="continue"; keywords[6]="default";	keywords[7]="do";
		keywords[8]="double";	keywords[9]="else";	keywords[10]="enum";	keywords[11]="extern";
		keywords[12]="float";	keywords[13]="for";	keywords[14]="goto";	keywords[15]="if";
		keywords[16]="int";	keywords[17]="long";	keywords[18]="register";	keywords[19]="return";
		keywords[20]="short";	keywords[21]="signed";	keywords[22]="sizeof"; keywords[23]="static";
		keywords[24]="struct";	keywords[25]="switch";	keywords[26]="typedef";
		keywords[27]="union"; keywords[28]="unsigned";
		keywords[29]="void";	keywords[30]="volatile";	keywords[31]="while";	
//If the character read into lex is a digit, it is saved into lstring and the whole number is later
//outputted.		
	while(lex!=EOF){
		if(isdigit(lex)){
			lstring.clear();
			do{
				lstring+=lex;
				lex=cin.get();
			}while(isdigit(lex));
		cout << "number:" << lstring << endl;
		}
//If the character is an underscore or letter, it is saved into lstring and analyzed/output as either
//a keyword or identifier.		
		else if(lex == '_' || isalpha(lex)){
			lstring.clear();
			key=0;
			do{
				lstring+=lex;
				lex=cin.get();
			}while(lex== '_' || isalpha(lex) || isdigit(lex));
			
			
			do
			{
				if(key<32){
					if(lstring.compare(keywords[key]) == 0){
						cout << "keyword:" << lstring << endl;
						break;
						key++;
					}

					else 
					key++;
				}

				else if(key==32){
					cout << "identifier:" << lstring << endl;
					key++;
				}
				
			}while(key!=33);
		}	
//If the character is a double quote, the entire string is saved in and output from lstring.
		else if(lex== '\"'){
			lstring.clear();
			lstring+=lex;
			lex=cin.get();
			do{
				if(lex=='\\'){
					lstring+=lex;
					lex=cin.get();
					if(lex=='\n'){
						cout << "Error" << endl;
						lex=cin.get();
						break;			
					}

					else{
						lstring+=lex;
						lex=cin.get();
					}
				}
				else{
					lstring+=lex;
					lex=cin.get();
				}
			}while(lex!='\"' && lex!='\n');

			lstring+=lex;
			lex=cin.get();
			cout << "string:" << lstring <<endl;	
		}
//If the character is a single quote, the entire character is saved in and output from lstring.
		else if(lex=='\''){		
			lstring.clear();
			lstring+=lex;
			lex=cin.get();
			do{
				if(lex=='\\'){
					lstring+=lex;
					lex=cin.get();
					if(lex=='\n'){
						cout << "Error" << endl;
						lex=cin.get();
						break;			
					}
				
					else{
						lstring+=lex;
						lex=cin.get();
					}
				}

				else{
					lstring+=lex;
					lex=cin.get();
				}
			}while(lex!='\'' && lex!='\n');
			lstring+=lex;
			lex=cin.get();
			cout << "character:" << lstring <<endl;
		}
//If the character is none of the above, it is analyzed/output as an operator, or ignored (comment).
		else if(!isdigit(lex) && !isalpha(lex) && lex!='_' && lex!='\'' && lex!='\"' && !isspace(lex)){
			lstring.clear();
			flag=0;
			if(lex=='/'){
				lex=cin.get();
				if(lex=='*'){
					lex=cin.get();
					do{
						if(lex=='*'){
							lex=cin.get();
							if(lex=='/'){
								lex=cin.get();
								flag=1;	
							}
						}	
						else
							lex=cin.get();
									
						
					}while(flag == 0 && lex!=EOF);
					flag=0;
				}

				else{
					cin.putback(lex);
					cout << "Operator:/" <<endl;
				}
			}
			do{
				if(lex=='='){
					lex=cin.get();
					if(lex=='=')
						cout << "Operator:==" << endl;
					else{
						cin.putback(lex);
						cout << "Operator:=" << endl;
					}
				}		
				else if(lex=='|'){
					lex=cin.get();
					cout << "Operator:||" << endl;
				}	
				else if(lex=='&'){
					lex=cin.get();
					if(lex=='&')
						cout << "Operator:&&" << endl;
					else{
						cin.putback(lex);
						cout << "Operator:&" << endl;
					}
				}	
				else if(lex=='.')
					cout << "Operator:." << endl;
				else if(lex==':')
					cout << "Operator::" << endl;
				else if(lex==';')
					cout << "Operator:;" << endl;	
				else if(lex==',')
					cout << "Operator:," << endl;
				else if(lex=='{')
					cout << "Operator:{" << endl;
				else if(lex=='}')
					cout << "Operator:}" << endl;
				else if(lex=='[')
					cout << "Operator:[" << endl;
				else if(lex==']')
					cout << "Operator:]" << endl;
				else if(lex=='(')
					cout << "Operator:(" << endl;
				else if(lex==')')
					cout << "Operator:)" << endl;
				else if(lex=='%')
					cout << "Operator:%" << endl;
				else if(lex=='*')
					cout << "Operator:*" << endl;
				else if(lex=='!'){
					lex=cin.get();
					if(lex=='=')
						cout << "Operator:!=" << endl;
					else{
						cin.putback(lex);
						cout << "Operator:!" <<endl;
					}
				}
				else if(lex=='-'){
					lex=cin.get();
					if(lex=='>')
						cout << "Operator:->" <<endl;
					else if(lex=='-')
						cout << "Operator:--" <<endl;
					else{
						cin.putback(lex);
						cout << "Operator-" <<endl;
					}
				}

				else if(lex=='+'){
					lex=cin.get();
					if(lex=='+')
						cout << "Operator:++" <<endl;
					else{
						cin.putback(lex);
						cout << "Operator:+" <<endl;
					}
				}
				else if(lex=='<'){
					lex=cin.get();
					if(lex=='=')
						cout << "Operator:<=" <<endl;
					else{
						cin.putback(lex);
						cout << "Operator:<" <<endl;
					}
				}
				else if(lex=='>'){
					lex=cin.get();
					if(lex=='=')
						cout << "Operator:>=" <<endl;
					else{
						cin.putback(lex);
						cout << "Operator:>" <<endl;
					}
				}
				lex=cin.get();
			}while(!isdigit(lex) && !isalpha(lex) && lex!='_' && lex!='\'' && lex!='\"' && !isspace(lex));
		}
//If the input is illgeal, it is ignored entirely and the next character is taken from the file. 		
		else
		lex=cin.get();	
	}

}
