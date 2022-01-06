cmake_minimum_required(VERSION 3.14)

project(WXCC CXX)
set(CMAKE_CXX_STANDARD 20)

set(SRC_FILES   env/environment.cpp env/symbol.cpp env/symbol.cpp
                inter/semantic.cpp inter/tree.cpp
                lex/input.cpp lex/lex.cpp lex/lexDebug.cpp  lex/lexWE.cpp
                parser/decl.cpp parser/expr.cpp parser/parser.cpp parser/parserTool.cpp
                parser/parserWE.cpp parser/stmt.cpp
                semantic/semacticWE.cpp semantic/type.cpp
                tools/tools.cpp)

add_executable(${PROJECT_NAME} main.cpp ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC includes/)
target_compile_options(${PROJECT_NAME} PUBLIC -Wall)