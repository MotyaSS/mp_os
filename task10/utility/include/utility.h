#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <stdio.h>
#include <stdint.h>

#define DEFAULT_HASH_PARAM 37

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
    MATH_OVERFLOW,
    UNINITIALIZED_USAGE,
    DIVISION_BY_ZERO,
    ZERO_POWERED_ZERO,
    BAD_ALLOC,
    BAD_ACCESS,
    USER_INPUT_END,
    SHUTDOWN,
} status_code;

void print_error(status_code code, int nl_cnt);
void fprint_error(FILE* file, status_code code, int nl_cnt);


#define OPERATION_COUNT 13

typedef enum
{
    OP_NOT,
    OP_INPUT,
    OP_OUTPUT,
    OP_ADD,
    OP_MULT,
    OP_SUB,
    OP_POW,
    OP_DIV,
    OP_REM,
    OP_XOR,
    OP_AND,
    OP_OR,
    OP_ASSIGN
} operation;

typedef enum
{
    OP_ORDER_PREFIX,
    OP_ORDER_INFIX,
    OP_ORDER_POSTFIX
} operation_order;

typedef enum
{
    ASSIGN_LEFT,
    ASSIGN_RIGHT
} assign_order;

typedef enum
{
    UNARY_PREFIX,
    UNARY_POSTFIX
} unary_operation_order;

typedef enum
{
    BINARY_PREFIX,
    BINARY_INFIX,
    BINARY_POSTFIX
} binary_operation_order;

typedef struct
{
    ull assign_numeral_system;
    ull input_numeral_system;
    ull output_numeral_system;
    assign_order assign;
    unary_operation_order unary;
    binary_operation_order binary;
    char* aliases[OPERATION_COUNT];
} config_data;

void config_destruct(config_data config);
int is_operation_unary(operation op);
int get_operation_precedence(operation op);
status_code parse_operation(const char* op_name, operation* op);
status_code parse_operation_alias(const char* op_alias, config_data config, operation* op);
status_code parse_operation_char(char op_char, operation* op);
status_code convert_operation_to_char(operation op, char* ch);
status_code calc_operation_uint32(operation op, uint32_t left, uint32_t right, uint32_t* res);

status_code validate_var_name(const char* var_name);

void free_all(ull cnt, ...);

status_code skip_spaces(FILE* file);
status_code skip_line(FILE* file);
status_code skip_multi_line_comment(FILE* file, char comment_begin, char comment_end);
status_code fread_char(FILE* file, char* ch, int skip_front_spaces);
status_code fread_line(FILE* file, char** line, int skip_front_spaces);
status_code fread_line_with_comments(FILE* file, char** line, int skip_front_spaces, char line_comment, char comment_begin, char comment_end);
status_code fread_word(FILE* file, char** word, int skip_front_spaces);
status_code read_line(char** line, int skip_front_spaces);
status_code sread_until(const char* src, const char* delims, int inclusive_flag, const char** end_ptr, char** str);
status_code sread_while(const char* src, const char* whitelist, const char** end_ptr, char** str);

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
status_code parse_uint32(const char* src, int base, uint32_t* number);
status_code parse_zeckendorf_uint32(const char* src, uint32_t* number);
status_code parse_roman_uint32(const char* src, uint32_t* number);
status_code parse_llong(const char* src, int base, ll* number);
status_code parse_ullong(const char* src, int base, ull* number);
status_code parse_double(const char* src, double* number);
status_code convert_uint32(uint32_t number, int base, char res[33]);
status_code convert_ullong(ull number, int base, char res[65]);
status_code remove_leading_zero(char* str);

int str_comparator(const void* ptr_1, const void* ptr_2);

int sign(ll number);
status_code add_uint32_safely(uint32_t arg_1, uint32_t arg_2, uint32_t* res);
status_code sub_uint32_safely(uint32_t arg_1, uint32_t arg_2, uint32_t* res);
status_code mult_uint32_safely(uint32_t arg_1, uint32_t arg_2, uint32_t* res);
status_code div_uint32_safely(uint32_t arg_1, uint32_t arg_2, uint32_t* res);
status_code bpow_uint32_safely(uint32_t base, uint32_t pow, uint32_t* res);

size_t calc_default_str_hash(const char* str);

#endif // _UTILITY_H_