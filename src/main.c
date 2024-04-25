#include <stdio.h>
#include "lalr.h"
#include "tokenizer.h"

int main() {
	tokenizer_ctx ctx = tctx_from_file("code.rf");
	lalr_ctx lctx = lalr_create();
	while ((lctx.current = tctx_get_next(&ctx)).type != T_EOF) {
		tctx_advance(&ctx);
		lctx.lookahead = tctx_get_next(&ctx);

		int reduced = 0;
		if ((reduced = lalr_reduce(&lctx)) == 0) {
			// couldn't reduce the stack
		}
		printf("curr: %s, lookahead: %s\n", token_str(lctx.current.type), token_str(lctx.lookahead.type));
	}
}
