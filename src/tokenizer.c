#include "tokenizer.h"
#include <ctype.h>
#include <stdio.h>
#include <regex.h>
#include <string.h>

regex_t rnew(const char* r) {
	regex_t reg;
	if (regcomp(&reg, r, REG_EXTENDED) != 0) {
		printf("Failed to compile regex [%s]\n", r);
		exit(1);
	}
	return reg;
}

int rmatch(const char* s, regex_t r, int* length_out) {
	regmatch_t match[1];
	if (regexec(&r, s, 1, match, 0) == 0) {
		// make sure the match was immediately at s
		if (match[0].rm_so != 0)
			return -1;
		const char* start = s + match[0].rm_so;
		const char* end = s + match[0].rm_eo;
		int len = end - start;
		*length_out = len;
		return 0;
	}
	return -1;
}

char* read_file(const char* filename, size_t* length) {
	FILE* f = fopen(filename, "r");
	if (!f) {
		fprintf(stderr, "Failed to open file: %s\n", filename);
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	size_t l = ftell(f);
	fseek(f, 0, SEEK_SET);

	if (length) {
		*length = l;
	}
	char *content = calloc(l + 1, sizeof(char));
	size_t read_bytes = fread(content, sizeof(char), l, f);
	content[read_bytes] = 0;
	if (read_bytes != l) {
		fprintf(stderr, "Something went wrong reading: %lu != %lu\n", read_bytes, l);	
		free(content);
		content = NULL;
	}
	fclose(f);
	return content;
}

const char* token_str(token_type t) {
	switch (t) {
		case T_ID:      		return "ID";
		case T_FN:      		return "FN";
		case T_IF:      		return "IF";
		case T_ELSE: 				return "ELSE";
		case T_SWITCH:      return "SWITCH";
		case T_BREAK: 			return "BREAK";
		case T_DEFAULT: 		return "DEFAULT";
		case T_LP:      		return "LP";
		case T_RP:      		return "RP";
		// case T_LBKT:				return "LKBT";
		// case T_RBKT:				return "RKBT";
		case T_LBRC:				return "LBRC";
		case T_RBRC:				return "RBRC";
		case T_DEQ:					return "DEQ"; 					 
		case T_GT: 					return "GT"; 
		case T_LT: 					return "LT";
		case T_GTEQ:				return "GTEQ"; 
		case T_LTEQ: 				return "LTEQ";
		case T_LOR:  				return "LOR";
		case T_LAND:   			return "LAND";
		case T_BOR:    			return "BOR";
		case T_BAND:   			return "BAND";
		case T_COLON:   		return "COLON";
		case T_COMMA_SEQ: 	return "COMMA_SEQ";
		case T_PERIOD_SEQ:	return "PERIOD_SEQ";
		case T_SEMI_SEQ:    return "SEMI_SEQ";
		case T_MINUS:   		return "MINUS";
		case T_PLUS:    		return "PLUS";
		case T_MUL:     		return "MUL";
		case T_DIV:     		return "DIV";
		case T_MOD:  				return "MOD";
		case T_DOUBLE_LIT:  return "DOUBLE_LIT";
		case T_DECIMAL_LIT: return "DECIMAL_LIT";
		case T_HEX_LIT:   	return "HEX_LIT";
		case T_STRING_LIT:  return "STRING_LIT";
		case T_CHAR_LIT:    return "CHAR_LIT";
		case T_SQUOTE: 			return "SQUOTE";
		case T_DQUOTE:      return "DQUOTE";
		case T_EOF: 				return "EOF";
		case T_SPACE:       return "SPACE";
		case T_UNKNOWN:  		return "UNKNOWN";
		default:            printf("%d\n", t); assert(0 && "Unimplemented token_str conversion");
	}
}

void tctx_internal_init_regex(tokenizer_ctx* ctx) {
	ctx->regex_store.r_string_lit = rnew("\\\"([^\\\"]|\n)*\\\"");
	ctx->regex_store.r_char_lit   = rnew("\\\'(.)\\\'");
	ctx->regex_store.r_fn         = rnew("fn");
	ctx->regex_store.r_if         = rnew("if");
	ctx->regex_store.r_else       = rnew("else");
	ctx->regex_store.r_switch     = rnew("switch");
	ctx->regex_store.r_break      = rnew("break");
	ctx->regex_store.r_default    = rnew("default");
	ctx->regex_store.r_hexlit     = rnew("0x[0-9a-fA-F]+");
	ctx->regex_store.r_dbllit     = rnew("[0-9]+\\.[0-9]+");
	ctx->regex_store.r_declit     = rnew("[0-9]+");
	ctx->regex_store.r_id         = rnew("[a-zA-Z_][a-zA-Z0-9_]*");
	ctx->regex_store.r_lor        = rnew("\\|\\|");
	ctx->regex_store.r_land       = rnew("&&");
	ctx->regex_store.r_gteq       = rnew(">=");
	ctx->regex_store.r_lteq       = rnew("<=");
	ctx->regex_store.r_deq        = rnew("==");
	ctx->regex_store.r_comma_seq  = rnew("[,]+");
	ctx->regex_store.r_period_seq = rnew("[.]+");
	ctx->regex_store.r_semi_seq   = rnew("[;]+");
}

void tctx_internal_free_regex(tokenizer_ctx* ctx) {
	regfree(&ctx->regex_store.r_string_lit);
	regfree(&ctx->regex_store.r_char_lit);
	regfree(&ctx->regex_store.r_fn);
	regfree(&ctx->regex_store.r_if);
	regfree(&ctx->regex_store.r_else);
	regfree(&ctx->regex_store.r_switch);
	regfree(&ctx->regex_store.r_break);
	regfree(&ctx->regex_store.r_default);
	regfree(&ctx->regex_store.r_hexlit);
	regfree(&ctx->regex_store.r_dbllit);
	regfree(&ctx->regex_store.r_declit);
	regfree(&ctx->regex_store.r_id);
	regfree(&ctx->regex_store.r_lor);
	regfree(&ctx->regex_store.r_land);
	regfree(&ctx->regex_store.r_gteq);
	regfree(&ctx->regex_store.r_lteq);
	regfree(&ctx->regex_store.r_comma_seq);
	regfree(&ctx->regex_store.r_period_seq);
	regfree(&ctx->regex_store.r_semi_seq);
}

tokenizer_ctx tctx_from_file(const char* filename) {
	tokenizer_ctx ctx = {0};
	char* content = read_file(filename, &ctx.content_length);
	ctx.content = content;
	ctx.state.cursor = content;
	tctx_internal_init_regex(&ctx);
	return ctx;
}

tokenizer_ctx tctx_from_cstr(const char* cstr) {
	tokenizer_ctx ctx = {0};
	ctx.content = strdup(cstr);
	ctx.content_length = strlen(cstr);
	ctx.state.cursor = ctx.content;
	return ctx;
}

void tctx_free(tokenizer_ctx* ctx) {
	tctx_internal_free_regex(ctx);
	free((void*) ctx->content);
}

#define RMATCH(str, t) \
	do {\
		int length;\
		if (rmatch(ctx->state.cursor, str, &length) != -1) {\
			const char* was = ctx->state.cursor;\
			/* ctx->state.cursor += length;*/\
			return (token) {\
				.type = t,\
				.text=(sv_from_parts(was, length)),\
				.state = ctx->state\
			};\
		}\
	} while(0)

#define CHMATCH(c, t) \
	do {\
		int length;\
		if (*ctx->state.cursor == c) {\
			const char* was = ctx->state.cursor;\
			/* ctx->state.cursor += 1;*/ \
			return (token) {\
				.type = t,\
				.text=(sv_from_parts(was, 1)),\
				.state = ctx->state\
			};\
		}\
	} while(0)

token tctx_advance(tokenizer_ctx* ctx) {
	token t = tctx_get_next(ctx);
	ctx->state.cursor += t.text.count;
	return t;
}

token tctx_get_next(tokenizer_ctx* ctx) {
	// Detect EOF
	if (ctx->state.cursor >= ctx->content + ctx->content_length - 1)
		return (token) {.type=T_EOF };
	if (*ctx->state.cursor == '\0')
		return (token) {.type=T_EOF };

	tokenizer_state s = ctx->state;
	// Consume comments
	if (strncmp(s.cursor, "//", 2) == 0) {
		const char* begin = s.cursor;
		while (*s.cursor != '\n') {
			s.cursor++;
		}
		s.cursor++;
	}
	// Consume spaces
	while (isspace(*s.cursor) != 0) {
		s.cursor++;
		s.col++;
	}
	if (*s.cursor == '\n') {
		s.cursor++;
		s.line++;
	}
	ctx->state = s;

	// Match code
	//   To see the actual regex strings, view tctx_internal_init_regex(..)
	RMATCH(ctx->regex_store.r_string_lit, T_STRING_LIT);
	RMATCH(ctx->regex_store.r_char_lit, T_CHAR_LIT);
	RMATCH(ctx->regex_store.r_fn, T_FN);
	RMATCH(ctx->regex_store.r_if, T_IF);
	RMATCH(ctx->regex_store.r_else, T_ELSE);
	RMATCH(ctx->regex_store.r_switch, T_SWITCH);
	RMATCH(ctx->regex_store.r_break, T_BREAK);
	RMATCH(ctx->regex_store.r_default, T_DEFAULT);
	RMATCH(ctx->regex_store.r_hexlit, T_HEX_LIT);
	RMATCH(ctx->regex_store.r_dbllit, T_DOUBLE_LIT);
	RMATCH(ctx->regex_store.r_declit, T_DECIMAL_LIT);
	RMATCH(ctx->regex_store.r_id, T_ID);
	RMATCH(ctx->regex_store.r_lor, T_LOR);
	RMATCH(ctx->regex_store.r_land, T_LAND);
	RMATCH(ctx->regex_store.r_gteq, T_GTEQ);
	RMATCH(ctx->regex_store.r_lteq, T_LTEQ);
	RMATCH(ctx->regex_store.r_deq, T_DEQ);
	RMATCH(ctx->regex_store.r_comma_seq, T_COMMA_SEQ);
	RMATCH(ctx->regex_store.r_period_seq, T_PERIOD_SEQ);
	RMATCH(ctx->regex_store.r_semi_seq, T_SEMI_SEQ);
	CHMATCH('|', T_BOR);
	CHMATCH('&', T_BAND);
	CHMATCH('>', T_GT);
	CHMATCH('<', T_LT);
	CHMATCH(':', T_COLON);
	// CHMATCH(',', T_COMMA);
	// CHMATCH('.', T_PERIOD);
	CHMATCH('(', T_LP);
	CHMATCH(')', T_RP);
	CHMATCH('{', T_LBRC);
	CHMATCH('}', T_RBRC);
	CHMATCH('-', T_MINUS);
	CHMATCH('+', T_PLUS);
	CHMATCH('*', T_MUL);
	CHMATCH('/', T_DIV);
	CHMATCH('%', T_MOD);
	// CHMATCH('=', T_EQ);
	CHMATCH('\'', T_SQUOTE);
	CHMATCH('\"', T_DQUOTE);

	return (token) {.type=T_UNKNOWN, .text=sv_from_parts(ctx->state.cursor, 1)};
}

void tctx_show_next_internal(tokenizer_ctx* ctx, int line) {
	token next = tctx_get_next(ctx);
	printf("[%d] Next: %s\n", line, token_str(next.type));
}

tokenizer_state tctx_save(tokenizer_ctx* ctx) {
	return ctx->state;
}

void tctx_restore(tokenizer_ctx* ctx, tokenizer_state state) {
	ctx->state = state;
}
