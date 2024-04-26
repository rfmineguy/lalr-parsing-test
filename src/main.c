#include "ast.h"
#include "lalr.h"
#include "tokenizer.h"
#include <stdio.h>

// This program will only parse
//   <idlist>   = <id> <idlist> | lambda
//   <param>    = <id> : <id>

int main() {
	tokenizer_ctx ctx = tctx_from_file("code.rf");
	lalr_ctx lctx = lalr_create();
	while ((lctx.current = tctx_get_next(&ctx)).type != T_EOF) {
		tctx_advance(&ctx);
		lctx.lookahead = tctx_get_next(&ctx);
	
		int reduced = 0;
		AST_Node node;
		if ((reduced = lalr_reduce_tok_to_term(lctx.current, &node)) != 0) {
			lalr_push_astnode(&lctx, node);
		}
		// lalr_show_stack(&lctx);
		while ((reduced = lalr_reduce(&lctx, &node)) != 0) {
			lalr_pop_n(&lctx, reduced);
			lalr_push_astnode(&lctx, node);
			if (lctx.skip_next == 1) {
				tctx_advance(&ctx);
				lctx.lookahead = tctx_get_next(&ctx);
				lctx.skip_next = 0;
				continue;
			}
		}
	}

	printf("\nFinal:\n");
	for (int i = 0; i <= lctx.stack_top; i++) {
		printf("%d: ", i);
		lalr_print_node(lctx.stack[i]);
	}
}
