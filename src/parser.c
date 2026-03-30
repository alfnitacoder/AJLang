#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ASTNode* parse_block();  // Forward declaration

static Token* current_token = NULL;

void advance() {
    if (current_token) current_token = current_token->next;
}

int match(TokenType type) {
    if (current_token && current_token->type == type) {
        advance();
        return 1;
    }
    return 0;
}

void expect(TokenType type) {
    if (!match(type)) {
        printf("Syntax error: Expected token type %d\n", type);
        exit(1);
    }
}

ASTNode* create_ast_node(ASTNodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    return node;
}

ASTNode* parse_expr();

ASTNode* parse_factor() {
    if (current_token && current_token->type == TOKEN_NUMBER) {
        int n = atoi(current_token->value);
        advance();
        ASTNode* node = create_ast_node(AST_NUMBER);
        node->number = n;
        return node;
    } else if (current_token && current_token->type == TOKEN_STRING) {
        char* s = strdup(current_token->value);
        advance();
        ASTNode* node = create_ast_node(AST_STRING);
        node->string = s;
        return node;
    } else if (current_token && current_token->type == TOKEN_IDENTIFIER) {
        char* id = strdup(current_token->value);
        advance();
        ASTNode* node = create_ast_node(AST_IDENTIFIER);
        node->identifier = id;
        return node;
    }
    printf("Syntax error: Expected number, string, or identifier\n");
    exit(1);
    return NULL;
}

ASTNode* parse_term() {
    ASTNode* left = parse_factor();
    while (current_token && (current_token->type == TOKEN_MUL || current_token->type == TOKEN_DIV)) {
        char op = current_token->value[0];
        advance();
        ASTNode* right = parse_factor();
        ASTNode* node = create_ast_node(AST_BINARY_OP);
        node->binop.op = op;
        node->binop.left = left;
        node->binop.right = right;
        left = node;
    }
    return left;
}

ASTNode* parse_expr() {
    ASTNode* left = parse_term();
    while (current_token && (current_token->type == TOKEN_PLUS || current_token->type == TOKEN_MINUS ||
                             current_token->type == TOKEN_GT || current_token->type == TOKEN_LT)) {
        char op = current_token->value[0];
        advance();
        ASTNode* right = parse_term();
        ASTNode* node = create_ast_node(AST_BINARY_OP);
        node->binop.op = op;
        node->binop.left = left;
        node->binop.right = right;
        left = node;
    }
    return left;
}

ASTNode* parse_statement() {
    if (match(TOKEN_LET)) {
        if (!current_token || current_token->type != TOKEN_IDENTIFIER) {
            printf("Syntax error: Expected identifier after let\n");
            exit(1);
        }
        char* var = strdup(current_token->value);
        advance();
        expect(TOKEN_ASSIGN);
        ASTNode* expr = parse_expr();
        expect(TOKEN_SEMICOLON);
        ASTNode* node = create_ast_node(AST_ASSIGN);
        node->assign.var = var;
        node->assign.expr = expr;
        return node;
    } else if (current_token && current_token->type == TOKEN_IDENTIFIER) {
        char* var = strdup(current_token->value);
        advance();
        if (!match(TOKEN_ASSIGN)) {
            printf("Syntax error: Expected '=' in assignment\n");
            exit(1);
        }
        ASTNode* expr = parse_expr();
        expect(TOKEN_SEMICOLON);
        ASTNode* node = create_ast_node(AST_ASSIGN);
        node->assign.var = var;
        node->assign.expr = expr;
        return node;
    } else if (match(TOKEN_PRINT)) {
        ASTNode* expr = parse_expr();
        expect(TOKEN_SEMICOLON);
        ASTNode* node = create_ast_node(AST_PRINT);
        node->print_expr = expr;
        return node;
    } else if (match(TOKEN_READ)) {
        if (!current_token || current_token->type != TOKEN_IDENTIFIER) {
            printf("Syntax error: Expected identifier after read\n");
            exit(1);
        }
        char* var = strdup(current_token->value);
        advance();
        expect(TOKEN_SEMICOLON);
        ASTNode* node = create_ast_node(AST_READ);
        node->read_stmt.var = var;
        return node;
    } else if (match(TOKEN_EXEC)) {
        ASTNode* cmd = parse_expr();
        expect(TOKEN_COMMA);
        if (!current_token || current_token->type != TOKEN_IDENTIFIER) {
            printf("Syntax error: Expected variable name after exec command\n");
            exit(1);
        }
        char* var = strdup(current_token->value);
        advance();
        expect(TOKEN_SEMICOLON);
        ASTNode* node = create_ast_node(AST_EXEC);
        node->exec_stmt.cmd = cmd;
        node->exec_stmt.var = var;
        return node;
    } else if (match(TOKEN_IMPORT)) {
        if (!current_token || current_token->type != TOKEN_STRING) {
            printf("Syntax error: import requires a string path\n");
            exit(1);
        }
        char* path = strdup(current_token->value);
        advance();
        expect(TOKEN_SEMICOLON);
        ASTNode* node = create_ast_node(AST_IMPORT);
        node->import_path = path;
        return node;
    } else if (match(TOKEN_IF)) {
        ASTNode* cond = parse_expr();
        expect(TOKEN_LBRACE);
        ASTNode* body = parse_block();
        expect(TOKEN_RBRACE);
        ASTNode* node = create_ast_node(AST_IF);
        node->if_stmt.cond = cond;
        node->if_stmt.body = body;
        return node;
    } else if (match(TOKEN_WHILE)) {
        ASTNode* cond = parse_expr();
        expect(TOKEN_LBRACE);
        ASTNode* body = parse_block();
        expect(TOKEN_RBRACE);
        ASTNode* node = create_ast_node(AST_WHILE);
        node->while_stmt.cond = cond;
        node->while_stmt.body = body;
        return node;
    }
    printf("Syntax error: Unknown statement\n");
    exit(1);
    return NULL;
}

ASTNode* parse_block() {
    ASTNode* block = create_ast_node(AST_BLOCK);
    block->block.statements = NULL;
    block->block.count = 0;

    while (current_token && current_token->type != TOKEN_RBRACE) {
        ASTNode* stmt = parse_statement();
        block->block.statements = (ASTNode**)realloc(block->block.statements, sizeof(ASTNode*) * (block->block.count + 1));
        block->block.statements[block->block.count++] = stmt;
    }
    return block;
}

ASTNode* parse(Token* tokens) {
    current_token = tokens;
    return parse_block();
}

void free_ast(ASTNode* node) {
    if (!node) return;
    switch (node->type) {
        case AST_ASSIGN:
            free(node->assign.var);
            free_ast(node->assign.expr);
            break;
        case AST_BINARY_OP:
            free_ast(node->binop.left);
            free_ast(node->binop.right);
            break;
        case AST_IDENTIFIER:
            free(node->identifier);
            break;
        case AST_STRING:
            free(node->string);
            break;
        case AST_IF:
            free_ast(node->if_stmt.cond);
            free_ast(node->if_stmt.body);
            break;
        case AST_WHILE:
            free_ast(node->while_stmt.cond);
            free_ast(node->while_stmt.body);
            break;
        case AST_PRINT:
            free_ast(node->print_expr);
            break;
        case AST_READ:
            free(node->read_stmt.var);
            break;
        case AST_EXEC:
            free_ast(node->exec_stmt.cmd);
            free(node->exec_stmt.var);
            break;
        case AST_IMPORT:
            free(node->import_path);
            break;
        case AST_BLOCK:
            for (int i = 0; i < node->block.count; i++) {
                free_ast(node->block.statements[i]);
            }
            free(node->block.statements);
            break;
        default: break;
    }
    free(node);
}
