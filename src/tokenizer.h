#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <stddef.h>
#include <regex.h>
#include "sv.h"
#include "cvector.h"

typedef enum token_type {
	// Unreserved tokens
	T_ID, 
	T_DOUBLE_LIT, T_DECIMAL_LIT, T_HEX_LIT, T_STRING_LIT, T_CHAR_LIT, 
	T_SINGLE_LINE_COMMENT, T_MULTI_LINE_COMMENT,

	// Reserved tokens
	T_RESERVE_BEG = 100,
	T_FN, T_IF, T_ELSE,
	T_SWITCH, T_BREAK, T_DEFAULT,
	T_SQUOTE, T_DQUOTE,
	T_LP, T_RP,
	T_LBRC, T_RBRC,
	T_RESERVE_END,

	T_BOR = 200, T_BAND, // bitwise

	// LogicOp 
	T_LOGIC_BEG = 300, 
	T_DEQ, T_GT, T_LT, T_GTEQ, T_LTEQ,
	T_LOR, T_LAND, // logical
	T_LOGIC_END,

	// StackOp
	T_STACK_BEG = 400,
	T_COLON, T_COMMA_SEQ, T_PERIOD_SEQ, T_SEMI_SEQ,
	T_STACK_END,

	// ArithOp
	T_ARITH_BEG = 500,
	T_MINUS, T_PLUS,T_MUL, T_DIV, T_MOD,
	T_ARITH_END,

	// Internal tokens
	T_EOF = 600, T_SPACE, T_UNKNOWN
} token_type;

typedef struct tokenizer_state {
	char const *cursor;
	int line, col, index;
} tokenizer_state;

typedef struct tokenizer_regex_store {
	regex_t r_string_lit;
	regex_t r_char_lit;
	regex_t r_fn, r_if, r_else, r_switch, r_break, r_default;
	regex_t r_hexlit, r_dbllit, r_declit, r_id;
	regex_t r_lor, r_land, r_gteq, r_lteq, r_deq;
	regex_t r_comma_seq, r_period_seq, r_semi_seq;
} tokenizer_regex_store;

typedef struct token {
	token_type type;
	String_View text;
	tokenizer_state state;
} token;

typedef struct tokenizer_ctx {
	char const *content;
	size_t content_length;
	tokenizer_state state;
	tokenizer_regex_store regex_store;
} tokenizer_ctx;

regex_t       rnew(const char*);
int           rmatch(const char*, regex_t, int*);

const char*   token_str(token_type);

bool 					is_token_terminal(token*);

tokenizer_ctx tctx_from_file(const char*);
tokenizer_ctx tctx_from_cstr(const char*);
void          tctx_free(tokenizer_ctx*);

token         tctx_advance(tokenizer_ctx*);
token         tctx_get_next(tokenizer_ctx*);
#define       tctx_show_next(t) { tctx_show_next_internal(t, __LINE__); }
void          tctx_show_next_internal(tokenizer_ctx*, int);

tokenizer_state tctx_save(tokenizer_ctx*);
void            tctx_restore(tokenizer_ctx*, tokenizer_state);

#endif
