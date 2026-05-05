cmake_minimum_required(VERSION 3.10)

project(exp10_project C)

set(CMAKE_C_STANDARD 11)

# флаги оптимизации
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native -mfma -mavx2")

# подключаем include
include_directories(include)

# список всех исходников
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
)

# создаём исполняемый файл
add_executable(lab ${SOURCES})

# линковка math библиотеки
target_link_libraries(lab m)