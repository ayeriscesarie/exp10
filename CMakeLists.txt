cmake_minimum_required(VERSION 3.10)

project(exp10_project C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native -mfma -mavx2")

include_directories(include)

set(SOURCES
    src/main.c
    src/common.c
    src/bench.c
    src/accuracy_logger.c
    src/v0.c
    src/v1.c
    src/v2.c
    src/v3.c
    src/v4.c
    src/v5.c
    src/v6.c
    src/v7_avx2.c
    src/v5_avx.c
    src/v4_avx.c
    src/v3_avx.c
    src/sleef_avx.c
)

add_executable(lab ${SOURCES})

target_link_libraries(
    lab
    m
    sleef
)