include(${PROJECT_SOURCE_DIR}/src/Engine/Engine_HEADERS.cmake)

add_executable(Lab4 Lab4.cpp )

target_include_directories(Lab4 PRIVATE ${PROJECT_SOURCE_DIR}/src/Engine)
target_link_libraries(Lab4 PRIVATE Engine)