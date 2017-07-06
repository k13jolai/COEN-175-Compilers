#include <iostream>
#ifndef LIST_H
#define LIST_H

using namespace std;

class List {
public:
	List();
	~List();
	List(const List&);
	const List& operator=(const List&); // Function: Assignment operator
	friend ostream& operator<<(ostream&, const List&); //Function: output operator
	void insert(const int&);
	void replace(int index, int new_int);
	class node {

public:
	node(int, node*);
	int data;
	node* next;
	};

 protected:
	node* list;
	int size; 
	node* cursor;
};

#endif