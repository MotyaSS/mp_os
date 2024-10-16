#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

#include "../include/debugger.h"

void print_help();
status_code handle_print(Trie* environment);
status_code handle_list(Trie* environment);
status_code handle_change(Trie* environment);
status_code handle_insert(Trie* environment);
status_code handle_delete(Trie* environment);;

status_code run_debug_session(Trie* environment)
{
    int running = 1;
    status_code st = OK;
    int enter_msg_flag = 1;
    
    while (running && !st)
    {
        char* command = NULL;
        
        if (enter_msg_flag)
        {
            printf("Enter the command: ");
        }
        enter_msg_flag = 1;
        
        st = st ? st : read_line(&command, 0);
        
        if (!st && !strcmp(command, "help"))
        {
            print_help();
        }
        else if (!st && !strcmp(command, "stop"))
        {
            running = 0;
            printf("Debug session ended\n");
        }
        else if (!st && !strcmp(command, "print"))
        {
            st = handle_print(environment);
        }
        else if (!st && !strcmp(command, "list"))
        {
            st = handle_list(environment);
        }
        else if (!st && !strcmp(command, "change"))
        {
            st = handle_change(environment);
        }
        else if (!st && !strcmp(command, "insert"))
        {
            st = handle_insert(environment);
        }
        else if (!st && !strcmp(command, "delete"))
        {
            st = handle_delete(environment);
        }

        else if (!st && !strcmp(command, "shutdown"))
        {
            st = SHUTDOWN;
        }
        else
        {
            printf("Unknown command. Try again (\"help\" to more info): ");
            enter_msg_flag = 0;
        }
        
        free(command);
    }
    return st;
}


void print_help()
{
    printf("\n");
    printf("print    - prints a variable\n");
    printf("list     - lists all initialized variables\n");
    printf("change   - changes an initialized variable\n");
    printf("insert   - initializes a variable\n");
    printf("delete   - uninitializes a variable\n");
    printf("stop     - ends debug session\n");
    printf("shutdown - terminates the interpreter\n\n");
}

status_code handle_print(Trie* environment)
{
    status_code st = OK;
    uint32_t value = 0;
    int flag = 1;
    
    printf("Enter name of variable you want to check (0 to exit): ");
    while (!st && flag)
    {
        char* name = NULL;
        
        st = st ? st : read_line(&name, 0);
        st = st ? st : (strcmp(name, "0") ? st : USER_INPUT_END);
        st = st ? st : validate_var_name(name);
        st = st ? st : trie_get(environment, name, &value);
        if (!st)
        {
            printf("%X\n", value);
            uint8_t* iter = (uint8_t*) &value;
            for (ull i = 0; i < sizeof(uint32_t) / sizeof(uint8_t); ++i)
            {
                for (ll mask = 128; mask > 0; mask >>= 1)
                {
                    printf("%d", *iter & mask ? 1 : 0);
                }
                printf(" ", *iter);
                ++iter;
            }
            printf("\n");
            flag = 0;
        }
        
        if (st == INVALID_INPUT)
        {
            printf("Invalid variable name. Try again (0 to exit): ");
            st = OK;
        }
        if (st == BAD_ACCESS)
        {
            printf("This variable does not exist. Try again (0 to exit): ");
            st = OK;
        }
        if (st == USER_INPUT_END)
        {
            st = OK;
            flag = 0;
        }
        
        free(name);
    }

    return st;
}

status_code handle_list(Trie* environment)
{
    status_code st = OK;
    ull trie_size = 0;
    trie_key_val* trie_info = NULL;
    
    st = st ? st : trie_get_key_vals(environment, &trie_size, &trie_info);
    
    if (!st)
    {
        if (trie_size == 0)
        {
            printf("No variables are initialized\n");
        }
        for (int i = 0; i < trie_size; ++i)
        {
            printf("%s = %u\n", trie_info[i].key, trie_info[i].value);
            free(trie_info[i].key);
        }
        free(trie_info);
    }
    
    return st;
}

status_code handle_change(Trie* environment)
{
    status_code st = OK;
    int flag = 1;
    
    printf("Enter name of variable you want to change value of (0 to exit): ");
    while (!st && flag)
    {
        char* name = NULL;
        
        st = st ? st : read_line(&name, 0);
        st = st ? st : (strcmp(name, "0") ? st : USER_INPUT_END);
        st = st ? st : validate_var_name(name);
        
        uint32_t value = 0;
        st = st ? st : trie_get(environment, name, &value);
        if (!st)
        {
            printf("Enter new value of variable '%s' (16-base): ", name);
            while (!st && flag)
            {
                char* new_value = NULL;
                
                st = st ? st : read_line(&new_value, 0);
                st = st ? st : (strcmp(name, "0") ? st : USER_INPUT_END);
                st = st ? st : parse_uint32(new_value, 16, &value);
                st = st ? st : trie_set(environment, name, value);
                flag = st ? flag : 0;
                
                if (st == INVALID_INPUT)
                {
                    printf("Invalid input. Try again (0 to exit): ");
                    st = OK;
                }
                
                free(new_value);
            }
        }
        
        if (st == INVALID_INPUT)
        {
            printf("Invalid variable name. Try again (0 to exit): ");
            st = OK;
        }
        if (st == BAD_ACCESS)
        {
            printf("Unknown variable! Try again (0 to exit): ");
            st = OK;
        }
        if (st == USER_INPUT_END)
        {
            st = OK;
            flag = 0;
        }
        
        free(name);
        name = NULL;
    }
    
    return st;
}

status_code handle_insert(Trie* environment)
{
    status_code st = OK;
    int flag = 1;
    
    printf("Enter name of new variable (0 to exit): ");
    while (!st && flag)
    {
        char* name = NULL;
        uint32_t value = 0;
        int contains = 0;
        
        st = st ? st : read_line(&name, 0);
        st = st ? st : (strcmp(name, "0") ? st : USER_INPUT_END);
        st = st ? st : validate_var_name(name);
        st = st ? st : trie_contains(environment, name, &contains);
        
        if (contains)
        {
            printf("This variable already exists! Try again (0 to exit): ");
        }
        else if (!st)
        {
            printf("Choose format of input\n");
            printf("1 - zeckendorf representation; 2 - roman numerals\n");
            printf("Enter identifier (0 to exit): ");
            
            while (!st && flag)
            {
                char* type = NULL;
                char* new_value = NULL;
                
                st = st ? st : read_line(&type, 0);
                st = st ? st : (strcmp(name, "0") ? st : USER_INPUT_END);
                
                if (!st && (!strcmp(type, "1") || !strcmp(type, "2")))
                {
                    status_code (*parse)(const char*, uint32_t*) = type[0] == '1'
                            ? parse_zeckendorf_uint32
                            : parse_roman_uint32;
                    
                    printf("Enter value: ");
                    st = st ? st : read_line(&new_value, 0);
                    st = st ? st : parse(new_value, &value);
                    st = st ? st : trie_insert(environment, name, value);
                    flag = st ? flag : 0;
                    
                    free(new_value);
                }
                else if (!st)
                {
                    printf("Wrong identifier! Try enter identifier again (0 to exit): ");
                }
                if (st == INVALID_INPUT)
                {
                    printf("Invalid input. Enter identifier again (0 to exit): ");
                    st = OK;
                }
                
                free(type);
            }
        }
        
        if (st == INVALID_INPUT)
        {
            printf("Invalid variable name. Try again (0 to exit): ");
            st = OK;
        }
        if (st == USER_INPUT_END)
        {
            st = OK;
            flag = 0;
        }
        
        free(name);
    }
    
    return st;
}

status_code handle_delete(Trie* environment)
{
    status_code st = OK;
    int flag = 1;
    
    printf("Enter name of variable you want to delete (0 to exit): ");
    while (!st && flag)
    {
        char* name = NULL;
        
        st = st ? st : read_line(&name, 0);
        st = st ? st : (strcmp(name, "0") ? st : USER_INPUT_END);
        st = st ? st : validate_var_name(name);
        st = st ? st : trie_erase(environment, name);
        flag = st ? flag : 0;
        
        if (st == INVALID_INPUT)
        {
            printf("Invalid variable name. Try again (0 to exit): ");
            st = OK;
        }
        if (st == BAD_ACCESS)
        {
            printf("Unknown variable. Try again (0 to exit): ");
            st = OK;
        }
        if (st == USER_INPUT_END)
        {
            st = OK;
            flag = 0;
        }
        
        free(name);
    }
    
    return st;
}
