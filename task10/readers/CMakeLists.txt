cmake_minimum_required(VERSION 3.21)
project(mp_4_10_readers)

add_library(
        mp_4_10_readers
        src/readers.c)
target_include_directories(
        mp_4_10_readers
        PUBLIC
        ./include)
target_link_libraries(
        mp_4_10_readers
        PUBLIC
        mp_4_10_utility)
set_target_properties(
        mp_4_10_readers PROPERTIES
        LANGUAGES C
        LINKER_LANGUAGE C
        CMAKE_C_STANDARD 17
        VERSION 1.0
        DESCRIPTION "mp_4_10_readers")