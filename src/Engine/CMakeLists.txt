include(Engine_HEADERS.cmake)

add_subdirectory(Core)
add_subdirectory(Platform)
add_subdirectory(Renderer)

add_library(Engine STATIC "")
target_link_libraries(Engine PUBLIC Core Renderer Platform glm_static)
target_include_directories(Engine PUBLIC ${ENGINE_HEADERS})