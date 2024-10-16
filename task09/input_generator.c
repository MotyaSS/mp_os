#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "utility.h"
#include "readers.h"

int rand_comp(const void* lhs, const void* rhs)
{
    return rand() & 1;
}

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        printf("Usage: ./cmd_path <max priority> <config path> <output>\n");
        return OK;
    }
    if (argc != 4)
    {
        print_error(INVALID_INPUT, 1);
        return INVALID_INPUT;
    }
    
    srand(time(NULL));
    
    status_code code = OK;
    ull max_prior = 0;
    char st_time[21];
    char end_time[21];
    ull st_time_int = 0;
    ull end_time_int = 0;
    size_t dep_cnt = 0;
    char** dep_names = NULL;
    
    size_t req_cnt = rand_32() % (1 << 10);
    unsigned* req_ids = (unsigned*) malloc(req_cnt * sizeof(unsigned));
    if (req_ids == NULL)
    {
        return BAD_ALLOC;
    }
    
    for (size_t i = 0; i < req_cnt; ++i)
    {
        req_ids[i] = i + 1;
    }
    qsort(req_ids, req_cnt, sizeof(unsigned), rand_comp);
    
    code = code ? code : parse_ullong(argv[1], 10, &max_prior);
    code = code ? code : read_config(argv[2], 1e-9, NULL, NULL, st_time, end_time,
                                NULL, NULL, &dep_cnt, &dep_names, NULL, NULL);
    
    code = code ? code : iso_time_convert_to_int(st_time, &st_time_int);
    code = code ? code : iso_time_convert_to_int(end_time, &end_time_int);
    
    FILE* file = fopen(argv[3], "w");
    if (file == NULL)
    {
        code = code ? code : FILE_OPENING_ERROR;
    }
    
    for (size_t i = 0; !code && i < req_cnt; ++i)
    {
        char rand_st_time[21];
        unsigned rand_prior = max_prior == (unsigned) -1 ? rand_32() : (rand_32() % max_prior);
        char* rand_dep_name = dep_names[rand_64() % dep_cnt];
        char* rand_message = NULL;
        
        ull rand_st_time_int = st_time_int + rand_64() % (end_time_int - st_time_int + 1);
        code = code ? code : iso_time_convert_to_str(rand_st_time_int, 3, rand_st_time);
        code = code ? code : generate_random_str(&rand_message, "a-zA-Z0-9 ", -1);
        
        if (!code)
        {
            fprintf(file, "%s %09u %u %s \"%s\"\n", rand_st_time, req_ids[i], rand_prior, rand_dep_name, rand_message);
        }
        
        free(rand_message);
    }
    
    for (size_t i = 0; i < dep_cnt; ++i)
    {
        free(dep_names[i]);
    }
    free_all(2, req_ids, dep_names);
    fclose(file);
    
    return code;
}
