#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "io.h"
#include "tokenizer.h"
#include "parser.h"
#include "evaluator.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <script.aj> [arguments...]\n", argv[0]);
        printf("  In the script: argc = number of args after the script;\n");
        printf("  arg0, arg1, ... = each argument as a string.\n");
        return 1;
    }

    char* source = read_file(argv[1]);
    if (!source) {
        perror(argv[1]);
        return 1;
    }
    Token* tokens = tokenize(source);
    ASTNode* ast = parse(tokens);

    Env env = {0};
    env_init_cli_args(&env, argc - 2, argv + 2);
    evaluator_set_script_for_imports(argv[1]);
    evaluate(ast, &env);

    free_ast(ast);
    free_tokens(tokens);
    free_env(&env);
    free(source);

    return 0;
}
