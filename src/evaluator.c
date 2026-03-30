#include "evaluator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EXEC_OUTPUT (256 * 1024)

Value get_var(Env* env, char* name) {
    for (int i = 0; i < env->count; i++) {
        if (strcmp(env->vars[i].name, name) == 0) {
            return env->vars[i].value;
        }
    }
    printf("Error: Undefined variable %s\n", name);
    exit(1);
    Value err = {VALUE_INT, .int_val = 0};
    return err;
}

void set_var(Env* env, char* name, Value value) {
    for (int i = 0; i < env->count; i++) {
        if (strcmp(env->vars[i].name, name) == 0) {
            if (env->vars[i].value.type == VALUE_STRING) {
                free(env->vars[i].value.str_val);
            }
            env->vars[i].value = value;
            return;
        }
    }
    if (env->count >= MAX_VARS) {
        printf("Error: Too many variables\n");
        exit(1);
    }
    env->vars[env->count].name = strdup(name);
    env->vars[env->count].value = value;
    env->count++;
}

void env_init_cli_args(Env* env, int cli_argc, char** cli_argv) {
    if (cli_argc > MAX_CLI_ARGS) {
        fprintf(stderr, "Error: At most %d command-line arguments are supported\n", MAX_CLI_ARGS);
        exit(1);
    }
    Value ac = {VALUE_INT, .int_val = cli_argc};
    set_var(env, "argc", ac);
    for (int i = 0; i < cli_argc; i++) {
        char name[32];
        snprintf(name, sizeof name, "arg%d", i);
        Value v = {VALUE_STRING, .str_val = strdup(cli_argv[i])};
        if (!v.str_val) {
            fprintf(stderr, "Error: Out of memory for CLI args\n");
            exit(1);
        }
        set_var(env, name, v);
    }
}

Value eval_expr(ASTNode* node, Env* env) {
    Value result = {VALUE_INT, .int_val = 0};
    Value left, right;
    switch (node->type) {
        case AST_NUMBER:
            result.type = VALUE_INT;
            result.int_val = node->number;
            return result;
        case AST_STRING:
            result.type = VALUE_STRING;
            result.str_val = strdup(node->string);
            return result;
        case AST_IDENTIFIER: {
            Value v = get_var(env, node->identifier);
            if (v.type == VALUE_STRING && v.str_val) {
                v.str_val = strdup(v.str_val);
            }
            return v;
        }
        case AST_BINARY_OP:
            left = eval_expr(node->binop.left, env);
            right = eval_expr(node->binop.right, env);
            if (left.type != VALUE_INT || right.type != VALUE_INT) {
                printf("Error: Binary op %c on non-integers\n", node->binop.op);
                exit(1);
            }
            result.type = VALUE_INT;
            switch (node->binop.op) {
                case '+': result.int_val = left.int_val + right.int_val; break;
                case '-': result.int_val = left.int_val - right.int_val; break;
                case '*': result.int_val = left.int_val * right.int_val; break;
                case '/': 
                    if (right.int_val == 0) { printf("Error: Division by zero\n"); exit(1); }
                    result.int_val = left.int_val / right.int_val; 
                    break;
                case '>': result.int_val = left.int_val > right.int_val; break;
                case '<': result.int_val = left.int_val < right.int_val; break;
                default: printf("Unknown operator %c\n", node->binop.op); exit(1);
            }
            return result;
        default: printf("Unknown expr type\n"); exit(1);
    }
    return result;
}

void evaluate(ASTNode* node, Env* env) {
    if (!node) return;

    switch (node->type) {
        case AST_ASSIGN:
            set_var(env, node->assign.var, eval_expr(node->assign.expr, env));
            break;
        case AST_PRINT: {
            Value v = eval_expr(node->print_expr, env);
            if (v.type == VALUE_INT) {
                printf("%d\n", v.int_val);
            } else if (v.type == VALUE_STRING) {
                printf("%s\n", v.str_val);
                free(v.str_val);
            }
            break;
        }
        case AST_READ: {
            fflush(stdout);
            int n;
            if (scanf("%d", &n) != 1) {
                printf("Error: Expected integer input\n");
                exit(1);
            }
            Value v = {VALUE_INT, .int_val = n};
            set_var(env, node->read_stmt.var, v);
            break;
        }
        case AST_EXEC: {
            Value cmdval = eval_expr(node->exec_stmt.cmd, env);
            if (cmdval.type != VALUE_STRING) {
                printf("Error: exec command must be a string\n");
                exit(1);
            }
            FILE* fp = popen(cmdval.str_val, "r");
            free(cmdval.str_val);
            if (!fp) {
                perror("exec");
                exit(1);
            }
            size_t cap = 4096;
            size_t len = 0;
            char* buf = (char*)malloc(cap);
            if (!buf) {
                pclose(fp);
                printf("Error: Out of memory capturing command output\n");
                exit(1);
            }
            int c;
            while ((c = fgetc(fp)) != EOF) {
                if (len >= MAX_EXEC_OUTPUT) {
                    free(buf);
                    pclose(fp);
                    printf("Error: Command output exceeds %d bytes\n", MAX_EXEC_OUTPUT);
                    exit(1);
                }
                if (len + 1 >= cap) {
                    cap *= 2;
                    if (cap > MAX_EXEC_OUTPUT + 1) {
                        cap = MAX_EXEC_OUTPUT + 1;
                    }
                    char* nb = (char*)realloc(buf, cap);
                    if (!nb) {
                        free(buf);
                        pclose(fp);
                        printf("Error: Out of memory capturing command output\n");
                        exit(1);
                    }
                    buf = nb;
                }
                buf[len++] = (char)c;
            }
            buf[len] = '\0';
            pclose(fp);
            while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r')) {
                buf[--len] = '\0';
            }
            Value out = {VALUE_STRING, .str_val = buf};
            set_var(env, node->exec_stmt.var, out);
            break;
        }
        case AST_IF: {
            Value cond = eval_expr(node->if_stmt.cond, env);
            if (cond.type == VALUE_INT && cond.int_val) {
                evaluate(node->if_stmt.body, env);
            }
            if (cond.type == VALUE_STRING) free(cond.str_val);
            break;
        }
        case AST_WHILE: {
            for (;;) {
                Value cond = eval_expr(node->while_stmt.cond, env);
                if (cond.type != VALUE_INT || !cond.int_val) {
                    if (cond.type == VALUE_STRING) free(cond.str_val);
                    break;
                }
                if (cond.type == VALUE_STRING) free(cond.str_val);
                evaluate(node->while_stmt.body, env);
            }
            break;
        }
        case AST_BLOCK:
            for (int i = 0; i < node->block.count; i++) {
                evaluate(node->block.statements[i], env);
            }
            break;
        default: break;
    }
}

void free_env(Env* env) {
    for (int i = 0; i < env->count; i++) {
        free(env->vars[i].name);
        if (env->vars[i].value.type == VALUE_STRING) {
            free(env->vars[i].value.str_val);
        }
    }
}
