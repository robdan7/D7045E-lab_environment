include(${PROJECT_SOURCE_DIR}/src/Engine/Engine_HEADERS.cmake)


add_library(Assets STATIC Instanced_mesh.cpp)
target_include_directories(Assets PRIVATE ${ENGINE_HEADERS})
target_link_libraries(Assets PRIVATE Renderer)