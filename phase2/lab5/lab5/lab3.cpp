#include <iostream>
#include "list.h"
using namespace std;


/*==========Constructor& Destructor==========*/
List::List()
{
	size=0;
	list=NULL;
	cursor=list;
}
List::~List()
{
	if(list!=NULL && cursor!=NULL)
	{
		node* temp;
		temp=list->next;
		while(list!=cursor)
		{
			delete list; 
			list=temp;
			temp=list->next;
		}
		delete cursor;
	}
}

List::node::node (int num, node* p)
{
	data=num;
	next=p;
}
/*==========Function==========*/
void List::insert(const int& num)
{
	node* input=new List::node(num, NULL);
	if (size==0)
	{
		list=input;
		cursor=list;
		size++;
		return;
	}
	else
	{
		cursor->next=input;
		cursor=input;
		size++;
	}
}
void List::replace(int index, int new_int)
{
    node* temp1=list;
    //node* temp= list->next;
    int i;
    for(i=1; i<index; i++)
    {
        
        temp1=temp1->next;
        
    }
    temp1->data=new_int;


}
List::List(const List& copy)
{
	list=NULL;
	size=0;
	cursor=list;
	node* temp;
	temp=copy.list;
	while(temp!=NULL)
	{
		insert(temp->data);
		temp=temp->next;
	}
	size=copy.size;
}
const List& List::operator=(const List& copy)
{
	if(this->list!=NULL){
		this->~List();}
	this->cursor=NULL;
	this->list=NULL;
	 size=0;
	list=NULL;
        cursor=list;
        node* temp;
        temp=copy.list;
        while(temp!=NULL)
        {
                insert(temp->data);
                temp=temp->next;
        }
        size=copy.size;
	return *this;
}
 ostream& operator<<(ostream& out, const List& p)
{
	List::node* temp;
	temp = p.list;
	while(temp!=NULL)
	{
		out<< temp->data<<",";
		temp=temp->next;
	}
	return out;
}




