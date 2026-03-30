#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"

typedef enum {
    AST_NUMBER,
    AST_STRING,
    AST_IDENTIFIER,
    AST_BINARY_OP,
    AST_ASSIGN,
    AST_PRINT,
    AST_READ,
    AST_EXEC,
    AST_IF,
    AST_WHILE,
    AST_BLOCK
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        int number;
        char* string;
        char* identifier;
        struct {
            char op;
            struct ASTNode* left;
            struct ASTNode* right;
        } binop;
        struct {
            char* var;
            struct ASTNode* expr;
        } assign;
        struct ASTNode* print_expr;
        struct {
            char* var;
        } read_stmt;
        struct {
            struct ASTNode* cmd;
            char* var;
        } exec_stmt;
        struct {
            struct ASTNode* cond;
            struct ASTNode* body;
        } if_stmt;
        struct {
            struct ASTNode* cond;
            struct ASTNode* body;
        } while_stmt;
        struct {
            struct ASTNode** statements;
            int count;
        } block;
    };
} ASTNode;

ASTNode* parse(Token* tokens);
void free_ast(ASTNode* node);

#endif
