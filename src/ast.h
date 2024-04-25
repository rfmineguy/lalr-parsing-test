#ifndef AST_H
#define AST_H
#include "sv.h"
#include "tokenizer.h"

typedef struct IdList   IdList;
typedef struct Param    Param;
typedef struct AST_Node AST_Node;
typedef enum AST_NodeType { NT_UNDEF, NT_TOKEN, NT_IDLIST, NT_PARAM } AST_NodeType;

struct IdList {
	String_View id_str;
	IdList* next;
};

struct Param {
	String_View id_str, type_str;
};

struct AST_Node {
	AST_NodeType type;
	union {
		IdList* idList;
		Param   param;
		token   token;
	};
};

#endif
