include(FetchContent)

set(FETCHCONTENT_BASE_DIR "${THIRD_PARTY_DIR}/fmt")
FetchContent_Declare(
    fmt_fetch DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/fmtlib/fmt/archive/refs/tags/9.1.0.tar.gz
    URL_HASH SHA256=5dea48d1fcddc3ec571ce2058e13910a0d4a6bab4cc09a809d8b1dd1c88ae6f2)

FetchContent_MakeAvailable(fmt_fetch)
add_library(Fmt INTERFACE IMPORTED)
target_link_libraries(Fmt INTERFACE fmt)
