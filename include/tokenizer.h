#ifndef TOKENIZER_H
#define TOKENIZER_H

typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_LET,
    TOKEN_IF,
    TOKEN_WHILE,
    TOKEN_PRINT,
    TOKEN_READ,
    TOKEN_EXEC,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_ASSIGN,
    TOKEN_GT,
    TOKEN_LT,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_STRING,
    TOKEN_UNKNOWN
} TokenType;

typedef struct Token {
    TokenType type;
    char* value;
    struct Token* next;
} Token;

Token* tokenize(const char* source);
void free_tokens(Token* head);

#endif
