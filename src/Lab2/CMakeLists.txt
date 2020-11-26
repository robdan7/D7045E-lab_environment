include(${PROJECT_SOURCE_DIR}/src/Engine/Engine_HEADERS.cmake)

add_executable(Lab2 Lab2.cpp Vertex.cpp Algorithms.cpp Triangle.cpp Search_tree.cpp)

target_include_directories(Lab2 PRIVATE ${PROJECT_SOURCE_DIR}/src/Engine)
target_link_libraries(Lab2 PRIVATE Engine)