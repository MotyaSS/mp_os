#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "department.h"

status_code generate_names(Department* dep)
{
    if (dep == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    
    for (size_t i = 0; !code && i < dep->staff_size; ++i)
    {
        int flag = 0;
        
        do
        {
            code = generate_random_str(&dep->staff[i].name, "a-zA-Z", 40);
            flag = 0;
            
            for (size_t j = 0; !code && j < i; ++j)
            {
                if (!strcmp(dep->staff[i].name, dep->staff[j].name))
                {
                    flag = 1;
                    free(dep->staff[i].name);
                    break;
                }
            }
        } while (!code && flag);
    }
    
    if (code)
    {
        for (size_t i = 0; i < dep->staff_size; ++i)
        {
            free(dep->staff[i].name);
            dep->staff[i].name = NULL;
        }
    }
    
    return code;
}

status_code operator_give_task(Department* dep, size_t op_id, const char time[21], request* req)
{
    if (dep == NULL || time == NULL || req == NULL)
    {
        return NULL_ARG;
    }
    if (op_id >= dep->staff_size)
    {
        return INVALID_INPUT;
    }
    
    Operator* op = &(dep->staff[op_id]);
    dep->free_staff_cnt--;
    
    unsigned handling_time = 0;
    if (dep->max_handling_time == UINT_MAX && dep->min_handling_time == 0)
    {
        handling_time = rand_32();
    }
    else
    {
        handling_time = dep->min_handling_time + rand_32() % (dep->max_handling_time - dep->min_handling_time + 1);
    }
    
    op->req = req;
    op->handling_time = handling_time;
    strcpy(op->start_time, time);
    iso_time_add(op->start_time, op->handling_time * 60, op->finish_time);
    
    return OK;
}


status_code department_set_null(Department* dep)
{
    if (dep == NULL)
    {
        return NULL_ARG;
    }
    dep->queue = NULL;
    dep->staff_size = 0;
    dep->staff = NULL;
    dep->free_staff_cnt = 0;
    dep->load_coef = 0;
    dep->overload_coef = 0;
    dep->eps = 0;
    dep->overload_flag = 0;
    dep->min_handling_time = 0;
    dep->max_handling_time = 0;
    return OK;
}

status_code department_construct(
    Department* dep,
    const char* dep_name,
    size_t staff_size,
    pq_base_t base,
    double overload_coef,
    double eps,
    unsigned min_handling_time,
    unsigned max_handling_time,
    int (*comp)(const request*, const request*))
{
    if (dep == NULL || comp == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    
    dep->name = (char*) malloc(sizeof(char) * (strlen(dep_name) + 1));
    if (dep->name == NULL)
    {
        return BAD_ALLOC;
    }
    
    dep->staff = (Operator*) malloc(sizeof(Operator) * staff_size);
    if (dep->staff == NULL)
    {
        free(dep->name);
        return BAD_ALLOC;
    }
    for (size_t i = 0; i < staff_size; ++i)
    {
        dep->staff[i].name = NULL;
        dep->staff[i].req = NULL;
        dep->staff[i].start_time[0] = '\0';
        dep->staff[i].finish_time[0] = '\0';
        dep->staff[i].handling_time = 0;
    }
    
    dep->staff_size = staff_size;
    code = code ? code : generate_names(dep);
    
    if (!code)
    {
        dep->queue = (p_queue*) malloc(sizeof(p_queue));
        code = dep->queue == NULL ? BAD_ALLOC : OK;
    }
    
    code = code ? code : p_queue_set_null(dep->queue);
    code = code ? code : p_queue_construct(dep->queue, base, comp);
    if (code)
    {
        department_destruct(dep);
        return code;
    }
    
    strcpy(dep->name, dep_name);
    dep->free_staff_cnt = staff_size;
    dep->load_coef = 0;
    dep->overload_coef = overload_coef;
    dep->eps = eps;
    dep->overload_flag = 0;
    dep->min_handling_time = min_handling_time;
    dep->max_handling_time = max_handling_time;
    
    return OK;
}

status_code department_destruct(Department* dep)
{
    if (dep == NULL)
    {
        return OK;
    }
    
    for (size_t i = 0; i < dep->staff_size; ++i)
    {
        request_destruct(dep->staff[i].req);
        free_all(2, dep->staff[i].name, dep->staff[i].req);
    }
    
    p_queue_destruct(dep->queue);
    free_all(3, dep->name, dep->queue, dep->staff);
    department_set_null(dep);
    return OK;
}

status_code department_update_load_coef(Department* dep)
{
    if (dep == NULL)
    {
        return NULL_ARG;
    }
    
    size_t task_cnt = 0;
    status_code code = p_queue_size(dep->queue, &task_cnt);
    if (code)
    {
        return code;
    }
    
    dep->load_coef = 1.0 * task_cnt / dep->staff_size;
    dep->overload_flag = dep->load_coef - dep->overload_coef > -dep->eps;
    
    return OK;
}

status_code department_get_task_cnt(Department* dep, size_t* task_cnt)
{
    if (dep == NULL || task_cnt == NULL)
    {
        return NULL_ARG;
    }
    return p_queue_size(dep->queue, task_cnt);
}

status_code department_handle_finishing(Department* dep, const char time[21], size_t* msg_cnt, dep_msg** msgs)
{
    if (dep == NULL || time == NULL || msg_cnt == NULL || msgs == NULL)
    {
        return NULL_ARG;
    }
    
    size_t msg_cnt_tmp = 0;
    dep_msg* msgs_tmp = NULL;
    
    for (size_t i = 0; i < dep->staff_size; ++i)
    {
        Operator* op = &(dep->staff[i]);
        if (op->req != NULL && strcmp(time, op->finish_time) >= 0)
        {
            ++msg_cnt_tmp;
        }
    }
    
    msgs_tmp = (dep_msg*) malloc(sizeof(dep_msg) * msg_cnt_tmp);
    if (msgs_tmp == NULL)
    {
        return BAD_ALLOC;
    }
    
    if (msg_cnt_tmp == 0)
    {
        *msg_cnt = 0;
        *msgs = msgs_tmp;
        return OK;
    }
    
    for (size_t i = 0, j = 0; i < dep->staff_size; ++i)
    {
        Operator* op = &(dep->staff[i]);
        if (op->req != NULL && strcmp(time, op->finish_time) >= 0)
        {
            msgs_tmp[j].code = REQUEST_HANDLING_FINISHED;
            msgs_tmp[j].req_id = op->req->id;
            msgs_tmp[j].dep_name = dep->name;
            msgs_tmp[j].dep_name = NULL;
            msgs_tmp[j].handling_time = op->handling_time;
            msgs_tmp[j].oper_name = op->name;
            ++j;
            
            request_destruct(op->req);
            free(op->req);
            
            op->req = NULL;
            op->start_time[0] = '\0';
            op->finish_time[0] = '\0';
            op->handling_time = 0;
        }
    }
    
    dep->free_staff_cnt += msg_cnt_tmp;
    *msg_cnt = msg_cnt_tmp;
    *msgs = msgs_tmp;
    return OK;
}

status_code department_add_request(Department* dep, const char time[21], request* req, size_t* msg_cnt, dep_msg** msgs)
{
    if (dep == NULL || time == NULL || req == NULL || msg_cnt == NULL || msgs == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    size_t free_id = 0;
    int prev_overload_flag = dep->overload_flag;
    
    size_t msg_cnt_tmp = 1;
    dep_msg* msgs_tmp = (dep_msg*) malloc(sizeof(dep_msg) * 2);
    if (msgs_tmp == NULL)
    {
        return BAD_ALLOC;
    }
    
    msgs_tmp[0].code = NEW_REQUEST;
    msgs_tmp[0].req_id = req->id;
    msgs_tmp[0].dep_name = dep->name;
    msgs_tmp[0].transfer_dep_name = NULL;
    
    code = code ? code : p_queue_insert(dep->queue, req);
    
    if (dep->free_staff_cnt > 0)
    {
        while (dep->staff[free_id].req != NULL)
        {
            ++free_id;
        }
        code = code ? code : p_queue_pop(dep->queue, &req);
        code = code ? code : operator_give_task(dep, free_id, time, req);
        msgs_tmp[1].code = REQUEST_HANDLING_STARTED;
        msgs_tmp[1].req_id = req->id;
        msgs_tmp[1].dep_name = dep->name;
        msgs_tmp[1].transfer_dep_name = NULL;
        msgs_tmp[1].oper_name = dep->staff[free_id].name; // <---------------------------- mb copy???????????
        msg_cnt_tmp = 2;
    }
    
    code = code ? code : department_update_load_coef(dep);
    
    if (code)
    {
        free(msgs_tmp);
        return code;
    }
    
    if (!prev_overload_flag && dep->overload_flag)
    {
        msgs_tmp[1].code = DEPARTMENT_OVERLOADED;
        msgs_tmp[1].req_id = req->id;
        msgs_tmp[1].dep_name = dep->name;
        msgs_tmp[1].transfer_dep_name = NULL;
        msg_cnt_tmp = 2;
    }
    
    *msg_cnt = msg_cnt_tmp;
    *msgs = msgs_tmp;
    
    return OK;
}

status_code department_handle_starting(Department* dep, const char time[21], size_t* msg_cnt, dep_msg** msgs)
{
    if (dep == NULL || time == NULL || msg_cnt == NULL || msgs == NULL)
    {
        return NULL_ARG;
    }
    
    size_t task_cnt = 0;
    status_code code = p_queue_size(dep->queue, &task_cnt);
    if (code)
    {
        return code;
    }
    
    size_t msg_cnt_tmp = dep->free_staff_cnt < task_cnt ? dep->free_staff_cnt : task_cnt;
    dep_msg* msgs_tmp = (dep_msg*) malloc(sizeof(dep_msg) * msg_cnt_tmp);
    if (msgs_tmp == NULL)
    {
        return BAD_ALLOC;
    }
    
    size_t free_id = 0;
    for (size_t i = 0; i < msg_cnt_tmp && !code; ++i)
    {
        while (dep->staff[free_id].req != NULL)
        {
            ++free_id;
        }
        
        request* req = NULL;
        code = code ? code : p_queue_pop(dep->queue, &req);
        code = code ? code : operator_give_task(dep, free_id, time, req);
        
        msgs_tmp[i].code = REQUEST_HANDLING_STARTED;
        msgs_tmp[i].req_id = req->id;
        msgs_tmp[i].dep_name = dep->name;
        msgs_tmp[i].transfer_dep_name = NULL;
        msgs_tmp[i].oper_name = dep->staff[free_id].name;
    }
    
    code = code ? code : department_update_load_coef(dep);
    
    if (code)
    {
        free(msgs_tmp);
        return code;
    }
    
    *msg_cnt = msg_cnt_tmp;
    *msgs = msgs_tmp;
    return OK;
}

status_code department_can_handle_transfer(Department* dep, size_t extra_task_cnt, int* can_handle)
{
    if (dep == NULL)
    {
        return NULL_ARG;
    }
    
    size_t task_cnt = 0;
    status_code code = p_queue_size(dep->queue, &task_cnt);
    if (code)
    {
        return code;
    }
    
    double extra_load_coef = 1.0 * (task_cnt + extra_task_cnt) / dep->staff_size;
    *can_handle = extra_load_coef - dep->overload_coef < -dep->eps;
    
    return OK;
}

status_code department_transfer(Department* dep_dest, Department* dep_src)
{
    if (dep_dest == NULL || dep_src == NULL)
    {
        return NULL_ARG;
    }
    
    status_code code = OK;
    
    code = code ? code : p_queue_meld(dep_dest->queue, dep_dest->queue, dep_src->queue);
    code = code ? code : department_update_load_coef(dep_dest);
    code = code ? code : department_update_load_coef(dep_src);
    
    return code;
}