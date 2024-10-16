cmake_minimum_required(VERSION 3.21)
project(mp_4_10_trie)

add_library(
        mp_4_10_trie
        src/trie.c)
target_include_directories(
        mp_4_10_trie
        PUBLIC
        ./include)
target_link_libraries(
        mp_4_10_trie
        PUBLIC
        mp_4_10_utility)
set_target_properties(
        mp_4_10_trie PROPERTIES
        LANGUAGES C
        LINKER_LANGUAGE C
        CMAKE_C_STANDARD 17
        VERSION 1.0
        DESCRIPTION "mp_4_10_trie")