#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#include <utility.h>
#include <int_stack.h>
#include <operand_stack.h>
#include <operator_stack.h>

#include "../include/instruction_executor.h"

status_code normalize_keywords(const char* instruction, config_data config, char** normalized);
status_code calc(const char* expression, const Trie* env, config_data config, uint32_t* res);

status_code process_assignment(const char* instruction, int assign_pos, Trie* env, config_data config);
status_code process_input(const char* var, Trie* env, config_data config);
status_code process_output(const char* expr, const Trie* env, config_data config);

status_code execute_instruction(const char* instruction, Trie* environment, config_data config)
{
    if (instruction == NULL || environment == NULL)
    {
        return NULL_ARG;
    }
    
    if (instruction[0] == '\0')
    {
        return OK;
    }
    
    status_code code = OK;
    char *normalized = NULL;
    ull len = 0;
    ll assign_pos = 0;
    char const *expr = NULL;
    
    code = code ? code : normalize_keywords(instruction, config, &normalized);
    
    for (; !code && normalized[len]; ++len)
    {
        if (normalized[len] == '=')
        {
            if (len == 0 || assign_pos)
            {
                code = INVALID_INPUT;
            }
            assign_pos = len;
        }
    }
    
    if (assign_pos)
    {
        code = code ? code : process_assignment(normalized, assign_pos, environment, config);
    }
    else if (len > 2 && (
            (config.unary == UNARY_PREFIX && normalized[0] == '$' &&
            normalized[1] == '(' && normalized[len-1] == ')') ||
            (config.unary == UNARY_POSTFIX && normalized[len-1] == '$' &&
            normalized[0] == '(' && normalized[len-2] == ')')))
    {
        char* expr = normalized;
        int null_pos = config.unary == UNARY_PREFIX ? len-1 : len-2;
        
        expr[null_pos] = '\0';
        expr += config.unary == UNARY_PREFIX ? 2 : 1;
        
        code = code ? code : process_input(expr, environment, config);
    }
    else if (len > 2 && (
            (config.unary == UNARY_PREFIX && normalized[0] == '#' &&
            normalized[1] == '(' && normalized[len-1] == ')') ||
            (config.unary == UNARY_POSTFIX && normalized[len-1] == '#' &&
            normalized[0] == '(' && normalized[len-2] == ')')))
    {
        char* expr = normalized;
        int null_pos = config.unary == UNARY_PREFIX ? len-1 : len-2;
        
        expr[null_pos] = '\0';
        expr += config.unary == UNARY_PREFIX ? 2 : 1;
        
        code = code ? code : process_output(expr, environment, config);
    }
    else
    {
        uint32_t unused = 0;
        code = code ? code : calc(normalized, environment, config, &unused);
    }
    
    free(normalized);
    
    return code;
}

status_code normalize_keywords(const char* instruction, config_data config, char** normalized)
{
    if (instruction == NULL || normalized == NULL)
    {
        return NULL_ARG;
    }
    
    ull new_iter = 0;
    char* new_str = (char*) malloc(sizeof(char) * (strlen(instruction) + 1));
    if (new_str == NULL)
    {
        return BAD_ALLOC;
    }
    
    status_code code = OK;
    const char* iter = instruction;
    
    while (!code && *iter != '\0')
    {
        char* word = NULL;
        operation op;
        
        code = code ? code : sread_until(iter, " (),", 0, &iter, &word);
              
        if (parse_operation_alias(word, config, &op) == OK)
        {
            char symbol;
            convert_operation_to_char(op, &symbol);
            new_str[new_iter++] = symbol;
        }
        else
        {   
            for (ull j = 0; word[j]; ++j)
            {
                if (('0' <= word[j] && word[j] <= '9') ||
                    ('A' <= word[j] && word[j] <= 'Z') ||
                    ('a' <= word[j] && word[j] <= 'z') ||
                    word[j] == '_')
                {
                    new_str[new_iter++] = word[j];
                }
                else
                {
                    code = code ? code : INVALID_INPUT;
                }
            }
        }
        
        while (*iter == ' ' || *iter == '(' || *iter == ')' || *iter == ',')
        {
            if (*iter != ' ')
            {
                new_str[new_iter++] = *iter;
            }
            ++iter;
        }
        
        free(word);
    }
    
    if (code)
    {
        free(new_str);
        return code;
    }
    
    new_str[new_iter] = '\0';
    *normalized = new_str;
    return OK;
}

status_code parse_expr_word(const char* word, const Trie* env, config_data config, uint32_t* value)
{
    if (word == NULL || env == NULL || value == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    
    if (parse_uint32(word, config.assign_numeral_system, value) == OK)
    {
        return OK;
    }
    
    code = code ? code : validate_var_name(word);
    code = code ? code : trie_get(env, word, value);
    code = code == BAD_ACCESS ? UNINITIALIZED_USAGE : code;
    
    return code;
}


status_code convert_unary_operations_to_prefix(const char* instruction, config_data config, char** converted)
{
    if (instruction == NULL || converted == NULL)
    {
        return NULL_ARG;
    }
    
    int instruction_len = strlen(instruction);
    
    char* new_str = (char*) malloc(sizeof(char) * (instruction_len + 1));
    if (new_str == NULL)
    {
        return BAD_ALLOC;
    }
    
    if (config.unary == UNARY_PREFIX)
    {
        strcpy(new_str, instruction);
        *converted = new_str;
        return OK;
    }
    
    status_code code = OK;
    Int_stack stack;
    ull iter = 0;
    int is_unary_expected = 0;
    int put_pos = 0;
    
    int_stack_set_null(&stack);
    
    for (ull i = 0; !code && instruction[i]; ++i)
    {
        if (iter > instruction_len)
        {
            code = INVALID_INPUT;
        }
        else if (instruction[i] == '(')
        {
            if ((i == 0) || instruction[i-1] == '(' || instruction[i-1] == ',' || instruction[i-1] == ' ')
            {
                code = code ? code : (iter + 2 > instruction_len ? INVALID_INPUT : OK);
                if (!code)
                {
                    new_str[iter++] = '_';
                }
            }
            code = code ? code : int_stack_push(&stack, iter);
            new_str[iter++] = '(';
        }
        else if (instruction[i] == ')')
        {
            int is_empty = 0;
            int pos = 0;
            
            code = code ? code : int_stack_is_empty(&stack, &is_empty);
            code = code ? code : (is_empty ? INVALID_INPUT : OK);
            code = code ? code : int_stack_top(&stack, &pos);
            code = code ? code : int_stack_pop(&stack);
            
            if (!code && new_str[pos-1] == '_')
            {
                is_unary_expected = 1;
                put_pos = pos - 1;
            }
            
            new_str[iter++] = ')';
        }
        else if (instruction[i] == '~') // is_unary(...)
        {
            code = code ? code : (is_unary_expected ? OK : INVALID_INPUT);

            new_str[put_pos] = '~';
            is_unary_expected = 0;
        }
        else
        {
            code = code ? code : (is_unary_expected ? INVALID_INPUT : OK);
            new_str[iter++] = instruction[i];
        }
    }
    
    int is_empty = 0;
    code = code ? code : int_stack_is_empty(&stack, &is_empty);
    code = code ? code : (is_empty ? OK : INVALID_INPUT);
    
    int_stack_destruct(&stack);
    
    if (code)
    {
        free(new_str);
        return code;
    }
    
    new_str[iter] = '\0';
    *converted = new_str;
    
    return OK;
}

status_code convert_unary_operations_to_postfix(const char* instruction, config_data config, char** converted)
{
    if (instruction == NULL || converted == NULL)
    {
        return NULL_ARG;
    }
    
    int instruction_len = strlen(instruction);
    
    char* new_str = (char*) malloc(sizeof(char) * (instruction_len + 1));
    if (new_str == NULL)
    {
        return BAD_ALLOC;
    }
    
    if (config.unary == UNARY_POSTFIX)
    {
        strcpy(new_str, instruction);
        *converted = new_str;
        return OK;
    }
    
    status_code code = OK;
    Int_stack nesting_stack;
    Operator_stack operator_stack;
    ull iter = 0;
    int cur_nesting = 0;
    
    int_stack_set_null(&nesting_stack);
    operator_stack_set_null(&operator_stack);
    
    for (ull i = 0; !code && instruction[i]; ++i)
    {
        if (iter > instruction_len)
        {
            code = INVALID_INPUT;
        }
        else if (instruction[i] == '~') // is_unary(...)
        {
            code = code ? code : int_stack_push(&nesting_stack, cur_nesting);
            code = code ? code : operator_stack_push(&operator_stack, instruction[i]);
        }
        else if (instruction[i] == '(')
        {
            ++cur_nesting;
            new_str[iter++] = '(';
        }
        else if (instruction[i] == ')')
        {
            --cur_nesting;
            new_str[iter++] = ')';
            
            int is_empty = 0;
            int stack_top_nesting = -1;
            char unary_oper_char;
            
            code = code ? code : int_stack_is_empty(&nesting_stack, &is_empty);
            
            if (!is_empty)
            {
                code = code ? code : int_stack_top(&nesting_stack, &stack_top_nesting);
                
                if (cur_nesting == stack_top_nesting)
                {
                    code = code ? code : operator_stack_top(&operator_stack, &unary_oper_char);
                    code = code ? code : int_stack_pop(&nesting_stack);
                    code = code ? code : operator_stack_pop(&operator_stack);
                    new_str[iter++] = unary_oper_char;
                }
            }
        }
        else
        {
            new_str[iter++] = instruction[i];
        }
    }
    
    int is_empty = 0;
    code = code ? code : int_stack_is_empty(&nesting_stack, &is_empty);
    code = code ? code : (is_empty ? OK : INVALID_INPUT);
    
    int_stack_destruct(&nesting_stack);
    operator_stack_destruct(&operator_stack);
    
    if (code)
    {
        free(new_str);
        return code;
    }
    
    new_str[iter] = '\0';
    *converted = new_str;
    
    return OK;
}


status_code validate_expr_prefix(const char* expr, config_data config)
{
    if (expr == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    const char* iter = expr;
    int is_operand_expected = 1;
    int nesting = 0;
    operation unused;
    
    while (!code && *iter)
    {
        if (parse_operation_char(*iter, &unused) == OK)
        {
            code = code ? code : (is_operand_expected ? OK : INVALID_INPUT);
            code = code ? code : (*(++iter) == '(' ? OK : INVALID_INPUT);
            ++nesting;
            ++iter;
        }
        else if (*iter == ',')
        {
            code = code ? code : (is_operand_expected ? INVALID_INPUT : OK);
            code = code ? code : (nesting > 0 ? OK : INVALID_INPUT);
            is_operand_expected = 1;
            ++iter;
        }
        else if (*iter == ')')
        {
            code = code ? code : (is_operand_expected ? INVALID_INPUT : OK);
            code = code ? code : (--nesting >= 0 ? OK : INVALID_INPUT);
            ++iter;
        }
        else
        {
            const char* tmp_iter;
            char whitelist[] = "0123456789ABCDEFGHIGKLMNOPQRSTUVWXYZabcdefghigklmnopqrstuvwxyz_";
            
            code = code ? code : (is_operand_expected ? OK : INVALID_INPUT);
            code = code ? code : sread_while(iter, whitelist, &tmp_iter, NULL);
            code = code ? code : (tmp_iter - iter > 0 ? OK : INVALID_INPUT);
            code = code ? code : (*tmp_iter == ',' || *tmp_iter == ')' || *tmp_iter == '\0' ? OK : INVALID_INPUT);
            
            is_operand_expected = 0;
            iter = tmp_iter;
        }
    }
    
    code = code ? code : (is_operand_expected ? INVALID_INPUT : OK);
    code = code ? code : (nesting ? INVALID_INPUT : OK);
    
    return code;
}

status_code validate_expr_infix(const char* expr, config_data config)
{
    if (expr == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    const char* iter = expr;
    int is_operand_expected = 1;
    int nesting = 0;
    operation oper;
    
    while (!code && *iter)
    {
        if (parse_operation_char(*iter, &oper) == OK)
        {
            code = code ? code : (is_operand_expected ? INVALID_INPUT : OK);
            code = code ? code : (is_operation_unary(oper) ? INVALID_INPUT : OK);
            is_operand_expected = 1;
            ++iter;
        }
        else if (*iter == '(')
        {
            code = code ? code : (is_operand_expected ? OK : INVALID_INPUT);
            ++nesting;
            ++iter;
        }
        else if (*iter == ')')
        {
            code = code ? code : (is_operand_expected ? INVALID_INPUT : OK);
            code = code ? code : (--nesting >= 0 ? OK : INVALID_INPUT);
            ++iter;
            
            if (!code && (parse_operation_char(*iter, &oper) == OK) && is_operation_unary(oper))
            {
                ++iter;
            }
        }
        else
        {
            const char* tmp_iter;
            char whitelist[] = "0123456789ABCDEFGHIGKLMNOPQRSTUVWXYZabcdefghigklmnopqrstuvwxyz_";
            
            code = code ? code : (is_operand_expected ? OK : INVALID_INPUT);
            code = code ? code : sread_while(iter, whitelist, &tmp_iter, NULL);
            code = code ? code : (tmp_iter - iter > 0 ? OK : INVALID_INPUT);
            
            int is_operation = parse_operation_char(*tmp_iter, &oper) == OK;
            code = code ? code : (is_operation || *tmp_iter == ')' || *tmp_iter == '\0' ? OK : INVALID_INPUT);
            code = code ? code : (is_operation && is_operation_unary(oper) ? INVALID_INPUT : OK);
            
            is_operand_expected = 0;
            iter = tmp_iter;
        }
    }
    
    code = code ? code : (is_operand_expected ? INVALID_INPUT : OK);
    code = code ? code : (nesting ? INVALID_INPUT : OK);
    
    return code;
}

status_code validate_expr_postfix(const char* expr, config_data config)
{
    if (expr == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    const char* iter = expr;
    int is_operand_expected = 1;
    int nesting = 0;
    operation unused;
    
    while (!code && *iter)
    {
        if (*iter == '(')
        {
            code = code ? code : (is_operand_expected ? OK : INVALID_INPUT);
            ++nesting;
            ++iter;
        }
        else if (*iter == ',')
        {
            code = code ? code : (is_operand_expected ? INVALID_INPUT : OK);
            code = code ? code : (nesting > 0 ? OK : INVALID_INPUT);
            is_operand_expected = 1;
            ++iter;
        }
        else if (*iter == ')')
        {
            code = code ? code : (is_operand_expected ? INVALID_INPUT : OK);
            code = code ? code : (--nesting >= 0 ? OK : INVALID_INPUT);
            code = code ? code : parse_operation_char(*(++iter), &unused);
            ++iter;
        }
        else
        {
            const char* tmp_iter;
            char whitelist[] = "0123456789ABCDEFGHIGKLMNOPQRSTUVWXYZabcdefghigklmnopqrstuvwxyz_";
            
            code = code ? code : (is_operand_expected ? OK : INVALID_INPUT);
            code = code ? code : sread_while(iter, whitelist, &tmp_iter, NULL);
            code = code ? code : (tmp_iter - iter > 0 ? OK : INVALID_INPUT);
            code = code ? code : (*tmp_iter == ',' || *tmp_iter == ')' || *tmp_iter == '\0' ? OK : INVALID_INPUT);
            
            is_operand_expected = 0;
            iter = tmp_iter;
        }
    }
    
    code = code ? code : (is_operand_expected ? INVALID_INPUT : OK);
    code = code ? code : (nesting ? INVALID_INPUT : OK);
    
    return code;
}


status_code calc_prefix(const char* expression, const Trie* env, config_data config, uint32_t* res)
{
    if (expression == NULL || env == NULL || res == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    Operand_stack operand_stack;
    Operator_stack operator_stack;
    operation oper;
    ull operand_cnt = 0;
    int is_operator_stack_empty = 0;
    
    operand_stack_set_null(&operand_stack);
    operator_stack_set_null(&operator_stack);
    
    const char* iter = expression;
    
    while (!code && *iter)
    {
        if (parse_operation_char(*iter, &oper) == OK)
        {
            code = code ? code : operator_stack_push(&operator_stack, *iter);
            ++iter;
        }
        else if (*iter == ',' || *iter == '(')
        {
            ++iter;
        }
        else if (*iter == ')')
        {
            char operator_char;
            uint32_t first_operand, second_operand, res;
            
            code = code ? code : operand_stack_size(&operand_stack, &operand_cnt);
            code = code ? code : operator_stack_is_empty(&operator_stack, &is_operator_stack_empty);
            code = code ? code : (is_operator_stack_empty ? INVALID_INPUT : OK);
            code = code ? code : operator_stack_top(&operator_stack, &operator_char);
            code = code ? code : operator_stack_pop(&operator_stack);
            code = code ? code : parse_operation_char(operator_char, &oper);
            
            if (!is_operation_unary(oper))
            {
                code = code ? code : (operand_cnt < 2 ? INVALID_INPUT : OK);
                code = code ? code : operand_stack_top(&operand_stack, &second_operand);
                code = code ? code : operand_stack_pop(&operand_stack);
            }
            
            code = code ? code : (operand_cnt < 1 ? INVALID_INPUT : OK);
            code = code ? code : operand_stack_top(&operand_stack, &first_operand);
            code = code ? code : operand_stack_pop(&operand_stack);
            code = code ? code : calc_operation_uint32(oper, first_operand, second_operand, &res);
            code = code ? code : operand_stack_push(&operand_stack, res);
            
            ++iter;
        }
        else
        {
            char* word = NULL;
            uint32_t value;
            code = code ? code : sread_until(iter, ",)", 0, &iter, &word);
            code = code ? code : parse_expr_word(word, env, config, &value);
            code = code ? code : operand_stack_push(&operand_stack, value);
            free(word);
        }
    }
    
    code = code ? code : operand_stack_size(&operand_stack, &operand_cnt);
    code = code ? code : operator_stack_is_empty(&operator_stack, &is_operator_stack_empty);
    code = code ? code : (is_operator_stack_empty ? OK : INVALID_INPUT);
    code = code ? code : (operand_cnt == 1 ? OK : INVALID_INPUT);
    code = code ? code : operand_stack_top(&operand_stack, res);
    
    operand_stack_destruct(&operand_stack);
    operator_stack_destruct(&operator_stack);
    
    return code;
}

status_code calc_infix(const char* expression, const Trie* env, config_data config, uint32_t* res)
{
    if (expression == NULL || env == NULL || res == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    Operand_stack operand_stack;
    Operator_stack operator_stack;
    operation oper, prev_oper;
    ull operand_cnt = 0;
    int is_operator_stack_empty = 0;
    
    operand_stack_set_null(&operand_stack);
    operator_stack_set_null(&operator_stack);
    
    const char* iter = expression;
    
    while (!code && *iter)
    {
        if (parse_operation_char(*iter, &oper) == OK)
        {            
            if (is_operation_unary(oper))
            {
                uint32_t operand, res;
                
                code = code ? code : operand_stack_top(&operand_stack, &operand);
                code = code ? code : operand_stack_pop(&operand_stack);
                code = code ? code : calc_operation_uint32(oper, operand, 0, &res);
                code = code ? code : operand_stack_push(&operand_stack, res);
            }
            else
            {
                code = code ? code : operator_stack_is_empty(&operator_stack, &is_operator_stack_empty);
            
                while (!code && !is_operator_stack_empty)
                {
                    char prev_oper_char;
                    uint32_t first_operand, second_operand, res;
                    
                    code = code ? code : operator_stack_top(&operator_stack, &prev_oper_char);
                    
                    int cur_precedence = get_operation_precedence(oper);
                    int prev_precedence = 999;
                    
                    if (prev_oper_char != '(')
                    {
                        code = code ? code : parse_operation_char(prev_oper_char, &prev_oper);
                        prev_precedence = get_operation_precedence(prev_oper);
                    }
                    
                    if (prev_precedence > cur_precedence)
                    {
                        break;
                    }
                    
                    code = code ? code : operand_stack_top(&operand_stack, &second_operand);
                    code = code ? code : operand_stack_pop(&operand_stack);
                    code = code ? code : operand_stack_top(&operand_stack, &first_operand);
                    code = code ? code : operand_stack_pop(&operand_stack);
                    code = code ? code : calc_operation_uint32(prev_oper, first_operand, second_operand, &res);
                    code = code ? code : operand_stack_push(&operand_stack, res);
                    
                    code = code ? code : operator_stack_pop(&operator_stack);
                    code = code ? code : operator_stack_is_empty(&operator_stack, &is_operator_stack_empty);
                }
                
                code = code ? code : operator_stack_push(&operator_stack, *iter);
            }
            
            ++iter;
        }
        else if (*iter == '(')
        {
            code = code ? code : operator_stack_push(&operator_stack, '(');
            ++iter;
        }
        else if (*iter == ')')
        {
            char oper_char = '\0';
            
            code = code ? code : operator_stack_top(&operator_stack, &oper_char);
            code = code ? code : operator_stack_pop(&operator_stack);
            
            while (!code && oper_char != '(')
            {
                uint32_t first_operand, second_operand, res;
                
                code = code ? code : parse_operation_char(oper_char, &oper);
                code = code ? code : operand_stack_top(&operand_stack, &second_operand);
                code = code ? code : operand_stack_pop(&operand_stack);
                code = code ? code : operand_stack_top(&operand_stack, &first_operand);
                code = code ? code : operand_stack_pop(&operand_stack);
                code = code ? code : calc_operation_uint32(oper, first_operand, second_operand, &res);
                code = code ? code : operand_stack_push(&operand_stack, res);
                
                code = code ? code : operator_stack_top(&operator_stack, &oper_char);
                code = code ? code : operator_stack_pop(&operator_stack);
            }
            
            ++iter;
        }
        else
        {
            char* word = NULL;
            uint32_t value;
            code = code ? code : sread_until(iter, "+-*/%@^&|,)", 0, &iter, &word);
            code = code ? code : parse_expr_word(word, env, config, &value);
            code = code ? code : operand_stack_push(&operand_stack, value);
            free(word);
        }
    }
    
    code = code ? code : operator_stack_is_empty(&operator_stack, &is_operator_stack_empty);
    
    while (!code && !is_operator_stack_empty)
    {
        char oper_char;
        uint32_t first_operand, second_operand, res;
        
        code = code ? code : operator_stack_top(&operator_stack, &oper_char);
        code = code ? code : parse_operation_char(oper_char, &oper);
        
        code = code ? code : operand_stack_top(&operand_stack, &second_operand);
        code = code ? code : operand_stack_pop(&operand_stack);
        code = code ? code : operand_stack_top(&operand_stack, &first_operand);
        code = code ? code : operand_stack_pop(&operand_stack);
        code = code ? code : calc_operation_uint32(oper, first_operand, second_operand, &res);
        code = code ? code : operand_stack_push(&operand_stack, res);
        
        code = code ? code : operator_stack_pop(&operator_stack);
        code = code ? code : operator_stack_is_empty(&operator_stack, &is_operator_stack_empty);
    }
    
    code = code ? code : operand_stack_size(&operand_stack, &operand_cnt);
    code = code ? code : (operand_cnt == 1 ? OK : INVALID_INPUT);
    code = code ? code : operand_stack_top(&operand_stack, res);
    
    operand_stack_destruct(&operand_stack);
    operator_stack_destruct(&operator_stack);
    
    code = code == BAD_ACCESS ? INVALID_INPUT : code;
    
    return code;
}

status_code calc_postfix(const char* expression, const Trie* env, config_data config, uint32_t* res)
{
    if (expression == NULL || env == NULL || res == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    Operand_stack operand_stack;
    operation oper;
    ull operand_cnt = 0;
    
    operand_stack_set_null(&operand_stack);
    
    const char* iter = expression;
    
    while (!code && *iter)
    {
        if (parse_operation_char(*iter, &oper) == OK)
        {
            uint32_t first_operand, second_operand, res;
            
            code = code ? code : operand_stack_size(&operand_stack, &operand_cnt);
            
            if (!is_operation_unary(oper))
            {
                code = code ? code : (operand_cnt < 2 ? INVALID_INPUT : OK);
                code = code ? code : operand_stack_top(&operand_stack, &second_operand);
                code = code ? code : operand_stack_pop(&operand_stack);
            }
            
            code = code ? code : (operand_cnt < 1 ? INVALID_INPUT : OK);
            code = code ? code : operand_stack_top(&operand_stack, &first_operand);
            code = code ? code : operand_stack_pop(&operand_stack);
            code = code ? code : calc_operation_uint32(oper, first_operand, second_operand, &res);
            code = code ? code : operand_stack_push(&operand_stack, res);
            
            ++iter;
        }
        else if (*iter == ',' || *iter == '(' || *iter == ')')
        {
            ++iter;
        }
        else
        {
            char* word = NULL;
            uint32_t value;
            code = code ? code : sread_until(iter, ",)", 0, &iter, &word);
            code = code ? code : parse_expr_word(word, env, config, &value);
            code = code ? code : operand_stack_push(&operand_stack, value);
            free(word);
        }
    }
    
    code = code ? code : operand_stack_size(&operand_stack, &operand_cnt);
    code = code ? code : (operand_cnt == 1 ? OK : INVALID_INPUT);
    code = code ? code : operand_stack_top(&operand_stack, res);
    
    operand_stack_destruct(&operand_stack);
    
    return code;
}

status_code calc(const char* expression, const Trie* env, config_data config, uint32_t* res)
{
    status_code code = OK;
    char* converted_expr = NULL;
    
    switch (config.binary)
    {
        case BINARY_PREFIX:
            code = code ? code : convert_unary_operations_to_prefix(expression, config, &converted_expr);
            code = code ? code : validate_expr_prefix(converted_expr, config);
            code = code ? code : calc_prefix(converted_expr, env, config, res);
            break;
        case BINARY_INFIX:
            code = code ? code : convert_unary_operations_to_postfix(expression, config, &converted_expr);
            code = code ? code : validate_expr_infix(converted_expr, config);
            code = code ? code : calc_infix(converted_expr, env, config, res);
            break;
        case BINARY_POSTFIX:
            code = code ? code : convert_unary_operations_to_postfix(expression, config, &converted_expr);
            code = code ? code : validate_expr_postfix(converted_expr, config);
            code = code ? code : calc_postfix(converted_expr, env, config, res);
            break;
        default:
            code = INVALID_INPUT;
    }
    
    free(converted_expr);
    
    return code;
}


status_code process_assignment(const char* instruction, int assign_pos, Trie* env, config_data config)
{
    if (instruction == NULL || env == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    
    if (strlen(instruction) < assign_pos + 2)
    {
        code = INVALID_INPUT;
    }
    
    char* left_word = NULL;
    char const* right_word = NULL;
    
    code = code ? code : sread_until(instruction, "=", 0, &right_word, &left_word);  
    right_word++;
    
    char const* var = config.assign == ASSIGN_LEFT ? left_word : right_word;
    char const* expr = config.assign == ASSIGN_LEFT ? right_word : left_word;
      
    uint32_t value = 0;
    int is_contained = 0;
    
    code = code ? code : validate_var_name(var);
    code = code ? code : calc(expr, env, config, &value);
    code = code ? code : trie_contains(env, var, &is_contained);
    
    if (is_contained)
    {
        code = code ? code : trie_set(env, var, value);
    }
    else
    {
        code = code ? code : trie_insert(env, var, value);
    }
    
    free(left_word);
    
    return code;
}

status_code process_input(const char* var, Trie* env, config_data config)
{
    if (var == NULL || env == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    char* raw_value = NULL;
    uint32_t value;
    int is_contained = 0;
    
    code = code ? code : validate_var_name(var);
    
    if (!code)
    {
        printf("Enter variable %s (%llu-base): ", var, config.assign_numeral_system);
    }
    
    code = code ? code : fread_word(stdin, &raw_value, 1);
    code = code ? code : parse_uint32(raw_value, config.assign_numeral_system, &value);
    code = code ? code : trie_contains(env, var, &is_contained);
    
    if (is_contained)
    {
        code = code ? code : trie_set(env, var, value);
    }
    else
    {
        code = code ? code : trie_insert(env, var, value);
    }
    
    free(raw_value);
    
    return code;
}

status_code process_output(const char* var, const Trie* env, config_data config)
{
    if (var == NULL || env == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    uint32_t value;
    char based_value[33];
    
    code = code ? code : parse_expr_word(var, env, config, &value);
    code = code ? code : convert_uint32(value, config.output_numeral_system, based_value);
    code = code ? code : remove_leading_zero(based_value);
    
    if (!code)
    {
        printf("Value of variable '%s' (%llu-base): %s\n", var, config.output_numeral_system, based_value);
    }
    
    return code;
}
