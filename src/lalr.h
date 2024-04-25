#ifndef LALR_H
#define LALR_H
#include "tokenizer.h"

typedef struct lalr_ctx {
	token current;
	token lookahead;
} lalr_ctx;

lalr_ctx lalr_create();
int      lalr_reduce(lalr_ctx*);

#endif
