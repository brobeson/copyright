add_executable(${PROJECT_NAME}
  application.cpp
  application.hpp
  main.cpp
)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core)
set_target_properties(${PROJECT_NAME}
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

configure_file(copyright.txt "${CMAKE_BINARY_DIR}/copyright.txt" COPYONLY)
