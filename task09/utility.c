#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>

typedef long long ll;
typedef unsigned long long ull;

void print_error(status_code code, int nl_cnt)
{
    fprint_error(stdout, code, nl_cnt);
}

void fprint_error(FILE* file, status_code code, int nl_cnt)
{
    switch (code)
    {
        case OK:
            return;
        case NULL_ARG:
            fprintf(file, "Null argument");
            break;
        case INVALID_INPUT:
            fprintf(file, "Invalid input");
            break;
        case INVALID_ARG:
            fprintf(file, "Invalid argument");
            break;
        case INVALID_FLAG:
            fprintf(file, "Invalid flag");
            break;
        case INVALID_NUMBER:
            fprintf(file, "Invalid number");
            break;
        case INVALID_BASE:
            fprintf(file, "Invalid base");
            break;
        case INVALID_EPSILON:
            fprintf(file, "Invalid epsilon");
            break;
        case INVALID_CMD:
            fprintf(file, "Invalid command");
            break;
        case INVALID_BRACKET_ORDER:
            fprintf(file, "Invalid bracket order");
            break;
        case FILE_OPENING_ERROR:
            fprintf(file, "File cannot be opened");
            break;
        case FILE_INVALID_CONTENT:
            fprintf(file, "File content is invalid");
            break;
        case FILE_INVALID_CONFIG:
            fprintf(file, "Configuration file is invalid");
            break;
        case FILE_KEY_DUPLICATE:
            fprintf(file, "File contains key duplicates");
            break;
        case FILE_END:
            fprintf(file, "Unexpected end of file was found");
            break;
        case UNINITIALIZED_USAGE:
            fprintf(file, "Attempting to access uninitialized variable");
            break;
        case DIVISION_BY_ZERO:
            fprintf(file, "Attempting to divide by zero");
            break;
        case ZERO_POWERED_ZERO:
            fprintf(file, "Attempting to raize zero in the power zero");
            break;
        case BAD_ALLOC:
            fprintf(file, "Memory lack occurred");
            break;
        case BAD_ACCESS:
            fprintf(file, "Attempting to access incorrect memory");
            break;
        default:
            fprintf(file, "Unexpected error occurred");
            break;
    }
    for (int i = 0; i < nl_cnt; ++i)
    {
        printf("\n");
    }
}


void free_all(ull cnt, ...)
{
	va_list va;
	va_start(va, cnt);
	for (ull i = 0; i < cnt; ++i)
	{
		void* ptr = va_arg(va, void*);
		free(ptr);
	}
	va_end(va);
}


status_code request_set_null(request* req)
{
    if (req == NULL)
    {
        return NULL_ARG;
    }
    
    req->dep_name = NULL;
    req->id = 0;
    req->prior = 0;
    req->time[0] = '\0';
    req->txt = NULL;
    
    return OK;
}

status_code request_construct(request* req, ull id, const char* dep_name, unsigned prior, const char time[21], const char* txt)
{
    if (req == NULL)
    {
        return NULL_ARG;
    }
    
    request_set_null(req);
    
    if (dep_name != NULL)
    {
        req->dep_name = (char*) malloc(sizeof(char) * (strlen(dep_name) + 1));
        if (req->dep_name == NULL)
        {
            return BAD_ALLOC;
        }
        strcpy(req->dep_name, dep_name);
    }
    
    if (txt != NULL)
    {
        req->txt = (char*) malloc(sizeof(char) * (strlen(txt) + 1));
        if (req->txt == NULL)
        {
            free(req->dep_name);
            req->dep_name = NULL;
            return BAD_ALLOC;
        }
        strcpy(req->txt, txt);
    }
    
    if (time != NULL)
    {
        strcpy(req->time, time);
    }
    
    req->id = id;
    req->prior = prior;
    
    return OK;
}

status_code request_copy(request* req_dest, const request* req_src)
{
    if (req_dest == NULL || req_src == NULL)
    {
        return NULL_ARG;
    }
    
    return request_construct(req_dest, req_src->id, req_src->dep_name, req_src->prior, req_src->time, req_src->txt);
}

status_code request_destruct(request* req)
{
    if (req == NULL)
    {
        return OK;
    }
    
    free(req->dep_name);
    free(req->txt);
    return request_set_null(req);
}

status_code fread_char(FILE* file, char* ch_res, int skip_front_spaces)
{
    if (file == NULL || ch_res == NULL)
    {
        return NULL_ARG;
    }
    char ch = getc(file);
    while (skip_front_spaces && isspace(ch) && !feof(file))
    {
        ch = getc(file);
    }
    if (feof(file))
    {
        return FILE_END;
    }
    *ch_res = ch;
    return OK;
}

status_code fread_line(FILE* file, char** line, int skip_front_spaces)
{
    if (file == NULL || line == NULL)
    {
        return NULL_ARG;
    }
    ull iter = 0;
    ull size = 4;
    char* line_tmp = (char*) malloc(sizeof(char) * size);
    if (line_tmp == NULL)
    {
        return BAD_ALLOC;
    }
    char ch = getc(file);
    while (skip_front_spaces && isspace(ch) && !feof(file))
    {
        ch = getc(file);
    }
    if (feof(file))
    {
        free(line_tmp);
        return FILE_END;
    }
    while (!feof(file) && ch != '\n')
    {
        if (iter + 1 == size)
        {
            size *= 2;
            char* tmp = realloc(line_tmp, size);
            if (tmp == NULL)
            {
                free(line_tmp);
                return BAD_ALLOC;
            }
            line_tmp = tmp;
        }
        line_tmp[iter++] = ch;
        ch = getc(file);
    }
    line_tmp[iter] = '\0';
    *line = line_tmp;
    return OK;
}

status_code fread_word(FILE* file, char** word, int skip_front_spaces)
{
    if (file == NULL || word == NULL)
    {
        return NULL_ARG;
    }
    ull iter = 0;
    ull size = 4;
    char* word_tmp = (char*) malloc(sizeof(char) * size);
    if (word_tmp == NULL)
    {
        return BAD_ALLOC;
    }
    char ch = getc(file);
    while (skip_front_spaces && isspace(ch) && !feof(file))
    {
        ch = getc(file);
    }
    if (feof(file))
    {
        free(word_tmp);
        return FILE_END;
    }
    while (!feof(file) && !isspace(ch))
    {
        if (iter + 1 == size)
        {
            size *= 2;
            char* tmp = realloc(word_tmp, size);
            if (tmp == NULL)
            {
                free(word_tmp);
                return BAD_ALLOC;
            }
            word_tmp = tmp;
        }
        word_tmp[iter++] = ch;
        ch = getc(file);
    }
    word_tmp[iter] = '\0';
    *word = word_tmp;
    return OK;
}

status_code read_line(char** str, int skip_front_spaces)
{
    return fread_line(stdin, str, skip_front_spaces);
}

status_code sread_until(const char* src, const char* delims, int inclusive_flag, const char** end_ptr, char** str)
{
    if (src == NULL || delims == NULL || str == NULL)
    {
        return NULL_ARG;
    }
    ull cnt = 0;
    ull size = 4;
    char* str_tmp = (char*) malloc(sizeof(char) * size);
    if (str_tmp == NULL)
    {
        return BAD_ALLOC;
    }
    char flags[256];
    for (ull i = 0; i < 256; ++i)
    {
        flags[i] = 1;
    }
    for (ull i = 0; delims[i]; ++i)
    {
        flags[(int) delims[i]] = 0;
    }
    const char* ptr = src;
    while (flags[(int) *ptr] && *ptr != '\0')
    {
        if (cnt + 2 == size)
        {
            size *= 2;
            char* tmp = realloc(str_tmp, sizeof(char) * size);
            if (tmp == NULL)
            {
                free(str_tmp);
                return BAD_ALLOC;
            }
            str_tmp = tmp;
        }
        str_tmp[cnt++] = *ptr;
        ++ptr;
    }
    if (end_ptr != NULL)
    {
        *end_ptr = ptr;
    }
    if (inclusive_flag && *ptr != '\0')
    {
        str_tmp[cnt++] = *ptr;
    }
    str_tmp[cnt] = '\0';
    *str = str_tmp;
    return OK;
}


status_code generate_random_str(char** str, const char* alphabet, size_t max_len)
{
	if (str == NULL)
	{
		return NULL_ARG;
	}
    
	char flags[256];
    memset(flags, 0, sizeof(char) * 256);
    
    for (size_t i = 0; alphabet[i]; ++i)
    {
        if (i > 0 && alphabet[i] == '-')
        {
            if (alphabet[i+1] == '\0' || alphabet[i-1] > alphabet[i+1])
            {
                return INVALID_INPUT;
            }
            
            for (size_t j = alphabet[i-1]; j <= alphabet[i+1]; ++j)
            {
                flags[(int) j] = 1;
            }
        }
        else
        {
            flags[(int) alphabet[i]] = 1;
        }
    }
    
    size_t len = 0;
	char symbols[256];
    char* ptr = symbols;
    
    for (size_t i = 0; i < 256; ++i)
    {
        if (flags[i])
        {
            *ptr++ = (char) i;
        }
    }
    
    *ptr = '\0';
    len = ptr - symbols;
    
	size_t iter = 0;
	size_t size = 4;
	*str = (char*) malloc(sizeof(char) * size);
	if (*str == NULL)
	{
		return BAD_ALLOC;
	}
	
	char ch = symbols[rand() % (len + 1)];
	while ((ch != '\0' || iter == 0) && iter < max_len)
	{
		while (iter == 0 && ch == '\0')
		{
			ch = symbols[rand() % (len + 1)];
		}
		if (iter > size - 2)
		{
			size *= 2;
			char* temp_str = realloc(*str, size);
			if (temp_str == NULL)
			{
				free(*str);
				return BAD_ALLOC;
			}
			*str = temp_str;
		}
		(*str)[iter++] = ch;
		ch = symbols[rand() % (len + 1)];
	}
	(*str)[iter] = '\0';
	return OK;
}


unsigned rand_32()
{
	unsigned x = rand() & 255;
	x |= (rand() & 255) << 8;
	x |= (rand() & 255) << 16;
	x |= (rand() & 255) << 24;
	return x;
}

ull rand_64()
{
	ull x = (ull) rand() & 255;
	x |= ((ull) rand() & 255) << 8;
	x |= ((ull) rand() & 255) << 16;
	x |= ((ull) rand() & 255) << 24;
	x |= ((ull) rand() & 255) << 32;
	x |= ((ull) rand() & 255) << 40;
	x |= ((ull) rand() & 255) << 48;
	x |= ((ull) rand() & 255) << 56;
	return x;
}


int is_word(const char* str)
{
    if (str == NULL || str[0] == '\0')
    {
        return 0;
    }
    for (ull i = 0; str[i]; ++i)
    {
        if (!isalpha(str[i]))
        {
            return 0;
        }
    }
    return 1;
}

int is_number(const char* str)
{
    if (str == NULL || str[0] == '\0')
    {
        return 0;
    }
    if (!isdigit(str[0]) && str[0] != '-' && str[0] != '+')
    {
        return 0;
    }
    if ((str[0] == '-' || str[0] == '+') && str[1] == '\0')
    {
        return 0;
    } 
    for (ull i = 1; str[i]; ++i)
    {
        if (!isdigit(str[i]))
        {
            return 0;
        }
    }
    return 1;
}

int ctoi(char ch)
{
    if (isdigit(ch))
    {
        return ch - '0';
    }
    else if (isalpha(ch))
    {
        return toupper(ch) - 'A' + 10;
    }
    return -1;
}

char itoc(int number)
{
    if (number >= 0 && number < 10)
    {
        return '0' + number;
    }
    else if (number >= 10 && number < 36)
    {
        return 'A' + number - 10;
    }
    return '\0';
}

status_code tolowern(const char* src, char** res)
{
    if (src == NULL || res == NULL)
    {
        return NULL_ARG;
    }
    *res = (char*) malloc(sizeof(char) * (strlen(src) + 1));
    if (*res == NULL)
    {
        return BAD_ALLOC;
    }
    ull i = 0;
    for (; src[i]; ++i)
    {
        (*res)[i] = isalpha(src[i]) ? tolower(src[i]) : src[i];
    }
    (*res)[i] = '\0';
    return OK;
}

status_code touppern(const char* src, char** res)
{
    if (src == NULL || res == NULL)
    {
        return NULL_ARG;
    }
    *res = (char*) malloc(sizeof(char) * (strlen(src) + 1));
    if (*res == NULL)
    {
        return BAD_ALLOC;
    }
    ull i = 0;
    for (; src[i]; ++i)
    {
        (*res)[i] = isalpha(src[i]) ? toupper(src[i]) : src[i];
    }
    (*res)[i] = '\0';
    return OK;
}

status_code erase_delims(const char* src, const char* delims, char** res)
{
    if (src == NULL || delims == NULL || res == NULL)
    {
        return NULL_ARG;
    }
    ull cnt = 0;
    ull size = 4;
    char* str_tmp = (char*) malloc(sizeof(char) * size);
    if (str_tmp == NULL)
    {
        return BAD_ALLOC;
    }
    char flags[256];
    for (ull i = 0; i < 256; ++i)
    {
        flags[i] = 1;
    }
    for (ull i = 0; delims[i]; ++i)
    {
        flags[(int) delims[i]] = 0;
    }
    const char* ptr = src;
    while (*ptr != '\0')
    {
        if (cnt + 2 == size && flags[(int) *ptr])
        {
            size *= 2;
            char* tmp = realloc(str_tmp, sizeof(char) * size);
            if (tmp == NULL)
            {
                free(str_tmp);
                return BAD_ALLOC;
            }
            str_tmp = tmp;
        }
        if (flags[(int) *ptr])
        {
            str_tmp[cnt++] = *ptr;
        }
        ++ptr;
    }
    str_tmp[cnt] = '\0';
    *res = str_tmp;
    return OK;
}

status_code parse_llong(const char* src, int base, ll* number)
{
    if (src == NULL || number == NULL)
    {
        return NULL_ARG;
    }
    if (base < 0 || base == 1 || base > 36)
    {
        return INVALID_BASE;
    }
    if (src[0] == '\0')
    {
        return INVALID_INPUT;
    }
    errno = 0;
    char* ptr;
    *number = strtoll(src, &ptr, base);
    if (*ptr != '\0')
    {
        return INVALID_INPUT;
    }
    if (errno == ERANGE)
    {
        return OVERFLOW;
    }
    return OK;
}

status_code parse_ullong(const char* src, int base, ull* number)
{
    if (src == NULL || number == NULL)
    {
        return NULL_ARG;
    }
    if (base < 0 || base == 1 || base > 36)
    {
        return INVALID_BASE;
    }
    if (src[0] == '\0')
    {
        return INVALID_INPUT;
    }
    errno = 0;
    char* ptr;
    *number = strtoull(src, &ptr, base);
    if (*ptr != '\0')
    {
        return INVALID_INPUT;
    }
    if (errno == ERANGE)
    {
        return OVERFLOW;
    }
    return OK;
}

status_code parse_double(const char* src, double* number)
{
    if (src == NULL || number == NULL)
    {
        return NULL_ARG;
    }
    if (src[0] == '\0')
    {
        return INVALID_INPUT;
    }
    char* ptr;
    *number = strtod(src, &ptr);
    if (*ptr != '\0')
    {
        return INVALID_INPUT;
    }
    return OK;
}

status_code convert_ullong(ull number, int base, char res[65])
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if (base < 2 || base > 36)
    {
        return INVALID_BASE;
    }
    for (ll i = 63; i >= 0; --i)
    {
        res[i] = itoc(number % base);
        number /= base;
    }
    res[64] = '\0';
    return OK;
}

int str_comparator(const void* ptr_1, const void* ptr_2)
{
    const char** str_ptr_1 = (const char**) ptr_1;
    const char** str_ptr_2 = (const char**) ptr_2;
    return strcmp(*str_ptr_1, *str_ptr_2);
}


int sign(ll number)
{
    return number == 0 ? 0 : (number > 0 ? 1 : -1);
}

status_code add_safely(ll arg_1, ll arg_2, ll* res)
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if (arg_2 > 0 && (arg_1 > LLONG_MAX - arg_2))
    {
        return OVERFLOW;
    }
    if (arg_2 < 0 && (arg_2 < LLONG_MIN - arg_2))
    {
        return OVERFLOW;
    }
    *res = arg_1 + arg_2;
    return OK;
}

status_code sub_safely(ll arg_1, ll arg_2, ll* res)
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if (arg_2 > 0 && (arg_2 < LLONG_MIN + arg_2))
    {
        return OVERFLOW;
    }
    if (arg_2 < 0 && (arg_1 > LLONG_MAX + arg_2))
    {
        return OVERFLOW;
    }
    *res = arg_1 - arg_2;
    return OK;
}

status_code mult_safely(ll arg_1, ll arg_2, ll* res)
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if ((arg_1 == LLONG_MIN && arg_2 != 1) || (arg_2 == LLONG_MIN && arg_1 != 1))
    {
        return OVERFLOW;
    }
    if (llabs(arg_1) > LLONG_MAX / llabs(arg_2))
    {
        return OVERFLOW;
    }
    *res = arg_1 * arg_2;
    return OK;
}

status_code div_safely(ll arg_1, ll arg_2, ll* res)
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if (arg_2 == 0)
    {
        return DIVISION_BY_ZERO;
    }
    *res = arg_1 / arg_2;
    return OK;
}

status_code bpow_safely(ll base, ll pow, ll* res)
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if (pow < 0)
    {
        return INVALID_INPUT;
    }
    if (base == 0 && pow == 0)
    {
        return ZERO_POWERED_ZERO;
    }
    ll res_tmp = 1;
    ll mult = base;
    while (pow > 0)
    {
        if (pow & 1)
        {
            status_code err_code = mult_safely(res_tmp, mult, &res_tmp);
            if (err_code)
            {
                return err_code;
            }
        }
        mult *= mult;
        pow >>= 1;
    }
    *res = res_tmp;
    return OK;
}

status_code fbpow_safely(double base, ll pow, double* res)
{
    if (res == NULL)
    {
        return NULL_ARG;
    }
    if (base == 0 && pow == 0)
    {
        return ZERO_POWERED_ZERO;
    }
    if (pow == 0)
    {
        *res = 1;
        return OK;
    }
    int pow_sign = sign(pow);
    pow = llabs(pow);
    double res_tmp = 1;
    double mult = base;
    while (pow > 0)
    {
        if (pow & 1)
        {
            res_tmp *= mult;
        }
        mult *= mult;
        pow >>= 1;
    }
    *res = pow_sign == 1 ? res_tmp : 1.0 / res_tmp;
    return OK;
}

size_t calc_default_str_hash(const char* str)
{
    if (str == NULL)
    {
        return 0;
    }
    size_t res = 0;
    for (ull i = 0; str[i]; ++i)
    {
        res *= DEFAULT_HASH_PARAM;
        res += ctoi(str[i]) + 1;
    }
    return res;
}

int is_leap_year(int year)
{
	return year % 400 == 0 && year % 100 != 0 && year % 4 == 0;
}


// ISO8601: YYYY-MM-DDThh:mm:ssZ
status_code iso_time_validate(const char time[21])
{
    if (time == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    char copy[21];
    strcpy(copy, time);
    
    if (strlen(time) != 20 || time[4] != '-' || time[7] != '-' || time[10] != 'T' ||
            time[13] != ':' || time[16] != ':' || time[19] != 'Z' || time[20] != '\0')
    {
        return INVALID_INPUT;
    }
    
    ull year, month, day, hour, min, sec;
    code = code ? code : parse_ullong(strtok(copy, "-"), 10, &year);
    code = code ? code : parse_ullong(strtok(NULL, "-"), 10, &month);
    code = code ? code : parse_ullong(strtok(NULL, "T"), 10, &day);
    code = code ? code : parse_ullong(strtok(NULL, ":"), 10, &hour);
    code = code ? code : parse_ullong(strtok(NULL, ":"), 10, &min);
    code = code ? code : parse_ullong(strtok(NULL, "Z"), 10, &sec);
    
    if (code)
    {
        return code;
    }
    
    ull mday_cnt[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    ull leap_mday_cnt[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    
    ull mx_day = is_leap_year(year) ? leap_mday_cnt[month-1] : mday_cnt[month-1];
    
    if (year < 1970 || year > 3000 || month < 1 || month > 12 || day < 1 || day > mx_day ||
            hour > 23 || min > 59 || sec > 59)
    {
        return INVALID_INPUT;
    }
    
    return OK;
}

status_code iso_time_convert_to_int(const char time[21], ull* time_int)
{
    if (time == NULL || time_int == NULL)
    {
        return NULL_ARG;
    }
    
    char time_tmp[21];
    strcpy(time_tmp, time);
    
    struct tm t = { 0 };
    t.tm_sec = atoi(time_tmp + 17);
    t.tm_min = atoi(time_tmp + 14);
    t.tm_hour = atoi(time_tmp + 11);
    t.tm_mday = atoi(time_tmp + 8);
    t.tm_mon = atoi(time_tmp + 5) - 1;
    t.tm_year = atoi(time_tmp) - 1900;
    
    *time_int = (ull) mktime(&t);
    
    return OK;
}

status_code iso_time_convert_to_str(ull time_int, int time_zone, char time[21])
{
    if (time == NULL)
    {
        return NULL_ARG;
    }
    
    struct tm t = { 0 };
    t.tm_sec = time_int;
    t.tm_min = 0;
    t.tm_hour = time_zone;
    t.tm_mday = 1;
    t.tm_mon = 0;
    t.tm_year = 70;
    
    mktime(&t);
    
    sprintf(time, "%04d-%02d-%02dT%02d:%02d:%02dZ", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
    
    return OK;
}

status_code iso_time_add(const char time[21], ull add_s, char res[21])
{
    if (time == NULL || res == NULL)
    {
        return NULL_ARG;
    }
    
    char time_tmp[21];
    strcpy(time_tmp, time);
    time_tmp[4] = time_tmp[7] = time_tmp[10] = time_tmp[13] = time_tmp[16] = time_tmp[19] = '\0';
    struct tm t = { 0 };
    t.tm_sec = atoi(time_tmp + 17);
    t.tm_min = atoi(time_tmp + 14);
    t.tm_hour = atoi(time_tmp + 11);
    t.tm_mday = atoi(time_tmp + 8);
    t.tm_mon = atoi(time_tmp + 5) - 1;
    t.tm_year = atoi(time_tmp) - 1900;
    
    t.tm_sec += add_s;
    mktime(&t);
    
    res[0] = '0' + ((t.tm_year + 1900) / 1000);
    res[1] = '0' + ((t.tm_year + 1900) / 100 % 10);
    res[2] = '0' + ((t.tm_year + 1900) / 10 % 10);
    res[3] = '0' + ((t.tm_year + 1900) / 1 % 10);
    
    res[5] = '0' + ((t.tm_mon + 1) / 10);
    res[6] = '0' + ((t.tm_mon + 1) % 10);
    
    res[8] = '0' + (t.tm_mday / 10);
    res[9] = '0' + (t.tm_mday % 10);
    
    res[11] = '0' + (t.tm_hour / 10);
    res[12] = '0' + (t.tm_hour % 10);
    
    res[14] = '0' + (t.tm_min / 10);
    res[15] = '0' + (t.tm_min % 10);
    
    res[17] = '0' + (t.tm_sec / 10);
    res[18] = '0' + (t.tm_sec % 10);
    
    res[4] = res[7] = '-';
    res[10] = 'T';
    res[13] = res[16] = ':';
    res[19] = 'Z';
    res[20] = '\0';
    
    return OK;
}
