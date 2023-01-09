cmake_minimum_required(VERSION 3.14)

project(WXCC CXX C)
set(CMAKE_CXX_STANDARD 20)
set(MAJOR_VERSION 0)
set(MINOR_VERSION 0)
set(PATCH_VERSION 0)
set(WXAC_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")

set(OUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/out/)
file(MAKE_DIRECTORY ${OUT_PATH})

set(LIBRARY_OUTPUT_PATH ${OUT_PATH})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
include(Check)


add_subdirectory(lib)

add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC includes/)
target_link_libraries(${PROJECT_NAME} PUBLIC libs)
target_compile_options(${PROJECT_NAME} PUBLIC -Wall)