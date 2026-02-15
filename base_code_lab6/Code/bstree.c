#include "bstree.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"


/*------------------------  BSTreeType  -----------------------------*/

typedef enum {red, black} NodeColor;

struct _bstree {
    BinarySearchTree* parent;
    BinarySearchTree* left;
    BinarySearchTree* right;
    int key;
    NodeColor color;
};

/*------------------------  BaseBSTree  -----------------------------*/

BinarySearchTree* bstree_create(void) {
    return NULL;
}

/* This constructor is private so that we can maintain the oredring invariant on
 * nodes. The only way to add nodes to the tree is with the bstree_add function
 * that ensures the invariant.
 */
BinarySearchTree* bstree_cons(BinarySearchTree* left, BinarySearchTree* right, int key) {
    BinarySearchTree* t = malloc(sizeof(struct _bstree));
    t->parent = NULL;
    t->left = left;
    t->right = right;
    if (t->left != NULL)
        t->left->parent = t;
    if (t->right != NULL)
        t->right->parent = t;
    t->key = key;
    t->color=red;
    return t;
}

void freenode(const BinarySearchTree* t, void* n) {
    (void)n;
    free((BinarySearchTree*)t);
}

void bstree_delete(ptrBinarySearchTree* t) {
    bstree_depth_postfix(*t, freenode, NULL);
    *t=NULL;
}

bool bstree_empty(const BinarySearchTree* t) {
    return t == NULL;
}

int bstree_key(const BinarySearchTree* t) {
    assert(!bstree_empty(t));
    return t->key;
}

BinarySearchTree* bstree_left(const BinarySearchTree* t) {
    assert(!bstree_empty(t));
    return t->left;
}

BinarySearchTree* bstree_right(const BinarySearchTree* t) {
    assert(!bstree_empty(t));
    return t->right;
}

BinarySearchTree* bstree_parent(const BinarySearchTree* t) {
    assert(!bstree_empty(t));
    return t->parent;
}

/*------------------------  BSTreeDictionary  -----------------------------*/

void leftrotate(BinarySearchTree *x){
    BinarySearchTree * y=x->right;
    x->right=y->left;
    if(x->right) x->right->parent=x;
    y->left=x;
    y->parent=x->parent;
    if(x->parent){
        if(x->parent->left==x) y->parent->left=y;
        if(x->parent->right==x) y->parent->right=y;
    }
    x->parent=y;
}

void rightrotate(BinarySearchTree *y){
    printf("y = %d\n",y->key);
    BinarySearchTree * x=y->left;
    y->left=x->right;
    if(y->left) y->left->parent=y;
    x->right=y;
    x->parent=y->parent;
    printf("me\n");
    if(y->parent){
        if(y->parent->left==y) x->parent->left=x;
        if(y->parent->right==y) x->parent->right=x;
    }
    y->parent=x;
}

void testrotateleft(BinarySearchTree *t) {
    leftrotate(t);
}
void testrotateright(BinarySearchTree *t) {
    rightrotate(t);
}

BinarySearchTree* grandparent(BinarySearchTree* n){
    if(n->parent) return n->parent->parent;
    else return NULL;
}

BinarySearchTree* uncle(BinarySearchTree* n){
    BinarySearchTree * gp=grandparent(n);
    if(gp){
        if(n->parent==gp->right) return gp->left;
        else return gp->right;
    }
    return NULL;
}

BinarySearchTree* fixredblack_insert_case1(BinarySearchTree* x);

BinarySearchTree* fixredblack_insert(BinarySearchTree* x){
    if(!x->parent) x->color=black;
    else if(x->parent->color==red) return fixredblack_insert_case1(x);
    return x;
}

BinarySearchTree* fixredblack_insert_case2(BinarySearchTree* x);

BinarySearchTree* fixredblack_insert_case1(BinarySearchTree* x){
    BinarySearchTree * uncleX=uncle(x);
    printf("x=%d\n",x->key);
    if(!uncleX || uncleX->color==red){
        BinarySearchTree * gpX=grandparent(x);
        printf("grandparent %d\n",gpX->key);
        gpX->color=red;
        x->color=black;
        if(uncleX) uncleX->color=black;
        return fixredblack_insert(gpX);
    }
    printf("no uncle\n");
    return fixredblack_insert_case2(x);
}

BinarySearchTree* fixredblack_insert_case2_left(BinarySearchTree* x);
BinarySearchTree* fixredblack_insert_case2_right(BinarySearchTree* x);

BinarySearchTree* fixredblack_insert_case2(BinarySearchTree* x){
    if(x->parent->left==x) return fixredblack_insert_case2_left(x);
    else return fixredblack_insert_case2_right(x);
}

BinarySearchTree* fixredblack_insert_case2_left(BinarySearchTree* x){
    BinarySearchTree * gpX=grandparent(x);
    rightrotate(gpX);
    gpX->color=red;
    x->parent->color=black;
    return x->parent;
}

BinarySearchTree* fixredblack_insert_case2_right(BinarySearchTree* x){
    BinarySearchTree * parentX=x->parent;
    leftrotate(parentX);
    return fixredblack_insert_case2_left(parentX);    
}


/* Obligation de passer l'arbre par référence pour pouvoir le modifier */
void bstree_add(ptrBinarySearchTree* t, int v) {
	BinarySearchTree * new=bstree_cons(NULL,NULL,v);
    BinarySearchTree * act=*t;
    BinarySearchTree * previous=NULL;
    while(act!=NULL){
        if(v<act->key){
            previous=act;
            act=act->left;
        }
        else{
            previous=act;
            act=act->right;
        }
    }
    if(previous){
        if(v<previous->key) previous->left=new;
        else previous->right=new;
    }
    else *t=new;
    new->parent=previous;
    /* fix colors after insertion */
    BinarySearchTree *stop = fixredblack_insert(new);
    /* stop is the node at which the coloration procedure terminates .
    It may be the new root of the tree . */
    if (stop->parent==NULL)
        *t=stop;
}

const BinarySearchTree* bstree_search(const BinarySearchTree* t, int v) {
    BinarySearchTree *act;
    if(t){
        if(v<t->key)act=t->left;
        else if(v>t->key)act=t->right;
        else return t;
        while(act && act->key!=v){
            if(v<act->key)act=act->left;
            else act=act->right;
        }
        return act;
    }
    return NULL;
}

typedef BinarySearchTree * (*AccessFunction)(const BinarySearchTree *);

typedef struct s_ChildAccessors{
    AccessFunction func1;
    AccessFunction func2;
} ChildAccessors;

BinarySearchTree* find_next(const BinarySearchTree* x, ChildAccessors access){
    assert(!bstree_empty(x));
    BinarySearchTree * result;
    if(access.func2(x)){
        result=access.func2(x);
        while(access.func1(result)){
            result=access.func1(result);
        } 
    }
    else{
        result=x->parent;
        if(!(x->parent) || x==access.func2((x->parent))){
            while(result && (!(result->parent) || result==access.func2((result->parent)))) result=result->parent;
            if(result) result=result->parent;
        }
    }
    return result;
}

const BinarySearchTree* bstree_successor(const BinarySearchTree* x) {
    assert(!bstree_empty(x));
    ChildAccessors accessors;
    accessors.func1=bstree_left;
    accessors.func2=bstree_right;
    return find_next(x,accessors);
    /*
    BinarySearchTree * successor;
    if(x->right){
        successor=x->right;
        while(successor->left) successor=successor->left;
    }
    else{
        successor=x->parent;
        if(!(x->parent) || x==(x->parent)->right){
            while(successor && (!(successor->parent) || successor==(successor->parent)->right)) successor=successor->parent;
            if(successor) successor=successor->parent;
        }
    }
    return successor;
    */
}

const BinarySearchTree* bstree_predecessor(const BinarySearchTree* x) {
    assert(!bstree_empty(x));
    ChildAccessors accessors;
    accessors.func1=bstree_right;
    accessors.func2=bstree_left;
    return find_next(x,accessors);
    /*
    BinarySearchTree * predecessor;
    if(x->left){
        predecessor=x->left;
        while(predecessor->right) predecessor=predecessor->right;
    }
    else{
        predecessor=x->parent;
        if(!(x->parent) || x==(x->parent)->left){
            while(predecessor && (!(predecessor->parent) || predecessor==(predecessor->parent)->left)) predecessor=predecessor->parent;
            if(predecessor) predecessor=predecessor->parent;
        }
    }
    return predecessor;
    */
}

void bstree_swap_nodes(ptrBinarySearchTree* tree, ptrBinarySearchTree from, ptrBinarySearchTree to) {
    assert(!bstree_empty(*tree) && !bstree_empty(from) && !bstree_empty(to));
    BinarySearchTree * parentFrom=from->parent;
    BinarySearchTree * parentTo=to->parent;
    BinarySearchTree * temp;
    //Modification de la racine de l'abre si necessaire
    if(from==*tree) *tree=to;
    else if(to==*tree) *tree=from;
    //Modification des liens des parents
    if(parentTo){
        if(parentTo->left==to) parentTo->left=from;
        else parentTo->right=from;
    }
    if(parentFrom){
        if(parentFrom && parentFrom->left==from) parentFrom->left=to;
        else parentFrom->right=to;
    }
    //Echange des fils gauche et droit et du parent
    temp=from->left;
    from->left=to->left;
    to->left=temp;
    temp=from->right;
    from->right=to->right;
    to->right=temp;
    temp=from->parent;
    from->parent=to->parent;
    to->parent=temp;
    //Mise à jour du parent des fils gauche et droit
    if(to->left) to->left->parent=to;
    if(to->right) to->right->parent=to;
    if(from->left) from->left->parent=from;
    if(from->right) from->right->parent=from;
}

// t -> the tree to remove from, current -> the node to remove
void bstree_remove_node(ptrBinarySearchTree* t, ptrBinarySearchTree current) {
    assert(!bstree_empty(*t) && !bstree_empty(current));
    ptrBinarySearchTree *parent;
    if(!current->parent) parent=t;
    else if(current->parent->left==current) parent=&(current->parent->left);
    else parent=&(current->parent->right);
    if(current->left && current->right){
        const BinarySearchTree *succesorTemp=bstree_successor(current);
        BinarySearchTree*succesor;
        if(succesorTemp==*t)succesor=*t;
        else{
            if(succesorTemp->parent->left==succesorTemp) succesor=succesorTemp->parent->left;
            else succesor=succesorTemp->parent->right;
        }
        //printf("j'échange %d et %d\n",current->key,succesor->key);
        bstree_swap_nodes(t,current,succesor);
        if(!current->parent) parent=t;
        else if(current->parent->left==current) parent=&(current->parent->left);
        else parent=&(current->parent->right);
    }
    if(current->left){
        *parent=current->left;
        current->left->parent=current->parent;
    }
    else{
        *parent=current->right;
        if(current->right)current->right->parent=current->parent;
    }
    free(current);
}

void bstree_remove(ptrBinarySearchTree* t, int v) {
    const BinarySearchTree *nodeTemp=bstree_search(*t,v);
    if(nodeTemp){
        //printf("je supprime %d, je dois supprimer %d\n",nodeTemp->key,v);
        BinarySearchTree *node;
        if(nodeTemp==*t)node=*t;
        else {
            //printf("mon parent est %d\n",nodeTemp->parent->key);
            if(nodeTemp->parent->left==nodeTemp) node=nodeTemp->parent->left;
            else node=nodeTemp->parent->right;
        }
        //printf("je commence à supprimer %d\n",node->key);
        bstree_remove_node(t,node);
    }
    //printf("je ne supprime pas\n");
}

/*------------------------  BSTreeVisitors  -----------------------------*/

void bstree_depth_prefix(const BinarySearchTree* t, OperateFunctor f, void* environment) {
    if(t){
        f(t,environment);
        bstree_depth_prefix(t->left,f,environment);
        bstree_depth_prefix(t->right,f,environment);
    }
}

void bstree_depth_infix(const BinarySearchTree* t, OperateFunctor f, void* environment) {
    if(t){
        //printf("Node %d\n je vais à droite\n",t->key);
        bstree_depth_infix(t->left,f,environment);
        f(t,environment);
        //printf("Node %d\n je vais à gauche\n",t->key);
        bstree_depth_infix(t->right,f,environment);
       // printf("je remonte\n");
    }
}

void bstree_depth_postfix(const BinarySearchTree* t, OperateFunctor f, void* environment) {
    if(t){
        bstree_depth_postfix(t->left,f,environment);
        bstree_depth_postfix(t->right,f,environment);
        f(t,environment);
    }
}

void bstree_iterative_breadth(const BinarySearchTree* t, OperateFunctor f, void* environment) {
    Queue * node_queue=create_queue();
    if(t){
        f(t,environment);
        if(t->left) queue_push(node_queue,(void *)(t->left));
        if(t->right) queue_push(node_queue,(void *)(t->right));
        BinarySearchTree * act;
        while(!queue_empty(node_queue)){
            act=(BinarySearchTree*)queue_top(node_queue);
            queue_pop(node_queue);
            f(act,environment);
            if(act->left) queue_push(node_queue,(void *)(act->left));
            if(act->right) queue_push(node_queue,(void *)(act->right));
        }
    }
    delete_queue(&node_queue);
}

void bstree_iterative_depth_infix(const BinarySearchTree* t, OperateFunctor f, void* environment) {
    const BinarySearchTree * current=t;
    const BinarySearchTree * prev=NULL;
    const BinarySearchTree * next=NULL;
    while(current){
        if(prev==current->parent){
            prev=current;
            next=current->left;
        }
        if(!next || prev==current->left){
            f(current,environment);
            prev=current;
            next=current->right;
        }
        if(!next || prev==current->right){
            prev=current;
            next=current->parent;
        }
        current=next;
    }

}

/*------------------------  BSTreeIterator  -----------------------------*/

struct _BSTreeIterator {
    /* the collection the iterator is attached to */
    const BinarySearchTree* collection;
    /* the first element according to the iterator direction */
    const BinarySearchTree* (*begin)(const BinarySearchTree* );
    /* the current element pointed by the iterator */
    const BinarySearchTree* current;
    /* function that goes to the next element according to the iterator direction */
    const BinarySearchTree* (*next)(const BinarySearchTree* );
};

/* minimum element of the collection */
const BinarySearchTree* goto_min(const BinarySearchTree* e) {
	const BinarySearchTree * act=e;
    while(act->left) act=act->left;
	return act;
}

/* maximum element of the collection */
const BinarySearchTree* goto_max(const BinarySearchTree* e) {
	const BinarySearchTree * act=e;
    while(act->right) act=act->right;
	return act;
}

/* constructor */
BSTreeIterator* bstree_iterator_create(const BinarySearchTree* collection, IteratorDirection direction) {
	BSTreeIterator *iterator=malloc(sizeof(BSTreeIterator));
    iterator->collection=collection;
    if(direction==forward){
        iterator->begin=goto_min;
        iterator->current=goto_min(collection);
        iterator->next=bstree_successor;
    }
    else{
        iterator->begin=goto_max;
        iterator->current=goto_max(collection);
        iterator->next=bstree_predecessor;
    }
	return iterator;
}

/* destructor */
void bstree_iterator_delete(ptrBSTreeIterator* i) {
    free(*i);
    *i = NULL;
}

BSTreeIterator* bstree_iterator_begin(BSTreeIterator* i) {
    i->current = i->begin(i->collection);
    return i;
}

bool bstree_iterator_end(const BSTreeIterator* i) {
    return i->current == NULL;
}

BSTreeIterator* bstree_iterator_next(BSTreeIterator* i) {
    i->current = i->next(i->current);
    return i;
}

const BinarySearchTree* bstree_iterator_value(const BSTreeIterator* i) {
    return i->current;
}


void bstree_node_to_dot(const BinarySearchTree* t, void* stream){
     FILE *file = (FILE *) stream;

    printf("%d ", bstree_key(t));
    if(t->color==red) fprintf(file, "\tn%d [label=\"{%d|{<left>|<right>}}\",style=filled, fillcolor=red];\n",
            bstree_key(t), bstree_key(t));
    else fprintf(file, "\tn%d [label=\"{%d|{<left>|<right>}}\",style=filled, fillcolor=grey];\n",
            bstree_key(t), bstree_key(t));

    if (bstree_left(t)) {
        fprintf(file, "\tn%d:left:c -> n%d:n [headclip=false, tailclip=false]\n",
                bstree_key(t), bstree_key(bstree_left(t)));
    } else {
        fprintf(file, "\tlnil%d [style=filled, fillcolor=grey, label=\"NIL\"];\n", bstree_key(t));
        fprintf(file, "\tn%d:left:c -> lnil%d:n [headclip=false, tailclip=false]\n",
                bstree_key(t), bstree_key(t));
    }
    if (bstree_right(t)) {
        fprintf(file, "\tn%d:right:c -> n%d:n [headclip=false, tailclip=false]\n",
                bstree_key(t), bstree_key(bstree_right(t)));
    } else {
        fprintf(file, "\trnil%d [style=filled, fillcolor=grey, label=\"NIL\"];\n", bstree_key(t));
        fprintf(file, "\tn%d:right:c -> rnil%d:n [headclip=false, tailclip=false]\n",
                bstree_key(t), bstree_key(t));
    }
}

