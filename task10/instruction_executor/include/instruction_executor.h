#ifndef _INSTRUCTION_EXECUTOR_H_
#define _INSTRUCTION_EXECUTOR_H_

#include <utility.h>
#include <trie.h>

status_code execute_instruction(const char* instruction, Trie* environment, config_data config);

#endif // _INSTRUCTION_EXECUTOR_H_