cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME triangulation)
project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 17)

# Options
set(WARNINGS_AS_ERRORS ON)
set(ENABLE_PVS_STUDIO ON)
set(ENABLE_SANITIZERS ON)

#! Build release version if not specified otherwise.
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif ()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# Include headers
include_directories(include)

# Project source compilation
add_executable(${PROJECT_NAME} src/triangulation.cpp)

add_executable(instance_generator src/instance_generator.cpp)
#
find_package(Threads)
find_package(OpenMP)

# Add external packages
find_package(Boost 1.71.0 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} Boost::program_options ${CMAKE_THREAD_LIBS_INIT} OpenMP::OpenMP_CXX)

# set output directory (bin)
set_target_properties(${PROJECT_NAME}
						PROPERTIES
						RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopt-info-vec -fopt-info-vec-missed -Ofast -march=native")
endif()
#####################################
# Define ALL_TARGETS variable to use in PVS and Sanitizers
set (ALL_TARGETS ${PROJECT_NAME})

# Include fixed CMake configuration
include(cmake/config.cmake)
