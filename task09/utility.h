#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <stdio.h>

#define DEFAULT_HASH_PARAM 37
#define CONFIG_MIN_DEP_CNT 1
#define CONFIG_MAX_DEP_CNT 100
#define CONFIG_MIN_STAFF_CNT 1
#define CONFIG_MAX_STAFF_CNT 50
#define CONFIG_MIN_OVERLOAD_COEF 1.0

typedef unsigned char uchar;
typedef long long ll;
typedef unsigned long long ull;

typedef enum status_code
{
    OK,
    NULL_ARG,
    INVALID_INPUT,
    INVALID_ARG,
    INVALID_FLAG,
    INVALID_NUMBER,
    INVALID_BASE,
    INVALID_EPSILON,
    INVALID_CMD,
    INVALID_BRACKET_ORDER,
    FILE_OPENING_ERROR,
    FILE_INVALID_CONTENT,
    FILE_INVALID_CONFIG,
    FILE_KEY_DUPLICATE,
    FILE_END,
    OVERFLOW,
    UNINITIALIZED_USAGE,
    DIVISION_BY_ZERO,
    ZERO_POWERED_ZERO,
    BAD_ALLOC,
    BAD_ACCESS,
    USER_INPUT_END,
} status_code;

void print_error(status_code code, int nl_cnt);
void fprint_error(FILE* file, status_code code, int nl_cnt);

void free_all(ull cnt, ...);

typedef struct pair_prior_time
{
    unsigned prior;
    char time[21];
} pair_prior_time;

typedef struct request
{
    ull id;
    char* dep_name;
    unsigned prior;
    char time[21]; // ISO8601: YYYY-MM-DDThh:mm:ssZ
    char* txt;
} request;

status_code request_set_null(request* req);
status_code request_construct(request* req, ull id, const char* dep_name, unsigned prior, const char time[21], const char* txt);
status_code request_copy(request* req_dest, const request* req_src);
status_code request_destruct(request* req);

status_code fread_char(FILE* file, char* ch, int skip_front_spaces);
status_code fread_line(FILE* file, char** str, int skip_front_spaces);
status_code fread_word(FILE* file, char** word, int skip_front_spaces);
status_code read_line(char** str, int skip_front_spaces);
status_code sread_until(const char* src, const char* delims, int inclusive_flag, const char** end_ptr, char** str);

status_code generate_random_str(char** str, const char* alphabet, size_t max_len);

unsigned rand_32();
ull rand_64();

int is_word(const char* str);
int is_number(const char* str);
int ctoi(char ch);
char itoc(int number);
status_code tolowern(const char* src, char** res);
status_code touppern(const char* src, char** res);
status_code erase_delims(const char* src, const char* delims, char** res);
status_code parse_llong(const char* src, int base, ll* number);
status_code parse_ullong(const char* src, int base, ull* number);
status_code parse_double(const char* src, double* number);
status_code convert_ullong(ull number, int base, char res[65]);

int str_comparator(const void* ptr_1, const void* ptr_2);

int sign(ll number);
status_code add_safely(ll arg_1, ll arg_2, ll* res);
status_code sub_safely(ll arg_1, ll arg_2, ll* res);
status_code mult_safely(ll arg_1, ll arg_2, ll* res);
status_code div_safely(ll arg_1, ll arg_2, ll* res);
status_code bpow_safely(ll base, ll pow, ll* res);
status_code fbpow_safely(double base, ll pow, double* res);

size_t calc_default_str_hash(const char* str);

// time_int = 0 <=> 1970-01-01T00:00:00Z (UTC0)
status_code iso_time_validate(const char time[21]);
status_code iso_time_convert_to_int(const char time[21], ull* time_int);
status_code iso_time_convert_to_str(ull time_int, int time_zone, char time[21]);
status_code iso_time_add(const char time[21], ull add_s, char res[21]);

#endif // _UTILITY_H_