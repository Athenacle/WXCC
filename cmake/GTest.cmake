# find_package(Threads REQUIRED)
# include(ExternalProject)
# 
# set(GTEST_ROOT ${THIRD_PARTY_DIR}/gtest/root CACHE FILEPATH "")
# 
# ExternalProject_Add(
#     gtest
#     URL https://github.com/google/googletest/archive/release-1.12.1.tar.gz
#     URL_HASH SHA256=81964fe578e9bd7c94dfdb09c8e4d6e6759e19967e397dbea48d1c10e45d0df2
#     DOWNLOAD_NO_PROGRESS ON
#     DOWNLOAD_EXTRACT_TIMESTAMP OFF
#     CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${GTEST_ROOT}
#     LOG_INSTALL OFF
#     PREFIX ${THIRD_PARTY_DIR}/gtest
#     INSTALL_COMMAND "")
# 
# ExternalProject_Get_Property(gtest source_dir binary_dir)
# 
# if (${COMPILER_SUPPORT_NO_ZERO_AS_NULL})
#     add_compile_options(-Wno-zero-as-null-pointer-constant)
# endif ()
# 
# add_library(libgtest IMPORTED STATIC GLOBAL) 
# add_dependencies(libgtest gtest)
# 
# set_target_properties(libgtest PROPERTIES   IMPORTED_LOCATION ${binary_dir}/lib/libgtest.a 
#                                             INTERFACE_SYSTEM_INCLUDE_DIRECTORIES ${source_dir}/googletest/include)
# 
# add_library(libgmock IMPORTED STATIC GLOBAL)
# 
# add_dependencies(libgmock gtest)
# 
# set_target_properties(libgmock PROPERTIES   IMPORTED_LOCATION ${binary_dir}/lib/libgmock.a 
#                                             INTERFACE_SYSTEM_INCLUDE_DIRECTORIES ${source_dir}/googlemock/include)
# 
# add_library(libgmock_main IMPORTED STATIC GLOBAL) 
# add_dependencies(libgmock_main gtest)
# set_target_properties(libgmock_main PROPERTIES IMPORTED_LOCATION ${binary_dir}/lib/libgmock_main.a)
# 
# if (Threads_FOUND)
#     target_link_libraries(libgmock INTERFACE ${CMAKE_THREAD_LIBS_INIT})
#     target_link_libraries(libgtest INTERFACE ${CMAKE_THREAD_LIBS_INIT})
# else ()
#     target_compile_definitions(libgmock PUBLIC GTEST_HAS_PTHREAD=0)
#     target_compile_definitions(libgtest PUBLIC GTEST_HAS_PTHREAD=0)
# endif ()
# 
# set(GTEST_INCLUDE_DIR ${source_dir}/googletest/include ${source_dir}/googlemock/include
#     CACHE INTERNAL "GTEST_INC")

include(FetchContent)

set(FETCHCONTENT_BASE_DIR "${THIRD_PARTY_DIR}/gtest")
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_Declare(
  gtest_fetch
  DOWNLOAD_EXTRACT_TIMESTAMP OFF
  URL       https://github.com/google/googletest/archive/release-1.12.1.tar.gz
  URL_HASH  SHA256=81964fe578e9bd7c94dfdb09c8e4d6e6759e19967e397dbea48d1c10e45d0df2

)

FetchContent_MakeAvailable(gtest_fetch)

add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest)
add_library(GTest::GMock INTERFACE IMPORTED)
target_link_libraries(GTest::GMock INTERFACE gmock)
add_library(GTest::GTestMain INTERFACE IMPORTED)
target_link_libraries(GTest::GTestMain INTERFACE gtest_main)
add_library(GTest::GMockMain INTERFACE IMPORTED)
target_link_libraries(GTest::GMockMain INTERFACE gmock_main)