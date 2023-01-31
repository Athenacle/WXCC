include(CheckFunctionExists)
include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)
include(CheckSymbolExists)
include(CMakeDependentOption)

check_symbol_exists(bzero strings.h HAVE_BZERO)
check_symbol_exists(alloca alloca.h HAVE_ALLOCA)

if (UNIX)
    check_include_file_cxx(pthread.h HAVE_PTHREAD_H)
    check_symbol_exists(getopt_long unistd.h UNIX_HAVE_GETOPTLONG)
    check_symbol_exists(pthread_mutex_lock pthread.h UNIX_HAVE_PTHREAD_MUTEX)
    check_symbol_exists(pthread_barrier_init pthread.h UNIX_HAVE_PTHREAD_BARRIER)
    check_symbol_exists(pthread_cond_init pthread.h UNIX_HAVE_PTHREAD_COND)
    check_symbol_exists(dlopen dlfcn.h UNIX_HAVE_DLOPEN)
    check_symbol_exists(nanosleep time.h UNIX_HAVE_NANOSLEEP)
    check_symbol_exists(get_nprocs sys/sysinfo.h UNIX_HAVE_GET_NPROCS)
    check_symbol_exists(setenv cstdlib UNIX_HAVE_SETENV)
    check_symbol_exists(mmap sys/mman.h UNIX_HAVE_MMAP)
elseif (WIN32)
    add_compile_options("/EHsc")
endif ()

check_cxx_source_compiles(
    "
    #include <pthread.h>
    int main(){
        pthread_spinlock_t *spin;
        pthread_spin_lock(spin);
    }
    "
    UNIX_HAVE_PTHREAD_SPINLOCK)

check_cxx_source_compiles(
    "
    #include <time.h>
    int main(){return clock_gettime(CLOCK_REALTIME_COARSE, nullptr);}
    "
    HAVE_CLOCK_REALTIME_COARSE)

check_cxx_source_compiles(
    "
    int main(int, char*[]){
        __builtin_unreachable();
    }
    "
    HAVE_BUILTIN_UNREACHABLE)

check_cxx_source_compiles("int main() {return __builtin_expect(0, 1);}" HAVE_BUILTIN_EXPECT)

set(ACTIVE_COMPILER_FLAG)

macro (CXX_COMPILER_CHECK_ADD)
    set(list_var "${ARGN}")
    foreach (flag IN LISTS list_var)
        string(TOUPPER ${flag} FLAG_NAME1)
        string(REPLACE "-" "_" FLAG_NAME2 ${FLAG_NAME1})
        string(REPLACE "=" "_" FLAG_NAME3 ${FLAG_NAME2})
        string(CONCAT FLAG_NAME "COMPILER_SUPPORT_" ${FLAG_NAME3})
        check_cxx_compiler_flag(-${flag} ${FLAG_NAME})
        if (${${FLAG_NAME}})
            list(APPEND ACTIVE_COMPILER_FLAG "-${flag}")
        endif ()
    endforeach ()
endmacro ()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    cxx_compiler_check_add(
        fno-exceptions
        fno-permissive
        # fno-rtti
        fstack-usage
        Wall
        Wconversion
        Wdouble-promotion
        Wduplicated-branches
        Wduplicated-cond
        Wextra
        Wextra-semi
        Wformat=2
        Wformat-overflow
        Wformat-truncation
        Wimplicit-float-conversion
        Wlogical-op
        Wno-useless-cast
        Wno-variadic-macros
        Wnull-dereference
        Wpedantic
        Wrestrict
        Wshorten-64-to-32)

    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        cxx_compiler_check_add(fstack-protector-strong)
        cxx_compiler_check_add(fno-omit-frame-pointer)
        add_compile_definitions(_GLIBCXX_ASSERTIONS)
        set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    endif ()
endif ()

find_program(FILT_FILE c++filt)
set(ALL_SU "${CMAKE_BINARY_DIR}/CMakeFiles/stack_usage.su")
file(REMOVE ${ALL_SU})

cmake_dependent_option(ENABLE_STACK_CHECK "Enable Stack Check Report" OFF
                       "UNIX;FILT_FILE;HAVE_UNISTD_H;COMPILER_SUPPORT_FSTACK_USAGE" ON)

if (${ENABLE_STACK_CHECK})
    add_executable(stack-usage-bin ${CMAKE_SOURCE_DIR}/cmake/stack-usage.cc)
    target_include_directories(stack-usage-bin PRIVATE ${CMAKE_BINARY_DIR})
    target_compile_definitions(stack-usage-bin PRIVATE SOURCE_DIR="${CMAKE_SOURCE_DIR}"
                                                       CPP_FILT="${FILT_FILE}" ALL_SU="${ALL_SU}")
    target_compile_options(stack-usage-bin PRIVATE -O3)
    set_target_properties(stack-usage-bin PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                                     ${CMAKE_BINARY_DIR}/CMakeFiles/)
    add_custom_target(report-stack-usage COMMAND $<TARGET_FILE:stack-usage-bin>
                      DEPENDS stack-usage-bin)
endif ()

macro (add_target_compile_flags)
    set(list_var "${ARGN}")
    foreach (tgt IN LISTS list_var)
        if (NOT TARGET ${tgt})
            message(STATUS "no such target: ${tgt}")
        else ()
            foreach (f IN LISTS ACTIVE_COMPILER_FLAG)
                target_compile_options(${tgt} PRIVATE ${f})
            endforeach ()

            if (${ENABLE_STACK_CHECK})
                get_target_property(SOURCE_FILE ${tgt} SOURCES)
                get_target_property(BD ${tgt} BINARY_DIR)
                set(STACK_FILES_FILE "${BD}/CMakeFiles/${tgt}.dir/stack-usage.su")
                file(REMOVE ${STACK_FILES_FILE})
                foreach (file IN LISTS SOURCE_FILE)
                    set(STACK_FILE "${BD}/CMakeFiles/${tgt}.dir/${file}.su")
                    list(APPEND STACK_USAGE_FILES ${STACK_FILE})
                    file(APPEND ${STACK_FILES_FILE} ${STACK_FILE} "\n")
                endforeach ()
                file(APPEND ${ALL_SU} ${STACK_FILES_FILE} "\n")
            endif ()
        endif ()
    endforeach ()
endmacro ()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(ON_64BITS ON)
endif ()

find_program(GIT_COMMAND git)
execute_process(COMMAND ${GIT_COMMAND} rev-parse HEAD RESULT_VARIABLE result
                OUTPUT_VARIABLE GIT_HASH_OUTPUT OUTPUT_STRIP_TRAILING_WHITESPACE)

if (NOT "${GIT_HASH_OUTPUT}" STREQUAL "")
    set(GIT_HASH "${GIT_HASH_OUTPUT}")
    set(HAVE_GIT_SHA ON)
endif ()

get_cmake_property(CACHE_VARS CACHE_VARIABLES)
foreach (CACHE_VAR ${CACHE_VARS})
    get_property(CACHE_VAR_HELPSTRING CACHE ${CACHE_VAR} PROPERTY HELPSTRING)
    if (CACHE_VAR_HELPSTRING STREQUAL "No help, variable specified on the command line.")
        get_property(CACHE_VAR_TYPE CACHE ${CACHE_VAR} PROPERTY TYPE)
        if (CACHE_VAR_TYPE STREQUAL "UNINITIALIZED")
            set(CACHE_VAR_TYPE)
        else ()
            set(CACHE_VAR_TYPE :${CACHE_VAR_TYPE})
        endif ()
        set(CMAKE_ARGS "${CMAKE_ARGS} -D${CACHE_VAR}${CACHE_VAR_TYPE}=\"${${CACHE_VAR}}\"")
    endif ()
endforeach ()

string(REPLACE "\"" "\\\"" CMAKE_ARGS_REPLACED "${CMAKE_ARGS}")

string(TIMESTAMP CONFIGURE_TIME "%Y-%m-%dT%H:%M:%S" UTC)
add_compile_definitions(CONFIGURE_TIME="${CONFIGURE_TIME}")
