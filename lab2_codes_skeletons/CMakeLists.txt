cmake_minimum_required(VERSION 3.5)
project(LabCppThreads1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads)

add_executable(HelloThreads src/HelloThreads.cpp)
target_link_libraries(HelloThreads ${CMAKE_THREAD_LIBS_INIT})

add_executable(CounterFirstTry src/CounterFirstTry.cpp)
target_link_libraries(CounterFirstTry ${CMAKE_THREAD_LIBS_INIT})

add_executable(CounterSecondTry src/CounterSecondTry.cpp)
target_link_libraries(CounterSecondTry ${CMAKE_THREAD_LIBS_INIT})

add_executable(ProducerConsumerBusyWaiting src/ProducerConsumerBusyWaiting.cpp)
target_link_libraries(ProducerConsumerBusyWaiting ${CMAKE_THREAD_LIBS_INIT})

add_executable(ProducerConsumerCondVar src/ProducerConsumerCondVar.cpp)
target_link_libraries(ProducerConsumerCondVar ${CMAKE_THREAD_LIBS_INIT})
