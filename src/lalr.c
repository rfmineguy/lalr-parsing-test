#include "lalr.h"
#include "ast.h"
#include "sv.h"
#include <stdio.h>

lalr_ctx lalr_create() {
	lalr_ctx ctx = {0};
	ctx.stack_top = -1;
	return ctx;
}

void lalr_show_stack(lalr_ctx* ctx) {
	printf("Stack: \n");
	for (int i = 0; i <= ctx->stack_top; i++) {
		printf("%d: ", i);
		lalr_print_node(ctx->stack[i]);
	}
	printf("lookahead: %d, " SV_Fmt "\n", ctx->lookahead.type, SV_Arg(ctx->lookahead.text));
}

int lalr_reduce_tok_to_term(token tok, AST_Node* out_n) {
	out_n->type = NT_TOKEN;
	out_n->token = tok;
	return 1;
}

int lalr_reduce(lalr_ctx* ctx, AST_Node* out_n) {
	// param   = <id> : <id>
	if (lalr_peek_astnode_n(ctx, 0).type       == NT_TOKEN &&
			lalr_peek_astnode_n(ctx, 0).token.type == T_ID &&
			ctx->lookahead.type == T_COLON
		 ) {
		out_n->type = NT_TOKEN;
		out_n->token = ctx->lookahead;
		return 0;
	}
	if (lalr_peek_astnode_n(ctx, 0).type == NT_TOKEN &&
			lalr_peek_astnode_n(ctx, 0).token.type == T_COLON &&
			lalr_peek_astnode_n(ctx, 1).type == NT_TOKEN &&
			lalr_peek_astnode_n(ctx, 1).token.type == T_ID &&
			ctx->lookahead.type == T_ID
			) {
		out_n->type = NT_PARAM;
		out_n->param.type_str = ctx->lookahead.text;
		out_n->param.id_str   = lalr_peek_astnode_n(ctx, 1).token.text;
		ctx->skip_next = 1;
		return 2;
	}
	// idlist = <idlist> <id>
	if (lalr_peek_astnode_n(ctx, 0).type == NT_TOKEN &&
			lalr_peek_astnode_n(ctx, 0).token.type == T_ID &&
			lalr_peek_astnode_n(ctx, 1).type == NT_IDLIST) {
		// printf("idlist = <idlist> <id>\n");
		AST_Node idlist = lalr_peek_astnode_n(ctx, 1);
		AST_Node id     = lalr_peek_astnode_n(ctx, 0);

		AST_Node newIdList;
		newIdList.type = NT_IDLIST;
		newIdList.idList = malloc(sizeof(IdList));
		newIdList.idList->id_str = id.token.text;
		IdList* curr = idlist.idList;
		while (curr->next != NULL) curr = curr->next;
		curr->next = newIdList.idList;
		*out_n = idlist;
		return 2;
	}

	// idlist  = <idlist> <id> | lambda
	//   uses the lookahead to check whether there is an idlist
	if (lalr_peek_astnode_n(ctx, 0).type == NT_TOKEN &&
			lalr_peek_astnode_n(ctx, 0).token.type == T_ID &&
			ctx->lookahead.type == T_ID) {
		out_n->type = NT_IDLIST;
		out_n->idList = malloc(sizeof(IdList));
		out_n->idList->id_str = lalr_peek_astnode_n(ctx, 0).token.text;
		out_n->idList->next = NULL;
		return 1;
	}
	return 0;
}

void lalr_push_astnode(lalr_ctx *ctx, AST_Node n) {
	ctx->stack[++ctx->stack_top] = n;
}

void lalr_pop(lalr_ctx *ctx) {
	ctx->stack_top--;
	if (ctx->stack_top < -1) {
		ctx->stack_top = -1;
	}
}

void lalr_pop_n(lalr_ctx* ctx, int n) {
	for (int i = 0; i < n; i++) {
		lalr_pop(ctx);
	}
}

AST_Node lalr_peek_astnode(lalr_ctx *ctx, AST_Node n) {
	return lalr_peek_astnode_n(ctx, 0);
}

AST_Node lalr_peek_astnode_n(lalr_ctx *ctx, int n) {
	if (ctx->stack_top - n < 0) {
		return (AST_Node){.type=NT_UNDEF}; // no peeking an empty stack
	}
	return ctx->stack[ctx->stack_top - n];
}

AST_Node* lalr_peek_astnode_pn(lalr_ctx* ctx, int n) {
	if (ctx->stack_top - n < 0) {
		return NULL; // no peeking an empty stack
	}
	return &ctx->stack[ctx->stack_top - n];
}

void lalr_print_node(AST_Node node) {
	switch (node.type) {
		case NT_UNDEF: printf("Undefined\n"); break;
		case NT_TOKEN: printf("Token(%d): '" SV_Fmt "'\n", node.token.type, SV_Arg(node.token.text)); break;
		case NT_IDLIST: printf("IdList: "); 
										lalr_print_idlist(node.idList);
										printf("\n");
										break;
		case NT_PARAM: printf("Param: {id: " SV_Fmt ", type: " SV_Fmt "}\n",
											 SV_Arg(node.param.id_str), SV_Arg(node.param.type_str));
									 break;
	}
}

void lalr_print_idlist(IdList* list) {
	if (!list) {
		printf("NULL");
		return;
	}
	printf("ID{" SV_Fmt "} -> ", SV_Arg(list->id_str));
	lalr_print_idlist(list->next);
}
