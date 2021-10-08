cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME parallel_merge_sort)
project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 17)

# Options
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors." ON)
option(ENABLE_PVS_STUDIO "Check using command-line PVS-Studio." ON)
option(ENABLE_SANITIZERS    "Use sanitizers to detect errors." ON) # Option for the test builds. Do not use it in the production builds.

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# Include headers
include_directories(include)

set (CMAKE_BUILD_TYPE Debug)

# Project source compilation
add_executable(${PROJECT_NAME} src/main.cpp src/linear_implementation.cpp src/parallel_implementation.cpp)

# Add external packages
find_package(Boost 1.71.0 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC Boost::program_options)

# Add C++ threads
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${CMAKE_THREAD_LIBS_INIT})

# set output directory (bin)
set_target_properties(${PROJECT_NAME}
						PROPERTIES
						RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

#####################################
# Define ALL_TARGETS variable to use in PVS and Sanitizers
set (ALL_TARGETS ${PROJECT_NAME})

# Include fixed CMake configuration
include(cmake/config.cmake)
