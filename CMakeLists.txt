cmake_minimum_required(VERSION 3.28)
project(jade)

include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(jade
    src/main.cpp)

target_include_directories(jade PRIVATE src)

target_compile_options(jade PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-Wall;-Wextra;-Wpedantic;-Wconversion>
)

set_target_properties(jade PROPERTIES COMPILE_WARNING_AS_ERROR ON)

if(BUILD_TESTING)
    add_executable(hash_map_test
        tests/hash_map_test.cpp)

    target_include_directories(hash_map_test PRIVATE src)

    target_compile_options(hash_map_test PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-Wall;-Wextra;-Wpedantic;-Wconversion>
    )

    set_target_properties(hash_map_test PROPERTIES COMPILE_WARNING_AS_ERROR ON)

    add_test(NAME hash_map_test COMMAND hash_map_test)
endif()
