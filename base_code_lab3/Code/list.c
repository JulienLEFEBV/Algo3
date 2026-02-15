#include <stdio.h>

#include <stdlib.h>
#include <assert.h>

#include "list.h"

typedef struct s_LinkedElement {
	int value;
	struct s_LinkedElement* previous;
	struct s_LinkedElement* next;
} LinkedElement;

/* Use of a sentinel for implementing the list :
 The sentinel is a LinkedElement* whose next pointer refer always to the head of the list and previous pointer to the tail of the list
 */
struct s_List {
	LinkedElement* sentinel;
	int size;
};


/*-----------------------------------------------------------------*/

List* list_create(void) {
	List* l = malloc(sizeof(struct s_List)+sizeof(LinkedElement));
	l->sentinel=(LinkedElement *)(l+1);
	l->sentinel->previous=l->sentinel;
	l->sentinel->next=l->sentinel;
	l->size=0;
	return l;
}

/*-----------------------------------------------------------------*/

List* list_push_back(List* l, int v) {
	LinkedElement * nouvEle=malloc(sizeof(LinkedElement));
	nouvEle->value=v;
	nouvEle->previous=l->sentinel->previous;
	nouvEle->previous->next=nouvEle;
	l->sentinel->previous=nouvEle;
	nouvEle->next=l->sentinel;
	l->size++;
	return l;
}

/*-----------------------------------------------------------------*/

void list_delete(ptrList* l) {
	LinkedElement * eleAct=(*l)->sentinel->next;
	LinkedElement * eleTemp;
	while(eleAct!=(*l)->sentinel){
		eleTemp=eleAct->next;
		free(eleAct);
		eleAct=eleTemp;
	}
	free(*l);
	*l=NULL;
}

/*-----------------------------------------------------------------*/

List* list_push_front(List* l, int v) {
	LinkedElement * nouvEle=malloc(sizeof(LinkedElement));
	nouvEle->value=v;
	nouvEle->next=l->sentinel->next;
	nouvEle->next->previous=nouvEle;
	l->sentinel->next=nouvEle;
	nouvEle->previous=l->sentinel;
	l->size++;
	return l;
}

/*-----------------------------------------------------------------*/

int list_front(const List* l) {
	assert(l->size!=0);
	return l->sentinel->next->value;
}

/*-----------------------------------------------------------------*/

int list_back(const List* l) {
	assert(l->size!=0);
	return l->sentinel->previous->value;
}

/*-----------------------------------------------------------------*/

List* list_pop_front(List* l) {
	assert(l->size!=0);
	LinkedElement * eleTemp=l->sentinel->next;
	eleTemp->next->previous=l->sentinel;
	l->sentinel->next=eleTemp->next;
	l->size--;
	free(eleTemp);
	return l;
}

/*-----------------------------------------------------------------*/

List* list_pop_back(List* l){
	assert(l->size!=0);
	LinkedElement * eleTemp=l->sentinel->previous;
	eleTemp->previous->next=l->sentinel;
	l->sentinel->previous=eleTemp->previous;
	l->size--;
	free(eleTemp);
	return l;
}

/*-----------------------------------------------------------------*/

List* list_insert_at(List* l, int p, int v) {
	assert(0<=p && p<=l->size);
	LinkedElement * nouvEle=malloc(sizeof(LinkedElement));
	nouvEle->value=v;
	LinkedElement * eleAct=l->sentinel;
	for(int i=0;i<p;i++) eleAct=eleAct->next;
	nouvEle->previous=eleAct;
	nouvEle->next=eleAct->next;
	eleAct->next=nouvEle;
	nouvEle->next->previous=nouvEle;
	l->size++;
	return l;
}

/*-----------------------------------------------------------------*/

List* list_remove_at(List* l, int p) {
	assert(0<=p && p<l->size);
	LinkedElement * eleTemp=l->sentinel->next;
	for(int i=0;i<p;i++) eleTemp=eleTemp->next;
	eleTemp->previous->next=eleTemp->next;
	eleTemp->next->previous=eleTemp->previous;
	l->size--;
	free(eleTemp);
	return l;
}

/*-----------------------------------------------------------------*/

int list_at(const List* l, int p) {
	assert(0<=p && p<l->size);
	LinkedElement * eleAct=l->sentinel->next;
	for(int i=0;i<p;i++) eleAct=eleAct->next;
	return eleAct->value;
}

/*-----------------------------------------------------------------*/

bool list_is_empty(const List* l) {
	return l->size==0;
}

/*-----------------------------------------------------------------*/

int list_size(const List* l) {
	return l->size;
}

/*-----------------------------------------------------------------*/

List* list_map(List* l, ListFunctor f, void* environment) {
	LinkedElement * eleAct=l->sentinel->next;
	while(eleAct!=l->sentinel){
		eleAct->value=f(eleAct->value,environment);
		eleAct=eleAct->next;
	}
	return l;
}

/*-----------------------------------------------------------------*/

typedef struct s_SubList{
	LinkedElement * head;
	LinkedElement * tail;
} SubList;

void sublist_push_back(SubList * l,LinkedElement * ele){
	ele->previous=l->tail;
	ele->next=NULL;
	if (l->head) l->tail->next=ele;
	else l->head=ele;
	l->tail=ele;
}

SubList list_split(SubList l){
	LinkedElement * eleParcour=l.head;
	LinkedElement * eleAct=l.head;
	int i=0;
	while(eleParcour->next){
		if (i%2==1) eleAct=eleAct->next;
		eleParcour=eleParcour->next;
		i++;
	}
	SubList listMillieu;
	listMillieu.head=eleAct;
	listMillieu.tail=eleAct->next;
	return listMillieu;
}

SubList list_merge(SubList leftlist, SubList rightlist, OrderFunctor f){
	SubList listMelange;
	listMelange.head=NULL;
	listMelange.tail=NULL;
	LinkedElement * eleActLeft=leftlist.head;
	LinkedElement * eleActRight=rightlist.head;
	while (eleActLeft || eleActRight){
		if (!eleActRight || (eleActLeft && f(eleActLeft->value,eleActRight->value))){
			LinkedElement * eleTemp=eleActLeft;
			eleActLeft=eleActLeft->next;
			sublist_push_back(&listMelange,eleTemp);
		}
		else{
			LinkedElement * eleTemp=eleActRight;
			eleActRight=eleActRight->next;
			sublist_push_back(&listMelange,eleTemp);
		}
	}
	return listMelange;
}

SubList list_mergesort(SubList l, OrderFunctor f){
	if (!l.head){
		return l;
	} 
	else if (l.head==l.tail) {
		return l;
	} 
	else {
		SubList listMillieu=list_split(l),l2;
		l2.tail=l.tail;
		l.tail=listMillieu.head;
		(l.tail)->next=NULL;
		l2.head=listMillieu.tail;
		(l2.head)->previous=NULL;
		return list_merge(list_mergesort(l,f),list_mergesort(l2,f),f);
	}
}

List* list_sort(List* l, OrderFunctor f){
	SubList subL;
	subL.head=l->sentinel->next;
	(subL.head)->previous=NULL;
	subL.tail=l->sentinel->previous;
	(subL.tail)->next=NULL;
	subL=list_mergesort(subL,f);
	l->sentinel->next=subL.head;
	(subL.head)->previous=l->sentinel;
	l->sentinel->previous=subL.tail;
	(subL.tail)->next=l->sentinel;
	return l;
}

