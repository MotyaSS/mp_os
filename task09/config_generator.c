#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utility.h"

typedef enum gen_stage
{
    GEN_STAGE_PQ,
    GEN_STAGE_MAP,
    GEN_STAGE_TIME,
    GEN_STAGE_HANDLE_TIME,
    GEN_STAGE_DEP_CNT,
    GEN_STAGE_DEP_INF,
    GEN_STAGE_COEF,
    GEN_STAGE_EXIT,
    GEN_STAGE_FINISH
} gen_stage;

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        printf("Usage: ./cmd_path <output>\n");
        return OK;
    }
    if (argc != 2)
    {
        print_error(INVALID_INPUT, 1);
        return INVALID_INPUT;
    }
    
    FILE* file = fopen(argv[1], "w");
    if (file == NULL)
    {
        print_error(FILE_OPENING_ERROR, 1);
        return FILE_OPENING_ERROR;
    }
    
    status_code code = OK;
    gen_stage stage = GEN_STAGE_PQ;
    
    char* input = NULL;
    char* input2 = NULL;
    char pq_name[20], map_name[40], st_time[21], end_time[21];
    ull min_handle_time, max_handle_time, dep_cnt;
    size_t* staff_cnts = NULL;
    char** dep_names = NULL;
    double overload_coef;
    
    while (!code && stage != GEN_STAGE_EXIT && stage != GEN_STAGE_FINISH)
    {
        if (stage == GEN_STAGE_PQ)
        {
            char* ds[6] = { "BinaryHeap", "LeftistHeap", "SkewHeap", "BinomialHeap", "FibonacciHeap", "Treap" };
            
            printf("Choose data structure for the priority queue\n");
            printf("1) %-20s 4) %-20s\n", "Binary heap", "Binomial heap");
            printf("2) %-20s 5) %-20s\n", "Leftist heap", "Fibonacci heap");
            printf("3) %-20s 6) %-20s\n", "Skew heap", "Treap (cartesian tree)");
            printf("(E to exit): ");
            
            code = code ? code : read_line(&input, 0);
            if (!code && strlen(input) == 1)
            {   
                int cmd = atoi(input);
                if (cmd > 0 && cmd < 7)
                {
                    strcpy(pq_name, ds[cmd - 1]);
                    stage = GEN_STAGE_MAP;
                }
                else if (input[0] == 'E')
                {
                    stage = GEN_STAGE_EXIT;
                }
            }
        }
        else if (stage == GEN_STAGE_MAP)
        {
            char* ds[4] = { "HashSet", "DynamicArray", "BinarySearchTree", "Trie" };
            
            printf("Choose data structure for the map\n");
            printf("1) %-20s 3) %-20s\n", "Dynamic array", "Hash set");
            printf("2) %-20s 4) %-20s\n", "Binary search tree", "Trie");
            printf("(E to exit): ");
            
            code = code ? code : read_line(&input, 0);
            if (!code && strlen(input) == 1)
            {
                int cmd = atoi(input);
                if (cmd > 0 && cmd < 5)
                {
                    strcpy(map_name, ds[cmd - 1]);
                    stage = GEN_STAGE_TIME;
                }
                else if (input[0] == 'E')
                {
                    stage = GEN_STAGE_EXIT;
                }
            }
        }
        else if (stage == GEN_STAGE_TIME)
        {
            printf("Enter start and end time of the simulation, each on new line (YYYY-MM-DDThh:mm:ssZ)\n");
            printf("(E to exit): ");
            code = code ? code : read_line(&input, 0);
            code = code ? code : (strcmp(input, "E") ? code : USER_INPUT_END);
            printf("%s", code ? "" : "(E to exit): ");
            code = code ? code : read_line(&input2, 0);
            code = code ? code : (strcmp(input2, "E") ? code : USER_INPUT_END);
            code = code ? code : iso_time_validate(input);
            code = code ? code : iso_time_validate(input2);
            
            if (code == INVALID_INPUT)
            {
                printf("Invalid input\n");
                code = OK;
            }
            else if (!code && strcmp(input, input2) > 0)
            {
                printf("Start time must be not greater than end time\n");
            }
            else if (!code)
            {
                strcpy(st_time, input);
                strcpy(end_time, input2);
                stage = GEN_STAGE_HANDLE_TIME;
            }
        }
        else if (stage == GEN_STAGE_HANDLE_TIME)
        {
            printf("Enter min and max handle time (in minutes), each on new line\n");
            printf("(E to exit): ");
            code = code ? code : read_line(&input, 0);
            code = code ? code : (strcmp(input, "E") ? code : USER_INPUT_END);
            printf("%s", code ? "" : "(E to exit): ");
            code = code ? code : read_line(&input2, 0);
            code = code ? code : (strcmp(input2, "E") ? code : USER_INPUT_END);
            code = code ? code : parse_ullong(input, 10, &min_handle_time);
            code = code ? code : parse_ullong(input2, 10, &max_handle_time);
            
            if (code == INVALID_INPUT || min_handle_time == 0 || max_handle_time == 0)
            {
                printf("Invalid input\n");
                code = OK;
            }
            else if (!code && min_handle_time > max_handle_time)
            {
                printf("Min handle time must be not greater than max one\n");
            }
            else if (!code)
            {
                stage = GEN_STAGE_DEP_CNT;
            }
        }
        else if (stage == GEN_STAGE_DEP_CNT)
        {
            printf("Enter department count ([%d..%d])\n", CONFIG_MIN_DEP_CNT, CONFIG_MAX_DEP_CNT);
            printf("(E to exit): ");
            code = code ? code : read_line(&input, 0);
            code = code ? code : (strcmp(input, "E") ? code : USER_INPUT_END);
            code = code ? code : parse_ullong(input, 10, &dep_cnt);
            
            if (code == INVALID_INPUT || dep_cnt < CONFIG_MIN_DEP_CNT || dep_cnt > CONFIG_MAX_DEP_CNT)
            {
                printf("Invalid input\n");
                code = OK;
            }
            else if (!code)
            {
                staff_cnts = (size_t*) calloc(dep_cnt, sizeof(size_t));
                dep_names = (char**) calloc(dep_cnt, sizeof(char*));
                
                if (staff_cnts == NULL || dep_names == NULL)
                {
                    code = BAD_ALLOC;
                }
                
                stage = GEN_STAGE_DEP_INF;
            }
        }
        else if (stage == GEN_STAGE_DEP_INF)
        {
            printf("Enter department staff size ([%d..%d]) and names\n", CONFIG_MIN_STAFF_CNT, CONFIG_MAX_STAFF_CNT);
            
            for (size_t i = 0; !code && i < dep_cnt; ++i)
            {
                printf("name-%llu (E to exit): ", (ull) i);
                code = code ? code : read_line(&dep_names[i], 0);
                code = code ? code : (strcmp(dep_names[i], "E") ? code : USER_INPUT_END);
                
                for (size_t j = 0; !code && j < i; ++j)
                {
                    if (!strcmp(dep_names[i], dep_names[j]))
                    {
                        printf("This name has already been entered\n");
                        code = INVALID_INPUT;
                    }
                }
                
                if (code == INVALID_INPUT)
                {
                    code = OK;
                    --i;
                }
            }
            
            for (size_t i = 0; !code && i < dep_cnt; ++i)
            {
                ull staff_cnt_tmp = 0;
                printf("staff-%llu (E to exit): ", (ull) i);
                code = code ? code : read_line(&input, 0);
                code = code ? code : (strcmp(input, "E") ? code : USER_INPUT_END);
                code = code ? code : parse_ullong(input, 10, &staff_cnt_tmp);
                staff_cnts[i] = (size_t) staff_cnt_tmp;
                free(input);
                input = NULL;
                
                if (code == INVALID_INPUT || staff_cnts[i] < CONFIG_MIN_STAFF_CNT || staff_cnts[i] > CONFIG_MAX_STAFF_CNT)
                {
                    printf("Invalid input\n");
                    code = OK;
                    --i;
                }
            }
            
            if (!code)
            {
                stage = GEN_STAGE_COEF;
            }
        }
        else if (stage == GEN_STAGE_COEF)
        {
            printf("Enter overload coefficient (not less than %lf)\n", CONFIG_MIN_OVERLOAD_COEF);
            code = code ? code : read_line(&input, 0);
            code = code ? code : parse_double(input, &overload_coef);
            
            if (!code && overload_coef - CONFIG_MIN_OVERLOAD_COEF < -1e9)
            {
                code = INVALID_INPUT;
            }
            
            if (code == INVALID_INPUT)
            {
                printf("Invalid input\n");
                code = OK;
            }
            else
            {
                stage = GEN_STAGE_FINISH;
            }
        }
        
        if (code == USER_INPUT_END)
        {
            code = OK;
            stage = GEN_STAGE_EXIT;
        }
        
        free_all(2, input, input2);
        input = input2 = NULL;
        
        printf("\n");
    }
    
    if (stage == GEN_STAGE_FINISH)
    {
        fprintf(file, "%s\n%s\n%s\n%s\n%llu\n%llu\n%llu\n", pq_name, map_name, st_time, end_time,
                min_handle_time, max_handle_time, dep_cnt);
        
        for (size_t i = 0; i + 1 < dep_cnt; ++i)
        {
            fprintf(file, "%s ", dep_names[i]);
        }
        fprintf(file, "%s\n", dep_names[dep_cnt - 1]);
        
        for (size_t i = 0; i + 1 < dep_cnt; ++i)
        {
            fprintf(file, "%llu ", (ull) staff_cnts[i]);
        }
        fprintf(file, "%llu\n", (ull) staff_cnts[dep_cnt - 1]);
        
        fprintf(file, "%lf", overload_coef);
        printf("Config file has been created\n");
    }
    
    if (dep_names != NULL)
    {
        for (size_t i = 0; i < dep_cnt; ++i)
        {
            free(dep_names[i]);
        }
    }
    
    free_all(2, staff_cnts, dep_names);
    fclose(file);
}
