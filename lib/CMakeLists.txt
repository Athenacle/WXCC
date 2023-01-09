

add_library(libs STATIC  
                env/environment.cpp env/symbol.cpp env/symbol.cpp
                inter/semantic.cpp inter/tree.cpp
                lex/input.cpp lex/lex.cpp lex/lexDebug.cpp  lex/lexWE.cpp
                parser/decl.cpp parser/expr.cpp parser/parser.cpp parser/parserTool.cpp
                parser/parserWE.cpp parser/stmt.cpp
                semantic/semacticWE.cpp semantic/type.cpp
                tools/tools.cpp)
target_include_directories(libs PUBLIC ${CMAKE_SOURCE_DIR}/includes)