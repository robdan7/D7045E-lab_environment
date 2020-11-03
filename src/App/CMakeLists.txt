include(${PROJECT_SOURCE_DIR}/src/Engine/Engine_HEADERS.cmake)

add_executable(App Sandbox.cpp)

target_include_directories(App PRIVATE ${PROJECT_SOURCE_DIR}/src/Engine)
target_link_libraries(App PRIVATE Engine)