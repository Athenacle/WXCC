include(ExternalProject)

ExternalProject_Add(
    argparse URL https://github.com/p-ranav/argparse/archive/refs/tags/v2.9.tar.gz
    URL_HASH SHA256=cd563293580b9dc592254df35b49cf8a19b4870ff5f611c7584cf967d9e6031e
    PREFIX ${THIRD_PARTY_DIR}/argparse
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    BUILD_COMMAND ""
    INSTALL_COMMAND "")

ExternalProject_Get_Property(argparse source_dir binary_dir)

set(ARGPARSE_INC_DIR ${source_dir}/include)
