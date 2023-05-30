cmake_minimum_required(VERSION 3.25)
project(cpp_inquirer)
set(CMAKE_CXX_STANDARD 11)

add_library(Inquirer src/inquirer.cpp)

add_executable(cpp_inquirer src/main.cpp)

target_link_libraries(${PROJECT_NAME} Inquirer)
target_include_directories(${PROJECT_NAME} PRIVATE src)
