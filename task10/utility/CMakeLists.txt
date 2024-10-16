cmake_minimum_required(VERSION 3.21)
project(mp_4_10_utility)

add_library(
        mp_4_10_utility
        src/utility.c)
target_include_directories(
        mp_4_10_utility
        PUBLIC
        ./include)
set_target_properties(
        mp_4_10_utility PROPERTIES
        LANGUAGES C
        LINKER_LANGUAGE C
        CMAKE_C_STANDARD 17
        VERSION 1.0
        DESCRIPTION "mp_4_10_utility")