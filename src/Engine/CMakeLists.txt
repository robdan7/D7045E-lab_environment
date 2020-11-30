include(Engine_HEADERS.cmake)

add_subdirectory(Core)
add_subdirectory(Platform)
add_subdirectory(Renderer)
add_subdirectory(ImGui)
add_subdirectory(Event)

add_library(Engine STATIC "")
target_link_libraries(Engine PUBLIC Core Renderer OpenGL_renderer Platform glm_static ImGui_layer Engine_Event)
target_include_directories(Engine PUBLIC ${ENGINE_HEADERS})