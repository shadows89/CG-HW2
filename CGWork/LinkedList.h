#ifndef LINKEDLIST_H
#define LINKEDLIST_H

template<class T>
class LinkedList{

public:
	class Node{
	public:
		Node* next;
		T data;

		Node(T new_data){
			data = new_data;
			next = NULL;
		}
		~Node(){
			delete data;
		}
	};

	Node* head;
	Node* curr;
	int size;

public:
	LinkedList(){
		head = NULL;
		curr = NULL;
		size = 0;
	}

	~LinkedList(){
		Node* tmp = head;
		Node* next;
		if (tmp == NULL)
			return;
		while (tmp != NULL){
			next = tmp->next;
			delete tmp;
			tmp = next;
		}
	}

	void clear(){
		Node* tmp = head;
		Node* next;
		if (tmp == NULL)
			return;
		while (tmp != NULL){
			next = tmp->next;
			delete tmp;
			tmp = next;
		}
		head = NULL;
		curr = NULL;
	}

	Node* add(T new_data){
		Node* tmp;
		Node* newNode = new Node(new_data);
		if (head == NULL){
			head = newNode;
		}
		else{
			tmp = head;
			while (tmp->next != NULL)
				tmp = tmp->next;
			tmp->next = newNode;
		}
		size++;
		return newNode;
	}

	T first(){
		curr = head;
		return head->data;
	}

	T next(){
		if (curr == NULL)
			return NULL;
		Node* tmp = curr;
		curr = curr->next;
		return tmp->data;
	}

	int getSize(){
		return size;
	}
};

#endif