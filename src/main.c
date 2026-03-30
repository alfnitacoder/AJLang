#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"
#include "parser.h"
#include "evaluator.h"

char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Could not open file");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(length + 1);
    if (!buffer) {
        fclose(file);
        perror("Memory allocation failed");
        exit(1);
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);
    return buffer;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <script.aj> [arguments...]\n", argv[0]);
        printf("  In the script: argc = number of args after the script;\n");
        printf("  arg0, arg1, ... = each argument as a string.\n");
        return 1;
    }

    char* source = read_file(argv[1]);
    Token* tokens = tokenize(source);
    ASTNode* ast = parse(tokens);

    Env env = {0};
    env_init_cli_args(&env, argc - 2, argv + 2);
    evaluate(ast, &env);

    free_ast(ast);
    free_tokens(tokens);
    free_env(&env);
    free(source);

    return 0;
}
