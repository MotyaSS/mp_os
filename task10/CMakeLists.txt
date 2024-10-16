cmake_minimum_required(VERSION 3.21)
project(mp_4_10)

add_subdirectory(utility)
add_subdirectory(trie)
add_subdirectory(readers)
add_subdirectory(instruction_executor)
add_subdirectory(debugger)

add_executable(
        mp_4_10
        main.c)
target_include_directories(
        mp_4_10
        PUBLIC
        ./include)
target_link_libraries(
        mp_4_10
        PUBLIC
        mp_4_10_utility)
target_link_libraries(
        mp_4_10
        PUBLIC
        mp_4_10_trie)
target_link_libraries(
        mp_4_10
        PUBLIC
        mp_4_10_readers)
target_link_libraries(
        mp_4_10
        PUBLIC
        mp_4_10_instruction_executor)
target_link_libraries(
        mp_4_10
        PUBLIC
        mp_4_10_debugger)
set_target_properties(
        mp_4_10 PROPERTIES
        LANGUAGES C
        LINKER_LANGUAGE C
        CMAKE_C_STANDARD C14
        VERSION 1.0
        DESCRIPTION "mp_4_10")