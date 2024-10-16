#ifndef _READERS_H_
#define _READERS_H_

#include <utility.h>

status_code read_config(FILE* file, config_data* config);

status_code check_for_breakpoint(FILE* file, int* is_breakpoint);

status_code read_instruction(FILE* file, char** instruction);

#endif // _READERS_H_