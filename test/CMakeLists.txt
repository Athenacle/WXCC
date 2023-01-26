include(GTest)
enable_testing()

find_package(FLEX 2.99)
find_package(BISON 3.0)

set(TEST_NAME ${PROJECT_NAME}_all_in_one)

set(TEST_LIB_SRC test_lib.cc)

if (${FLEX_FOUND} AND ${BISON_FOUND})
    set(LEXER_OUT "${CMAKE_CURRENT_BINARY_DIR}/flex_lexer.cc")
    set(PARSER_OUT "${CMAKE_CURRENT_BINARY_DIR}/bison_parser.cc")

    flex_target(LEXER "${CMAKE_CURRENT_SOURCE_DIR}/flex_bison/misc/flex.l" "${LEXER_OUT}"
                DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/flex_lexer.tab.h")

    bison_target(
        PARSER "${CMAKE_CURRENT_SOURCE_DIR}/flex_bison/misc/parser.y" "${PARSER_OUT}"
        COMPILE_FLAGS "-Wcounterexamples -v"
        DEFINES_FILE "${CMAKE_CURRENT_BINARY_DIR}/bison_parser.tab.h")

    add_flex_bison_dependency(LEXER PARSER)
    list(APPEND TEST_LIB_SRC ${LEXER_OUT} ${PARSER_OUT} flex_bison/misc/fb.cc)
endif ()

add_library(test STATIC ${TEST_LIB_SRC})

set(FFF_INC /usr/include/boost)

target_link_libraries(test PRIVATE GTest::GTest GTest::GMock libs)
target_include_directories(
    test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/includes
                ${CMAKE_CURRENT_BINARY_DIR} $<$<BOOL:${FLEX_INCLUDE_DIRS}>:${FLEX_INCLUDE_DIRS}>)
set(LIBS)
macro (add_subdirectory_test)
    file(GLOB files ${ARGN}/*.cc)
    foreach (file ${files})
        get_filename_component(basename ${file} NAME_WE)
        set(TN test_${ARGN}_${basename})
        add_executable(${TN} ${file})
        list(APPEND LIBS ${file})
        target_link_libraries(${TN} PRIVATE GTest::GTest GTest::GMock GTest::GMockMain libs test)
        target_include_directories(${TN} PRIVATE ${CMAKE_SOURCE_DIR}/lib)
        add_test(${TN} ${TN} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    endforeach ()
endmacro ()

add_subdirectory_test(lex)
add_subdirectory_test(utils)
add_subdirectory_test(parser)

if (${FLEX_FOUND} AND ${BISON_FOUND})
    target_compile_definitions(test PUBLIC HAVE_FLEX_BISON)
    add_subdirectory_test(flex_bison)
    add_executable(flex_bison_runner ${CMAKE_CURRENT_SOURCE_DIR}/flex_bison/misc/main.cc)
    target_link_libraries(flex_bison_runner PRIVATE libs test)
endif ()

add_executable(${TEST_NAME} ${LIBS})
add_test(${TEST_NAME} ${TEST_NAME})
target_link_libraries(${TEST_NAME} PRIVATE GTest::GTest GTest::GMock GTest::GMockMain libs test)
target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_compile_definitions(
    ${TEST_NAME}
    PRIVATE "-DUNITTEST_FILE=$<TARGET_FILE:${TEST_NAME}>"
            "-DUNITTEST_FILENAME=$<TARGET_FILE_NAME:${TEST_NAME}>"
            "$<$<BOOL:${RUN_ID}>:RUN_ID=${RUN_ID}>")
add_custom_target(run-unittest COMMAND "$<TARGET_FILE:${TEST_NAME}>")

target_code_coverage(${TEST_NAME} EXCLUDE test/*)
