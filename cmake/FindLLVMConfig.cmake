find_program(LLVM_CONFIG NAMES llvm-config-14 llvm-config)

if (NOT ${LLVM_CONFIG_FOUND})
    message(WARNING "llvm-config not found.")
    return()
endif ()

macro (llvm_set var flag)
    if (LLVM_FIND_QUIETLY)
        set(_quiet_arg ERROR_QUIET)
    endif ()
    set(result_code)
    execute_process(COMMAND ${LLVM_CONFIG} --${flag} RESULT_VARIABLE result_code
                    OUTPUT_VARIABLE LLVM_${var} OUTPUT_STRIP_TRAILING_WHITESPACE ${_quiet_arg})
    if (result_code)
        _llvm_fail(
            "Failed to execute llvm-config ('${LLVM_CONFIG}', result code: '${result_code})'")
    else ()
        if (${ARGV2})
            file(TO_CMAKE_PATH "${LLVM_${var}}" LLVM_${var})
        endif ()
    endif ()
endmacro ()

llvm_set(VERSION_STRING version)
llvm_set(CMAKE_DIR cmakedir)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${LLVM_CMAKE_DIR})
include(AddLLVM)
