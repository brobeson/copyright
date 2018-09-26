cmake_minimum_required(VERSION 3.10)

# bail out right now if this is an in-source build, or when attempting to build on an unsupported
# platform.
if (CMAKE_BINARY_DIR MATCHES "${CMAKE_SOURCE_DIR}.*")
    message(FATAL_ERROR "In source builds are not allowed.")
endif()
if (NOT UNIX)
    message(FATAL_ERROR "Your platform is not yet supported.")
endif()

if (NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel")
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo;MinSizeRel")

project(copyright
    VERSION 0
    DESCRIPTION "A tool to maintain copyright information in software source files."
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
add_compile_options(
    -fstack-protector-strong
    -Wall
    -Werror
    -Wpedantic
    -Wextra
    -Wfloat-equal
    -Wshadow
    -Wstack-protector
)

#--------------------------------------------------------------------------------------------------
#                                                    set up clang-tidy to run as part of the build
#--------------------------------------------------------------------------------------------------
find_program(cr_clang_tidy clang-tidy)
mark_as_advanced(cr_clang_tidy)
if (cr_clang_tidy)
    # just the command. everything else is configured in the .clang-tidy file
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy" "-fix-errors")
else()
    message(AUTHOR_WARNING "Clang-tidy is not installed; clang-tidy will not be enabled.")
endif()

#--------------------------------------------------------------------------------------------------
#                                                        set up a build target to run clang-format
#--------------------------------------------------------------------------------------------------
find_program(cr_clang_format clang-format)
mark_as_advanced(cr_clang_format)
if (cr_clang_format)
    # I've seen issues in the past where even minor version changes to clang-format can drastically
    # change the output. So, if clang-format is installed, it must be the exact same version.
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
            COMMENT "Formatting the code..."
        )
    endif()
else()
    message(AUTHOR_WARNING "Clang-format is not installed; the clang-format target will not be enabled.")
endif()

#--------------------------------------------------------------------------------------------------
#                                                            set up a build target to run Cppcheck
#--------------------------------------------------------------------------------------------------
find_program(cr_cppcheck cppcheck)
mark_as_advanced(cr_cppcheck)
if (cr_cppcheck)
    # Using Cppcheck within CMake does not cause the build to fail, even with --error-exitcode. Use
    # the CMake variable so errors can be reported at build time. But also add a target to run
    # Cppcheck with easy to detect --error-exitcode. This makes it easy to fail a CI build if a
    # Cppcheck error exists.
    set(CMAKE_CXX_CPPCHECK
        "cppcheck"
        "--enable=all"
        "--quiet"
        "--relative-paths"
        "--suppress=missingIncludeSystem"
        "--inline-suppr"
        "--error-exitcode=1"
    )
    add_custom_target(cppcheck
        COMMAND ${CMAKE_CXX_CPPCHECK} ./
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Running Cppcheck..."
    )
else()
    message(AUTHOR_WARNING "Cppcheck is not installed; the cppcheck target will not be enabled.")
endif()

#--------------------------------------------------------------------------------------------------
#                                               set up a build target to run Lizard (CCN analysis)
#--------------------------------------------------------------------------------------------------
find_program(cr_lizard lizard)
mark_as_advanced(cr_lizard)
if (cr_lizard)
    add_custom_target(lizard
        COMMAND lizard --CCN 10 --length 30 --arguments 6 --warnings_only --modified ./
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Running Lizard complexity analysis..."
    )
else()
    message(AUTHOR_WARNING "Lizard is not installed; the lizard target will not be generated.")
endif()

find_package(Qt5 REQUIRED COMPONENTS Core)
mark_as_advanced(Qt5Core_DIR Qt5_DIR)

add_subdirectory(application) # the actual copyright application
