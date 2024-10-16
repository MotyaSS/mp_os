#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "utility.h"
#include "department.h"

typedef struct Logger
{
    FILE* file;
} Logger;

status_code logger_set_null(Logger* log);
status_code logger_construct(Logger* log, const char* path);
status_code logger_destruct(Logger* log);

status_code logger_log(Logger* log, const char time[21], department_message msg);
status_code logger_multi_log(Logger* log, const char time[21], size_t msg_cnt, const department_message* msgs);

#endif // _LOGGER_H_