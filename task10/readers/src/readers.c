#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

#include "../include/readers.h"

status_code read_config(FILE* file, config_data* config)
{
    if (file == NULL || config == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    
    config->assign = ASSIGN_LEFT;
    config->unary = UNARY_PREFIX;
    config->binary = BINARY_PREFIX;
    
    char default_operation_names[13][7] =
    {
        "not",
        "input",
        "output",
        "add",
        "mult",
        "sub",
        "pow",
        "div",
        "rem",
        "xor",
        "and",
        "or",
        "="
    };
    
    memset(config->aliases, 0, sizeof(char*) * OPERATION_COUNT);
    for (int i = 0; !code && i < OPERATION_COUNT; ++i)
    {
        config->aliases[i] = (char*) malloc(sizeof(char) * (strlen(default_operation_names[i]) + 1));
        if (config->aliases[i] == NULL)
        {
            code = BAD_ALLOC;
            break;
        }
        
        strcpy(config->aliases[i], default_operation_names[i]);
    }
    
    while (!code)
    {
        char* line = NULL;
        char* first_word = NULL;
        char* second_word = NULL;
        
        code = code ? code : fread_line_with_comments(file, &line, 1, '#', '[', ']');
        code = code ? code : (line[0] == '\0' ? FILE_END : OK);
        
        const char* iter = line;
        
        code = code ? code : sread_until(iter, " ", 0, &iter, &first_word);
        code = code ? code : sread_while(iter, " ", &iter, NULL);
        
        if (!code && *iter != '\0')
        {
            code = code ? code : sread_until(iter, " ", 0, &iter, &second_word);
            code = code ? code : sread_while(iter, " ", &iter, NULL);
            
            if (!code && *iter != '\0')
            {
                code = FILE_INVALID_CONFIG;
            }
        }
        
        if (!code)
        {
            if (second_word == NULL)
            {
                if (!strcmp(first_word, "left="))
                {
                    config->assign = ASSIGN_LEFT;
                }
                else if (!strcmp(first_word, "right="))
                {
                    config->assign = ASSIGN_RIGHT;
                }
                else if (!strcmp(first_word, "unary()"))
                {
                    config->unary = UNARY_PREFIX;
                }
                else if (!strcmp(first_word, "()unary"))
                {
                    config->unary = UNARY_POSTFIX;
                }
                else if (!strcmp(first_word, "binary()"))
                {
                    config->binary = BINARY_PREFIX;
                }
                else if (!strcmp(first_word, "(binary)"))
                {
                    config->binary = BINARY_INFIX;
                }
                else if (!strcmp(first_word, "()binary"))
                {
                    config->binary = BINARY_POSTFIX;
                }
                else
                {
                    code = FILE_INVALID_CONFIG;
                }
            }
            else
            {
                operation op;
                
                code = code ? code : parse_operation(first_word, &op);
                
                if (!code)
                {
                    free(config->aliases[op]);
                    config->aliases[op] = second_word;
                    second_word = NULL;
                }
            }
        }
        
        free(line);
        free_all(2, first_word, second_word);
    } // while end
    
    if (code == FILE_END)
    {
        code = OK;
    }
    
    for (int i = 0; !code && i < OPERATION_COUNT; ++i)
    {
        for (int j = i+1; j < OPERATION_COUNT; ++j)
        {
            if (!strcmp(config->aliases[i], config->aliases[j]))
            {
                code = FILE_INVALID_CONFIG;
                break;
            }
        }
    }
    
    if (code)
    {
        for (int i = 0; i < OPERATION_COUNT; ++i)
        {
            free(config->aliases[i]);
        }
    }
    
    return code;
}

status_code check_for_breakpoint(FILE* file, int* is_breakpoint)
{
    if (file == NULL || is_breakpoint == NULL)
    {
        return NULL_ARG;
    }
    
    *is_breakpoint = 0;
    
    skip_spaces(file);
    
    char str[] = "#BREAKPOINT";
    char* iter = str;
    char ch = getc(file);
    
    while (ch == *iter && ch != EOF)
    {
        ++iter;
        ch = getc(file);
    }
    
    if (*iter == '#')
    {
        ungetc(ch, file);
        return OK;
    }
    
    if (*iter == '\0')
    {
        *is_breakpoint = 1;
    }
    
    while (ch != '\n' && ch != EOF)
    {
        ch = getc(file);
    }
    
    return OK;
}

status_code read_instruction(FILE* file, char** instruction)
{
    if (file == NULL || instruction == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    
    size_t iter = 0;
    size_t size = 16;
    char* instruction_tmp = (char*) malloc(sizeof(char) * 16);
    if (instruction_tmp == NULL)
    {
        return BAD_ALLOC;
    }
    
    char ch = getc(file);
    
    if (ch == EOF)
    {
        free(instruction_tmp);
        return FILE_END;
    }
    
    while (!code && ch != ';' && ch != EOF)
    {
        if (isspace(ch) || ch == '#' || ch == '[')
        {   
            if (iter != 0 && instruction_tmp[iter-1] != ' ' && instruction_tmp[iter-1] != ',' &&
                    instruction_tmp[iter-1] != '(' && instruction_tmp[iter-1] != ')')
            {
                instruction_tmp[iter++] = ' ';
            }
            
            if (ch == '#')
            {
                code = skip_line(file);
            }
            else if (ch == '[')
            {
                code = skip_multi_line_comment(file, '[', ']');
            }
        }
        else
        {
            if (iter != 0 && instruction_tmp[iter-1] == ' ' &&
                    (ch == '(' || ch == ')' || ch == ','))
            {
                instruction_tmp[iter-1] = ch;
            }
            else
            {
                instruction_tmp[iter++] = ch;
            }
        }
        
        if (iter + 1 == size)
        {
            size *= 2;
            char* tmp = (char*) realloc(instruction_tmp, sizeof(char) * size);
            if (tmp == NULL)
            {
                code = BAD_ALLOC;
            }
            else
            {
                instruction_tmp = tmp;
            }
        }
        
        if (!code)
        {
            ch = getc(file);
        }
    }
    
    if (ch != ';')
    {
        code = code ? code : FILE_INVALID_CONTENT;
    }
    
    if (code)
    {
        free(instruction_tmp);
        return code;
    }
    
    if (iter != 0 && instruction_tmp[iter-1] == ' ')
    {
        instruction_tmp[iter-1] = '\0';
    }
    else
    {
        instruction_tmp[iter] = '\0';
    }
    
    *instruction = instruction_tmp;
    
    return OK;
}
