#include <stdlib.h>
#include <assert.h>
#include <stdio.h> //à retirer

#include "skiplist.h"
#include "rng.h"

typedef struct s_Node Node;

typedef struct s_Link{
	Node * previous;
	Node * next;
} Link;

struct s_Node{
	int val;
	Link * liens;
};

struct s_SkipList{
	Node * sentinelle;
	unsigned int size;
	int nb_levels;
	RNG rng;
};

SkipList* skiplist_create(int nb_levels) {
	SkipList * new_list = malloc(sizeof(SkipList)+sizeof(Node)+(sizeof(Link)*nb_levels));
	new_list->sentinelle=(Node *)(new_list+1);
	new_list->sentinelle->liens=(Link*)(new_list->sentinelle+1);
	for(int i=0;i<nb_levels;i++){
		(new_list->sentinelle->liens[i]).previous=new_list->sentinelle;
		(new_list->sentinelle->liens[i]).next=new_list->sentinelle;
	}
	new_list->size=0;
	new_list->rng=rng_initialize(0, nb_levels);
	new_list->nb_levels=nb_levels;
	return new_list;
}

void skiplist_delete(SkipList** d) {
	Node * parcours=((*d)->sentinelle->liens[0]).next;
	Node * temp;
	while(parcours!=(*d)->sentinelle){
		temp=parcours;
		parcours=(parcours->liens[0]).next;
		free(temp);
	}
	free(*d);
	d=NULL;
}

unsigned int skiplist_size(const SkipList* d){
	return d->size;
}

int skiplist_at(const SkipList* d, unsigned int i){
	assert(i<d->size);
	Node * parcours=(d->sentinelle->liens[0]).next;
	for(unsigned int j=0;j<i;j++) parcours=(parcours->liens[0]).next;
	return parcours->val;
}

void skiplist_map(const SkipList* d, ScanOperator f, void *user_data){
	Node * parcours=(d->sentinelle->liens[0]).next;
	while(parcours!=d->sentinelle){
		f(parcours->val,user_data);
		parcours=(parcours->liens[0]).next;
	}
}

SkipList* skiplist_insert(SkipList* d, int value) {
	int niveau=rng_get_value(&(d->rng));
	Node *new_ele= malloc(sizeof(Node)+(sizeof(Link)*(niveau+1)));
	new_ele->liens=(Link *)(new_ele+1);
	new_ele->val=value;
	Node *ele_act=d->sentinelle;
	int level_act=d->nb_levels-1;
	while(level_act>=0){
		if ((ele_act->liens[level_act]).previous!=d->sentinelle && (ele_act->liens[level_act]).previous->val>=value){
			ele_act=(ele_act->liens[level_act]).previous;
			if(ele_act->val==value){
				free(new_ele);
				return d;
			}
		}
		else{
			if(level_act<=niveau){
				Node * previous=(ele_act->liens[level_act]).previous;
				(new_ele->liens[level_act]).previous=previous;
				(new_ele->liens[level_act]).next=ele_act;
			}
			level_act--;
		}
	}
	for(int i=0;i<niveau+1;i++){
		(((new_ele->liens[i]).next)->liens[i]).previous=new_ele;
		(((new_ele->liens[i]).previous)->liens[i]).next=new_ele;
	}
	d->size++;
	return d;
}


bool skiplist_search(const SkipList* d, int value, unsigned int *nb_operations){
	bool find=false;
	Node *ele_act=d->sentinelle;
	(*nb_operations)++;
	int level_act=d->nb_levels-1;
	while(level_act>=0 && !find){
		if((ele_act->liens[level_act]).previous!=d->sentinelle && (ele_act->liens[level_act]).previous->val>=value){
			(*nb_operations)++;
			ele_act=(ele_act->liens[level_act]).previous;
			if(ele_act->val==value) find=true;
		}
		else level_act--;
	}
	return find;
}