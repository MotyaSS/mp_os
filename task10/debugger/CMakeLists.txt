cmake_minimum_required(VERSION 3.21)
project(mp_4_10_debugger)

add_library(
        mp_4_10_debugger
        src/debugger.c)
target_include_directories(
        mp_4_10_debugger
        PUBLIC
        ./include)
target_link_libraries(
        mp_4_10_debugger
        PUBLIC
        mp_4_10_utility)
target_link_libraries(
        mp_4_10_debugger
        PUBLIC
        mp_4_10_trie)
set_target_properties(
        mp_4_10_debugger PROPERTIES
        LANGUAGES C
        LINKER_LANGUAGE C
        CMAKE_C_STANDARD 17
        VERSION 1.0
        DESCRIPTION "mp_4_10_debugger")