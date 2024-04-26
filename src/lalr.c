#include "lalr.h"
#include "ast.h"

lalr_ctx lalr_create() {
	lalr_ctx ctx = {0};
	ctx.stack_top = -1;
	return ctx;
}

int lalr_reduce(lalr_ctx* ctx, AST_Node* out_n) {
	// idlist  = <idlist> <id> | lambda
	//   we have an ID and the lookahead is also an ID
	if (lalr_peek_astnode_n(ctx, 0).type == T_ID && ctx->lookahead.type == T_ID) {
		out_n->type = NT_IDLIST;
	}

	// param   = <id> : <id>
	if (lalr_peek_astnode_n(ctx, 0).type == T_ID &&
			lalr_peek_astnode_n(ctx, 1).type == T_COLON &&
			lalr_peek_astnode_n(ctx, 2).type == T_ID) {
		out_n->type = NT_PARAM;
	}
	return 0;
}

void lalr_push_astnode(lalr_ctx *ctx, AST_Node n) {
	ctx->stack[++ctx->stack_top] = n;
}

void lalr_pop_astnode(lalr_ctx *ctx, AST_Node n) {
	ctx->stack_top--;
	if (ctx->stack_top < -1) {
		ctx->stack_top = -1;
	}
}

AST_Node lalr_peek_astnode(lalr_ctx *ctx, AST_Node n) {
	if (ctx->stack_top < 0) {
		return (AST_Node){.type=NT_UNDEF}; // no peeking an empty stack
	}
	return ctx->stack[ctx->stack_top];
}

AST_Node lalr_peek_astnode_n(lalr_ctx *ctx, AST_Node node, int n) {
}
