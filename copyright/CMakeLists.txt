cmake_minimum_required(VERSION 3.10)

project(copyright
  VERSION 0
  DESCRIPTION "A tool to maintain copyright information in software source files."
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)

find_package(Qt5
  REQUIRED
  COMPONENTS
    Core
)

add_executable(${PROJECT_NAME}
  application.cpp
  application.hpp
  main.cpp
)
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    Qt5::Core
)
set_target_properties(${PROJECT_NAME}
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

configure_file(copyright.txt "${CMAKE_BINARY_DIR}/copyright.txt" COPYONLY)
