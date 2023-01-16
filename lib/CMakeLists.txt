

add_library(libs STATIC  
                env/environment.cc env/symbol.cc env/symbol.cc
                inter/semantic.cc inter/tree.cc
                lex/input.cc lex/lex.cc lex/output.cc  lex/lexWE.cc lex/token.cc
                parser/decl.cc parser/expr.cc parser/parser.cc parser/parserTool.cc
                parser/parserWE.cc parser/stmt.cc
                semantic/semacticWE.cc semantic/type.cc
                tools/tools.cc)
target_include_directories(libs PUBLIC ${CMAKE_SOURCE_DIR}/includes)
target_link_libraries(libs PUBLIC fmt)