cmake_minimum_required(VERSION 3.16)

#set(CMAKE_VERBOSE_MAKEFILE on)

project(TestAndPlot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# ------------------ SCIPLOT ------------------
message(STATUS "Fetching sciplot...")
FetchContent_Declare(
    sciplot
    GIT_REPOSITORY      https://github.com/sciplot/sciplot.git
    GIT_SHALLOW         TRUE
)

option(SCIPLOT_BUILD_EXAMPLES "Build examples" OFF)
option(SCIPLOT_BUILD_TESTS "Build tests" OFF)
option(SCIPLOT_BUILD_DOCS "Build documentation" OFF)

FetchContent_MakeAvailable(sciplot)

# ------------------ VARS ------------------
set(LIBS sciplot)

# ------------------ TARGETS ------------------
# ------------------ EXPERIMENT 0 ------------------
add_executable(tap "${CMAKE_CURRENT_LIST_DIR}/src/ex_0.cpp")
target_compile_options( tap PRIVATE $<$<CONFIG:Debug>: -O0 -g3 > ) #add here specific compile for debug mode flags
target_compile_options( tap PRIVATE $<$<CONFIG:Release>: -O3 -march=native > ) #add here specific compile for release mode flags
target_link_libraries( tap "${LIBS}" )
