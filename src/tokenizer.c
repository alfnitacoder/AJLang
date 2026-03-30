#include "tokenizer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

Token* create_token(TokenType type, char* value) {
    Token* token = (Token*)malloc(sizeof(Token));
    token->type = type;
    token->value = strdup(value);
    token->next = NULL;
    return token;
}

void free_token(Token* token) {
    free(token->value);
    free(token);
}

Token* tokenize(const char* source) {
    Token* head = NULL;
    Token* current = NULL;

    int i = 0;
    int len = strlen(source);

    while (i < len) {
        // Skip whitespace
        if (isspace(source[i])) {
            i++;
            continue;
        }

        // Comments: // to end of line
        if (source[i] == '/' && i+1 < len && source[i+1] == '/') {
            while (i < len && source[i] != '\n') i++;
            continue;
        }

        // Numbers
        if (isdigit(source[i])) {
            char num[256];
            int j = 0;
            while (i < len && isdigit(source[i])) {
                num[j++] = source[i++];
            }
            num[j] = '\0';
            Token* token = create_token(TOKEN_NUMBER, num);
            if (!head) head = token;
            if (current) current->next = token;
            current = token;
            continue;
        }

        // Identifiers and keywords
        if (isalpha(source[i]) || source[i] == '_') {
            char id[256];
            int j = 0;
            while (i < len && (isalnum(source[i]) || source[i] == '_')) {
                id[j++] = source[i++];
            }
            id[j] = '\0';

            TokenType type = TOKEN_IDENTIFIER;
            if (strcmp(id, "let") == 0) type = TOKEN_LET;
            else if (strcmp(id, "if") == 0) type = TOKEN_IF;
            else if (strcmp(id, "while") == 0) type = TOKEN_WHILE;
            else if (strcmp(id, "print") == 0) type = TOKEN_PRINT;
            else if (strcmp(id, "read") == 0) type = TOKEN_READ;
            else if (strcmp(id, "exec") == 0) type = TOKEN_EXEC;
            else if (strcmp(id, "import") == 0) type = TOKEN_IMPORT;

            Token* token = create_token(type, id);
            if (!head) head = token;
            if (current) current->next = token;
            current = token;
            continue;
        }

        // Strings
        if (source[i] == '"') {
            i++;  // Skip opening "
            char str[256];
            int j = 0;
            while (i < len && source[i] != '"') {
                str[j++] = source[i++];
            }
            if (i >= len || source[i] != '"') {
                printf("Error: Unclosed string\n");
                exit(1);
            }
            str[j] = '\0';
            i++;  // Skip closing "
            Token* token = create_token(TOKEN_STRING, str);
            if (!head) head = token;
            if (current) current->next = token;
            current = token;
            continue;
        }

        // Operators and punctuation
        char ch = source[i];
        char* val = (char*)malloc(2);
        val[0] = ch;
        val[1] = '\0';
        TokenType type = TOKEN_UNKNOWN;

        switch (ch) {
            case '+': type = TOKEN_PLUS; break;
            case '-': type = TOKEN_MINUS; break;
            case '*': type = TOKEN_MUL; break;
            case '/': type = TOKEN_DIV; break;
            case '=': type = TOKEN_ASSIGN; break;
            case '>': type = TOKEN_GT; break;
            case '<': type = TOKEN_LT; break;
            case '{': type = TOKEN_LBRACE; break;
            case '}': type = TOKEN_RBRACE; break;
            case ';': type = TOKEN_SEMICOLON; break;
            case ',': type = TOKEN_COMMA; break;
            default: printf("Unknown token: %c\n", ch); free(val); i++; continue;
        }

        Token* token = create_token(type, val);
        free(val);
        if (!head) head = token;
        if (current) current->next = token;
        current = token;
        i++;
    }

    return head;
}

void free_tokens(Token* head) {
    Token* current = head;
    while (current) {
        Token* next = current->next;
        free_token(current);
        current = next;
    }
}
