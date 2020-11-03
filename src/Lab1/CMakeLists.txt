include(${PROJECT_SOURCE_DIR}/src/Engine/Engine_HEADERS.cmake)

add_executable(Lab1 Lab1.cpp)

target_include_directories(Lab1 PRIVATE ${PROJECT_SOURCE_DIR}/src/Engine)
target_link_libraries(Lab1 PRIVATE Engine)