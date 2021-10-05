cmake_minimum_required(VERSION 3.5)
project(LabCppThreads2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads)

add_executable(PromiseAndFuture ../src/PromiseAndFuture.cpp)
target_link_libraries(PromiseAndFuture ${CMAKE_THREAD_LIBS_INIT})

add_executable(Async ../src/Async.cpp)
target_link_libraries(Async ${CMAKE_THREAD_LIBS_INIT})

add_executable(AtomicCounter ../src/AtomicCounter.cpp)
target_link_libraries(AtomicCounter ${CMAKE_THREAD_LIBS_INIT})

add_executable(BarrierTest ../src/Barrier.hpp ../src/BarrierTest.cpp)
target_link_libraries(BarrierTest ${CMAKE_THREAD_LIBS_INIT})

