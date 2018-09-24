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

find_package(Qt5 REQUIRED COMPONENTS Core)
add_subdirectory(application) # the actual copyright application
