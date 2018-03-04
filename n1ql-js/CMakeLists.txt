cmake_minimum_required(VERSION 3.9)
project(n1ql_js)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(deps/googletest)
include_directories(deps/googletest/googletest/include)
include_directories(deps/googletest/googlemock/include)

set(N1QL_JS_SRC src/main.cpp)
add_executable(n1ql_js ${N1QL_JS_SRC})
target_link_libraries(n1ql_js gtest gtest_main)