
aux_source_directory(v1/inter V1_SRC_INTER)
aux_source_directory(v1/parser V1_SRC_PARSER)
aux_source_directory(v1/semantic V1_SRC_SEMANTIC)
aux_source_directory(v1/env V1_SRC_ENV)

set(V1_SRC ${V1_SRC_INTER} ${V1_SRC_PARSER} ${V1_SRC_SEMANTIC} ${V1_SRC_ENV})

add_library(v1 STATIC ${V1_SRC})
target_include_directories(v1 PUBLIC ${CMAKE_SOURCE_DIR}/includes ${CMAKE_SOURCE_DIR}/includes/v1)
target_link_libraries(v1 PUBLIC fmt)

add_library(libs STATIC   
                lex/input.cc lex/lex.cc lex/output.cc  lex/lexWE.cc lex/token.cc
                environment/identifier.cc
                parser/parser.cc parser/declaration.cc
                semantic/type.cc
                tools/error_manager.cc
                tools/tools.cc)

target_include_directories(libs PUBLIC ${CMAKE_SOURCE_DIR}/includes ${CMAKE_SOURCE_DIR}/includes/v1)
target_link_libraries(libs PUBLIC fmt)
