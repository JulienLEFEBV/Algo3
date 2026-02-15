// Need this to use the getline C function on Linux. Works without this on MacOs. Not tested on Windows.
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "token.h"
#include "queue.h"
#include "stack.h"


/** 
 * Utilities function to print the token queues
 */
void print_token(const void* e, void* user_param);
void print_queue(FILE* f, Queue* q);

bool isSymbol(char c){
	return c=='+' || c=='-' || c=='*' || c=='/' || c=='^' || c=='(' || c==')';
}

void freeQueueToken (Queue * queue){
	Token * token;
	while(queue_size(queue)>0){
		token=(Token *)(queue_top(queue));
		delete_token(&token);
		queue_pop(queue);
	}
	delete_queue(&queue);
}

bool isOperatorPriority(Token * op1, Token * op2){
	int prioOp1=token_operator_priority(op1);
	int prioOp2=token_operator_priority(op2);
	return prioOp1>prioOp2 || (prioOp1==prioOp2 && token_operator_leftAssociative(op1));
}

Queue* shuntingYard(Queue* infix){
	Queue * postfix=create_queue();
	Stack * stackOperateur=create_stack(queue_size(infix));
	Token * tokenAct;
	while(!(queue_empty(infix))){
		tokenAct=(Token *)queue_top(infix);
		if (token_is_number(tokenAct)) queue_push(postfix,(void *)tokenAct);
		else if(token_is_operator(tokenAct)){
			while(!(stack_empty(stackOperateur)) && (!(token_is_operator((Token *)stack_top(stackOperateur))) || isOperatorPriority((Token *)stack_top(stackOperateur),tokenAct))
			&& (!(token_is_parenthesis((Token *)stack_top(stackOperateur))) || token_parenthesis((Token *)stack_top(stackOperateur))!='(')){
				queue_push(postfix,(void *)(stack_top(stackOperateur)));
				stack_pop(stackOperateur);
			}
			stack_push(stackOperateur,(void *) tokenAct);
		}
		else if (token_is_parenthesis(tokenAct)){
			if (token_parenthesis(tokenAct)=='(') stack_push(stackOperateur,(void *) tokenAct);
			else{
				while(!(stack_empty(stackOperateur)) && !(token_is_parenthesis(stack_top(stackOperateur)))){
					queue_push(postfix,stack_top(stackOperateur));
					stack_pop(stackOperateur);
				}
				if (!(stack_empty(stackOperateur))){
					Token * top=(Token *)stack_top(stackOperateur);
					delete_token(&top);
					stack_pop(stackOperateur);
				}
				delete_token(&tokenAct);
			}
		}
		queue_pop(infix);
	}
	while (!(stack_empty(stackOperateur))){
		queue_push(postfix,stack_top(stackOperateur));
		stack_pop(stackOperateur);
	}
	delete_stack(&stackOperateur);
	return postfix;
}

Queue* stringToTokenQueue(const char* expression){
	Queue * queue=create_queue();
	const char* curpos = expression;
	while (curpos[0]!='\0'){
		while (curpos[0]==' ' || curpos[0]=='\n') curpos++;
		if (curpos[0]!='\0'){
			int lgVal=1;
			if (!(isSymbol(curpos[0]))) 
			while(!(isSymbol(curpos[lgVal]) || curpos[lgVal]==' ' || curpos[lgVal]=='\n' || curpos[lgVal]=='\0')) lgVal++;
			Token* token=create_token_from_string(curpos, lgVal);
			queue_push(queue,(void *)token);
			for(;lgVal>0;lgVal--) curpos++;
		}
	}
	return queue;
}



/** 
 * Function to be written by students
 */
void computeExpressions(FILE* input) {
	size_t n=0;
	ssize_t nb_char;
	char *expression=NULL;
	Queue* infix;
	Queue* postfix;
	nb_char=getline(&expression,&n,input);
	while (nb_char != -1){
		if (expression[0]!='\n'){
			printf("Input : %s",expression);
			infix=stringToTokenQueue(expression);
			print_queue(stdout,infix);
			printf("\n");
			postfix=shuntingYard(infix);
			print_queue(stdout,postfix);
			printf("\n\n");
			freeQueueToken(infix);
			freeQueueToken(postfix);
		}
		nb_char=getline(&expression,&n,input);
	}
	free(expression);
}

/** Main function for testing.
 * The main function expects one parameter that is the file where expressions to translate are
 * to be read.
 *
 * This file must contain a valid expression on each line
 *
 */
int main(int argc, char** argv){
	if (argc<2) {
		fprintf(stderr,"usage : %s filename\n", argv[0]);
		return 1;
	}
	
	FILE* input = fopen(argv[1], "r");

	if ( !input ) {
		perror(argv[1]);
		return 1;
	}

	computeExpressions(input);

	fclose(input);
	return 0;
}
 
void print_token(const void* e, void* user_param) {
	FILE* f = (FILE*)user_param;
	Token* t = (Token*)e;
	token_dump(f, t);
}

void print_queue(FILE* f, Queue* q) {
	fprintf(f, "(%d) --  ", queue_size(q));
	queue_map(q, print_token, f);
}
