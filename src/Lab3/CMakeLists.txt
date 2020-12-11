include(${PROJECT_SOURCE_DIR}/src/Engine/Engine_HEADERS.cmake)

add_executable(Lab3 Lab3.cpp GraphicsNode.cpp Monochrome_material.cpp)

target_include_directories(Lab3 PRIVATE ${PROJECT_SOURCE_DIR}/src/Engine)
target_link_libraries(Lab3 PRIVATE Engine)