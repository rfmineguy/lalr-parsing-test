#ifndef LALR_H
#define LALR_H
#include "ast.h"
#include "tokenizer.h"

typedef struct lalr_ctx {
	// lalr data
	token current;
	token lookahead;

	// parse stack
	AST_Node stack[255];
	int stack_top;
} lalr_ctx;

lalr_ctx lalr_create();
int      lalr_reduce(lalr_ctx*, AST_Node*);

void     lalr_push_astnode(lalr_ctx*, AST_Node);
void     lalr_pop_astnode(lalr_ctx*, AST_Node);
AST_Node lalr_peek_astnode(lalr_ctx*, AST_Node);
AST_Node lalr_peek_astnode_n(lalr_ctx*, int);

#endif
