cmake_minimum_required(VERSION 3.10)

# bail out right now if this is an in-source build, or when attempting to build on an unsupported
# platform.
if (CMAKE_BINARY_DIR MATCHES "${CMAKE_SOURCE_DIR}.*")
    message(FATAL_ERROR "In source builds are not allowed.")
endif()
if (NOT UNIX)
    message(FATAL_ERROR "Your platform is not yet supported.")
endif()

project(copyright
    VERSION 0
    DESCRIPTION "A tool to maintain copyright information in software source files."
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

# set up clang-tidy to run as part of the build
find_program(cr_clang_tidy clang-tidy)
if (cr_clang_tidy)
    # just the command. everything else is configured in the .clang-tidy file
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy" "-fix-errors")
endif()

# set up clang-format
# I've seen issues in the past where even minor version changes to clang-format can drastically
# change the output. So, if clang-format is installed, it must be the exact same version.
find_program(cr_clang_format clang-format)
if (cr_clang_format)
    execute_process(
        COMMAND clang-format -version
        OUTPUT_VARIABLE cr_clang_format_version
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if (NOT cr_clang_format_version MATCHES "clang-format version 6.0.0.*")
        message(WARNING "Clang-format version does not match the official version. Clang-format target disabled.")
    else()
        # run clang-format on all the header and source files in the repository
        file(GLOB_RECURSE cr_source_files
            LIST_DIRECTORIES false
            RELATIVE "${CMAKE_SOURCE_DIR}"
            "*.[ch]pp"
        )
        add_custom_target(format
            COMMAND clang-format -i -style=file ${cr_source_files}
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            COMMENT "Formatting the code."
        )
    endif()
endif()

find_package(Qt5 REQUIRED COMPONENTS Core)
add_subdirectory(application) # the actual copyright application
