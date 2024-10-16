#include <stdlib.h>
#include <stdio.h>

#include "logger.h"

status_code logger_set_null(Logger* log)
{
    if (log == NULL)
    {
        return NULL_ARG;
    }
    
    log->file = NULL;
    
    return OK;
}

status_code logger_construct(Logger* log, const char* path)
{
    if (log == NULL || path == NULL)
    {
        return NULL_ARG;
    }
    
    log->file = fopen(path, "w");
    if (log->file == NULL)
    {
        return FILE_OPENING_ERROR;
    }
    
    return OK;
}

status_code logger_destruct(Logger* log)
{
    if (log == NULL)
    {
        return NULL_ARG;
    }
    
    if (log->file != NULL)
    {
        fclose(log->file);
    }
    
    return logger_set_null(log);
}


status_code logger_log(Logger* log, const char time[21], department_message msg)
{
    if (log == NULL || time == NULL)
    {
        return NULL_ARG;
    }
    
    switch (msg.code)
    {
        case NEW_REQUEST:
        {
            if (msg.dep_name == NULL)
            {
                return INVALID_INPUT;
            }
            
            fprintf(log->file, "[%s] [NEW_REQUEST]: Request %u was received by dep. %s.\n",
                    time, msg.req_id, msg.dep_name);
            
            break;
        }
        case REQUEST_HANDLING_STARTED:
        {
            if (msg.oper_name == NULL || msg.dep_name == NULL)
            {
                return INVALID_INPUT;
            }
            
            fprintf(log->file, "[%s] [REQUEST_HANDLING_STARTED]: Handling of request %u was started in dep. %s by operator %s.\n",
                    time, msg.req_id, msg.dep_name, msg.oper_name);
            
            break;
        }
        case REQUEST_HANDLING_FINISHED:
        {
            if (msg.oper_name == NULL)
            {
                return INVALID_INPUT;
            }
            
            fprintf(log->file, "[%s] [REQUEST_HANDLING_FINISHED]: Handling of request %u was finished in by operator %s in %u minutes.\n",
                    time, msg.req_id, msg.oper_name, msg.handling_time);
            
            break;
        }
        case DEPARTMENT_OVERLOADED:
        {
            if (msg.transfer_dep_name == NULL)
            {
                fprintf(log->file, "[%s] [DEPARTMENT_OVERLOADED]: Department %s was overloaded after receiving request %u. ",
                        time, msg.dep_name, msg.req_id);
                fprintf(log->file, "No transfer can be executed.\n");
            }
            else
            {
                fprintf(log->file, "[%s] [DEPARTMENT_OVERLOADED]: Department %s was overloaded after receiving request %u. ",
                        time, msg.dep_name, msg.req_id);
                fprintf(log->file, "Its request was transfered to department %s\n",
                        msg.transfer_dep_name);
            }
            break;
        }
    }
    
    return OK;
}

status_code logger_multi_log(Logger* log, const char time[21], size_t msg_cnt, const department_message* msgs)
{
    if (log == NULL || time == NULL || msgs == NULL)
    {
        return NULL_ARG;
    }
    
    for (size_t i = 0; i < msg_cnt; ++i)
    {
        status_code code = logger_log(log, time, msgs[i]);
        if (code)
        {
            return code;
        }
    }
    
    return OK;
}
