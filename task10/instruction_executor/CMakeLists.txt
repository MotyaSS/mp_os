cmake_minimum_required(VERSION 3.21)
project(mp_4_10_instruction_executor)

add_library(
        mp_4_10_instruction_executor
        src/instruction_executor.c
        src/int_stack.c
        src/operand_stack.c
        src/operator_stack.c)
target_include_directories(
        mp_4_10_instruction_executor
        PUBLIC
        ./include)
target_link_libraries(
        mp_4_10_instruction_executor
        PUBLIC
        mp_4_10_utility)
target_link_libraries(
        mp_4_10_instruction_executor
        PUBLIC
        mp_4_10_trie)
set_target_properties(
        mp_4_10_instruction_executor PROPERTIES
        LANGUAGES C
        LINKER_LANGUAGE C
        CMAKE_C_STANDARD 17
        VERSION 1.0
        DESCRIPTION "mp_4_10_instruction_executor")