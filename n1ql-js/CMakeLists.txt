cmake_minimum_required(VERSION 3.8)
project(n1ql_js)

set(CMAKE_CXX_STANDARD 11)

set(V8_BUILD_DIR C:/Users/gauth/source/repos/v8/v8)
set(LIB_V8 ${V8_BUILD_DIR}/out.gn/x64.debug/*.dll.lib)
include_directories(${V8_BUILD_DIR})
include_directories(${V8_BUILD_DIR}/include)
link_directories(${V8_BUILD_DIR}/out.gn/x64.debug)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(deps/googletest)
include_directories(deps/googletest/googletest/include)

include_directories(.)

set(N1QL_JS_SRC src/main.cpp)
add_executable(n1ql_js ${N1QL_JS_SRC})
target_link_libraries(n1ql_js gtest_main)

set(N1QL_JS_TEST tests/src/main.cpp tests/src/utils.cpp tests/include/utils.h)
add_executable(n1ql_js_test ${N1QL_JS_TEST})
target_link_libraries(n1ql_js_test gtest_main ${LIB_V8})