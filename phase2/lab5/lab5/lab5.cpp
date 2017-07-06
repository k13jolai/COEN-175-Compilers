#include"lab3.cpp"
#include"list.h"
#include<iostream>

class OrderedList: public List{

	public:
		void insert(int x);
		void replace(int index, int new_int);



};

void OrderedList::insert(int x){
	node* temp= list;
	node* temp1= list;
	node* input=new List::node(x,NULL);
	if(size==0)
	{
		list=input;
		cursor=list;
		size++;
		return;
	}
	if(x<temp->data){
		input->next=temp;
		list=input;
		size++;
	}

	else{
		while(temp!=NULL){
	
			if(x>temp->data){
				if(temp->next==NULL)
				{	
					temp->next=input;
					size++;
					return;
				}
				input->next=temp->next;
				temp->next=input;
				size++;
				return;
			
			}
			else
				temp++;
		}	
	}
}

void OrderedList::replace(int index, int new_int){
	 node* temp1=list;
	 node* temp= list->next;
	 int i;
     
	 

	 for(i=1; i<index; i++)
	 {                   
		 temp1=temp1->next;	                                    
	 }
	 temp=temp1--;
	 temp->next=temp1->next;
	 delete temp;

	 insert(new_int);
	//temp1->data=new_int;
	//temp->data=temp->next->data;	                                            
	//temp->next->data=new_int;
	return;
}

int main(){

	List x;
	OrderedList y;

	x.insert(5);
	x.insert(1);
	x.insert(3);

	cout << x << endl; //should print 5, 1, 3,
	x.replace(2, 6);
	cout << x << endl; //should print 5, 6, 3,
	x.insert(2);
	cout << x << endl; //should print 5, 6, 3, 2,

	y.insert(5);
	y.insert(1);
	y.insert(3);
	cout << y << endl; //should print 1, 3, 5,

	y.replace(2, 6);
	cout << y << endl; //should print 1, 5, 6,
	y.insert(2);
	cout << y << endl; //should print 1, 2, 5, 6,

	return 1;}
