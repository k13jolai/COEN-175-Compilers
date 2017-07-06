//This header file defines the classes TYPE, SYMBOL, and SCOPE. Furthermore, it initializes the stack of scopes
//called SYMTABLE
#include <stack>
using namespace std;
int stackcount;
//Stack count will be used as a method of keeping track how many scopes are present in the stack.
class TYPE{
	public:
		string spec;
		int indirection;
		string kind;
		int arraysize;

		TYPE();
		TYPE(string s, int i, string k, int h);
		bool operator==(const TYPE& B){
			if(spec.compare(B.spec)!=0){
				return false;
			}
			if(indirection!=B.indirection){
				return false;
			}
			if(kind.compare(B.kind)!=0){
				return false;
			}
			if(kind=="ARRAY" && arraysize!=B.arraysize){
				return false;
			}	
			return true;
		}

		bool operator!=(const TYPE& B){
			return !operator==(B);
		}
};

TYPE::TYPE(){}
TYPE::TYPE(string s, int i, string k, int h){
	spec=s;
	indirection=i;
	kind=k;
	arraysize=h;
}


class SYMBOL{
	public:
		string name;
		TYPE stype;
		int defined;
		//The defined variable is a flag that tells us whether or not a function has been defined yet. We will also
		//use this to tell us whether or not a STRUCT is complete or not. 
		SYMBOL(string aye, TYPE T);
		bool compare(const SYMBOL& B){
			if(stype==B.stype && name.compare(B.name)==0)
				return true;
			else
				return false;
		}
};

SYMBOL::SYMBOL(string aye, TYPE T){
	name=aye;
	stype=T;
	defined=0;
}

class SCOPE{
	public:
		SYMBOL *TABLE[100];
		SCOPE *enclosing;
		int tablesize;
		SCOPE();
		SYMBOL* FIND(const string& newname){
			int i;
			for(i=0;i<tablesize;i++){
				if(TABLE[tablesize]->name.compare(newname)==0)
					return TABLE[tablesize];	
			}
			return NULL;
		}
		void insert(SYMBOL *CHOSEN){	
			TABLE[tablesize]=CHOSEN;
			tablesize++;
		}

		SYMBOL* LOOKUP(const string& newname){
			SCOPE* temp=enclosing;
			SYMBOL* Chosen=NULL;
			while(temp!=NULL){
				Chosen=temp->FIND(newname);
				if(Chosen!=NULL){
					return Chosen;
				}
				temp=temp->enclosing;
			}
			return NULL;
		}
};

SCOPE::SCOPE(){
	tablesize=0;
	enclosing=NULL;
}



