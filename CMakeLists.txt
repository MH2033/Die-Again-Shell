cmake_minimum_required(VERSION 3.13.0)
project(Die-Again-Shell VERSION 0.1.0)

include_directories(inc)
add_executable(Die-Again-Shell main.cpp)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
