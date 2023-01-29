cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0135 OLD)

project(wxcc CXX C)
set(CMAKE_CXX_STANDARD 17)
set(MAJOR_VERSION 0)
set(MINOR_VERSION 0)
set(PATCH_VERSION 0)
set(PROJECT_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")

set(OUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/out/)
file(MAKE_DIRECTORY ${OUT_PATH})
set(THIRD_PARTY_DIR ${CMAKE_BINARY_DIR}/3party)
file(MAKE_DIRECTORY ${THIRD_PARTY_DIR})

option(CLANG_TIDY "Enable clang-tidy" OFF)

if (${CLANG_TIDY})
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
endif ()

if (NOT ${CMAKE_BUILD_TYPE})
    set(CMAKE_BUILD_TYPE "Debug")
endif ()

set(LIBRARY_OUTPUT_PATH ${OUT_PATH})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
include(Check)
include(Fmt)
include(ArgParse)
include(Coverage)
find_package(Sanitizers)

configure_file(${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/config.h @ONLY)
include_directories(${CMAKE_BINARY_DIR})
add_compile_definitions(HAVE_CONFIG_H)

add_subdirectory(lib)
add_subdirectory(test)

# add_executable(${PROJECT_NAME}_v1 driver/v1/main.cc driver/opt.cc)
# target_link_libraries(${PROJECT_NAME}_v1 PUBLIC libs v1 Fmt)
# add_dependencies(${PROJECT_NAME}_v1 argparse)
# target_include_directories(${PROJECT_NAME}_v1 PUBLIC includes/ driver/)
# target_include_directories(${PROJECT_NAME}_v1 SYSTEM PUBLIC ${ARGPARSE_INC_DIR})

add_executable(${PROJECT_NAME} driver/main.cc driver/opt.cc)
target_link_libraries(${PROJECT_NAME} PUBLIC libs Fmt)
add_dependencies(${PROJECT_NAME} argparse)
target_include_directories(${PROJECT_NAME} PUBLIC includes/)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${ARGPARSE_INC_DIR})

# add_code_coverage(${PROJECT_NAME}_v1)
add_code_coverage(${PROJECT_NAME})
add_code_coverage_all_targets(EXCLUDE test/* 3party/*)
add_sanitizers(${PROJECT_NAME})
