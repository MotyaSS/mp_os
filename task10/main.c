#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <fileapi.h>
#endif // _WIN32

#include <utility.h>
#include <trie.h>
#include <readers.h>
#include <instruction_executor.h>
#include <debugger.h>

status_code validate_input(
    int argc,
    char** argv,
    char** input_path,
    char** config_path,
    int* help_flag,
    int* debug_flag,
    config_data* config);

void print_usage();
status_code save_config(char* config_path);
status_code find_previous_config(char** config_path);

int main(int argc, char** argv)
{
    status_code code = OK;
    config_data config;
    char* config_path = NULL;
    char* input_path = NULL;
    int help_flag = 0;
    int debug_flag = 0;
    int tmp_config = 0;
    
    // config_path = "D:/Code/VSCode/MAI_MathPrac/lab4/task10/config";
    // input_path = "D:/Code/VSCode/MAI_MathPrac/lab4/task10/input";
    // config.assign_numeral_system = 10;
    // config.input_numeral_system = 10;
    // config.output_numeral_system = 10;
    // debug_flag = 1;
    
    code = validate_input(argc, argv, &input_path, &config_path, &help_flag, &debug_flag, &config);
    if (code)
    {
        print_error(code, 1);
        printf("Run \"%s --help\" for more information.\n", argv[0]);
        return code;
    }
    
    if (help_flag)
    {
        print_usage();
        return OK;
    }
    
    if (!code && config_path == NULL)
    {
        tmp_config = 1;
        
        code = find_previous_config(&config_path);
        if (code == FILE_OPENING_ERROR)
        {
            printf("Cannot locate configuration file. Please enter it manually.\n");
            return code;
        }
        if (code)
        {
            print_error(code, 1);
            return code;
        }
    }
    
    Trie environment;
    trie_set_null(&environment);
    
    FILE* config_file = fopen(config_path, "r");
    if (config_file == NULL)
    {
        printf("Cannot open config file\n");
        return FILE_OPENING_ERROR;
    }
    
    if (tmp_config)
    {
        printf("Previous configuration successfully loaded\n");
    }
    
    FILE* input_file = fopen(input_path, "r");
    if (input_file == NULL)
    {
        fclose(config_file);
        printf("Cannot open code file\n");
        return FILE_OPENING_ERROR;
    }
    
    code = code ? code : read_config(config_file, &config);
    code = code ? code : trie_construct(&environment);
    fclose(config_file);
    
    while (!code)
    {
        char* instruction = NULL;
        int is_breakpoint = 0;
        
        code = code ? code : check_for_breakpoint(input_file, &is_breakpoint);
        
        if (is_breakpoint && debug_flag)
        {
            code = code ? code : run_debug_session(&environment);
            continue;
        }
        
        code = code ? code : read_instruction(input_file, &instruction);
        code = code ? code : execute_instruction(instruction, &environment, config);
        
        free(instruction);
    }
    
    if (code == FILE_END)
    {
        code = OK;
    }
    
    if (!tmp_config)
    {
        status_code c = save_config(config_path);
        if (c)
        {
            printf("Failed to save configuration.\n");
        }
    }
    else
    {
        free(config_path);
    }
    
    fclose(input_file);
    trie_destruct(&environment);
    config_destruct(config);
    
    code = code == SHUTDOWN ? OK : code;
    if (code)
    {
        print_error(code, 1);
    }
    
    return code;
}

status_code validate_input(
    int argc,
    char** argv,
    char** input_path,
    char** config_path,
    int* help_flag,
    int* debug_flag,
    config_data* config)
{
    if (argv == NULL || help_flag == NULL || input_path == NULL ||
            config_path == NULL || debug_flag == NULL || config == NULL)
    {
        return NULL_ARG;
    }
    
    if (argc == 1)
    {
        return INVALID_INPUT;
    }
    
    *help_flag = 0;
    *input_path = NULL;
    *config_path = NULL;
    *debug_flag = 0;
    config->assign_numeral_system = 10;
    config->input_numeral_system = 10;
    config->output_numeral_system = 10;
    
    int iter = 1;
    int mask = 0;
    
    while (iter < argc)
    {
        char* cur = argv[iter];
        
        if (!strcmp(cur, "-h") || !strcmp(cur, "/h") || !strcmp(cur, "--help"))
        {
            *help_flag = 1;
            ++iter;
        }
        else if (!strcmp(cur, "-c") || !strcmp(cur, "/c") || !strcmp(cur, "--config"))
        {
            if (mask & (1 << 0) || iter + 1 >= argc)
            {
                return INVALID_INPUT;
            }
            
            *config_path = argv[iter + 1];
            
            mask |= 1 << 0;
            iter += 2;
        }
        else if (!strcmp(cur, "-d") || !strcmp(cur, "/d") || !strcmp(cur, "--debug"))
        {
            if (mask & (1 << 1))
            {
                return INVALID_INPUT;
            }
            
            *debug_flag = 1;
            
            mask |= 1 << 1;
            ++iter;
        }
        else if (!strcmp(cur, "-a") || !strcmp(cur, "/a") || !strcmp(cur, "--assign-system"))
        {
            if (mask & (1 << 2) || iter + 1 >= argc)
            {
                return INVALID_INPUT;
            }
            
            status_code code = parse_ullong(argv[iter+1], 10, &(config->assign_numeral_system));
            if (code)
            {
                return code;
            }
            if (config->assign_numeral_system < 2 || config->assign_numeral_system > 36)
            {
                return INVALID_BASE;
            }
            
            mask |= 1 << 2;
            iter += 2;
        }
        else if (!strcmp(cur, "-i") || !strcmp(cur, "/i") || !strcmp(cur, "--input-system"))
        {
            if (mask & (1 << 3) || iter + 1 >= argc)
            {
                return INVALID_INPUT;
            }
            
            status_code code = parse_ullong(argv[iter+1], 10, &(config->input_numeral_system));
            if (code)
            {
                return code;
            }
            if (config->input_numeral_system < 2 || config->input_numeral_system > 36)
            {
                return INVALID_BASE;
            }
            
            mask |= 1 << 3;
            iter += 2;
        }
        else if (!strcmp(cur, "-o") || !strcmp(cur, "/o") || !strcmp(cur, "--output-system"))
        {
            if (mask & (1 << 4) || iter + 1 >= argc)
            {
                return INVALID_INPUT;
            }
            
            status_code code = parse_ullong(argv[iter+1], 10, &(config->output_numeral_system));
            if (code)
            {
                return code;
            }
            if (config->output_numeral_system < 2 || config->output_numeral_system > 36)
            {
                return INVALID_BASE;
            }
            
            mask |= 1 << 4;
            iter += 2;
        }
        else if (cur[0] == '-' || cur[0] == '/')
        {
            return INVALID_FLAG;
        }
        else
        {
            if (iter + 1 != argc)
            {
                return INVALID_INPUT;
            }
            
            *input_path = cur;
            
            ++iter;
        }
    } // while end
    
    if (*input_path == NULL && !(*help_flag))
    {
        return INVALID_INPUT;
    }
    
    return OK;
}

void print_usage()
{
    printf("\nUsage: ./exe [-h] [-c <config_path>] [--debug] [-a <int>] [-i <int>] [-o <int>] <file_path>\n");
    printf("Run interpreter on <file_path>\n\n");
    printf("-h               (--help)           - show usage\n");
    printf("-c <config_path> (--config)         - set interpreter configuration\n");
    printf("-d               (--debug)          - enable debug mode\n");
    printf("-a <int>         (--assign-system)  - set numeral system for assign operator\n");
    printf("-i <int>         (--input-system)   - set numeral system for input operator\n");
    printf("-o <int>         (--output-system)  - set numeral system for output operator\n\n");
}

status_code save_config(char* config_path)
{
    if (config_path == NULL)
    {
        return NULL_ARG;
    }
    
    int flag = 1;
    char save_path[261];
    
    #ifdef _WIN32
    flag = 0;
    int len = GetTempPathA(261, save_path);
    
    if (len == 0 || len + 12 > 260)
    {
        return FILE_OPENING_ERROR;
    }
    
    strcpy(save_path + len, "4_10_cfg.txt");
    #endif // _WIN32
    
    #ifdef __linux__
    flag = 0;
    strcpy(save_path, "/tmp/4_10_cfg");
    #endif // __linux__
    
    if (flag)
    {
        return FILE_OPENING_ERROR;
    }
    
    FILE* config = fopen(config_path, "rb");    
    if (config == NULL)
    {
        return FILE_OPENING_ERROR;
    }
    
    FILE* save = fopen(save_path, "wb");    
    if (save == NULL)
    {
        fclose(config);
        return FILE_OPENING_ERROR;
    }
    
    char ch;
    while (fread(&ch, sizeof(ch), 1, config) == 1)
    {
        fwrite(&ch, sizeof(char), 1, save);
    }
    
    fclose(config);
    fclose(save);
    
    return OK;
}

status_code find_previous_config(char** config_path)
{
    if (config_path == NULL)
    {
        return NULL_ARG;
    }
    
    char save_path[261] = "";
    
    #ifdef _WIN32
    int len = GetTempPathA(261, save_path);
    
    if (len == 0 || len + 12 > 260)
    {
        return FILE_OPENING_ERROR;
    }
    
    strcpy(save_path + len, "4_10_cfg.txt");
    #endif // _WIN32
    
    #ifdef __linux__
    strcpy(save_path, "/tmp/4_10_cfg");
    #endif // __linux__
    
    if (save_path[0] == '\0')
    {
        return FILE_OPENING_ERROR;
    }
    
    FILE* test = fopen(save_path, "r");
    if (test == NULL)
    {
        return FILE_OPENING_ERROR;
    }
    
    fclose(test);
    
    *config_path = (char*) malloc(sizeof(char) * (strlen(save_path) + 1));
    if (*config_path == NULL)
    {
        return BAD_ALLOC;
    }
    
    strcpy(*config_path, save_path);
    
    return OK;
}
