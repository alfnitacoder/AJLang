#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "parser.h"

#define MAX_VARS 100
#define MAX_CLI_ARGS 64

typedef enum {
    VALUE_INT,
    VALUE_STRING
} ValueType;

typedef struct {
    ValueType type;
    union {
        int int_val;
        char* str_val;
    };
} Value;

typedef struct {
    char* name;
    Value value;
} Var;

typedef struct {
    Var vars[MAX_VARS];
    int count;
} Env;

Value eval_expr(ASTNode* node, Env* env);
void evaluate(ASTNode* node, Env* env);
void env_init_cli_args(Env* env, int cli_argc, char** cli_argv);
void free_env(Env* env);

#endif
