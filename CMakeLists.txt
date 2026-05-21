cmake_minimum_required(VERSION 3.23)

project(
    Truffle
    VERSION 0.1.0
    DESCRIPTION "Embeddable graphics engine and rendering system"
    LANGUAGES CXX)

option(TRUFFLE_BUILD_TESTS "Build Truffle contract tests" ${PROJECT_IS_TOP_LEVEL})
option(TRUFFLE_BUILD_EXAMPLES "Build Truffle examples" ${PROJECT_IS_TOP_LEVEL})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/EHsc)
endif()

include(CTest)

add_subdirectory(src)

if(TRUFFLE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(TRUFFLE_BUILD_TESTS)
    add_subdirectory(tests)
endif()
