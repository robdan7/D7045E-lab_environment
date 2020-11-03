# --- Add ImGui library ---
file(GLOB IMGUI_CORE ${PROJECT_SOURCE_DIR}/include/ImGui-dock/*.cpp)
file(GLOB IMGUI_EXAMPLES ${PROJECT_SOURCE_DIR}/include/ImGui-dock/examples/*.cpp)
add_library(ImGui_SOURCE STATIC ${IMGUI_CORE} ${PROJECT_SOURCE_DIR}/include/ImGui-dock/examples/imgui_impl_opengl3.cpp ${PROJECT_SOURCE_DIR}/include/ImGui-dock/examples/imgui_impl_glfw.cpp)
target_sources(ImGui_SOURCE PUBLIC ${PROJECT_SOURCE_DIR}/include/ImGui-dock/imgui.cpp
        ${PROJECT_SOURCE_DIR}/include/ImGui-dock/imgui_draw.cpp
        ${PROJECT_SOURCE_DIR}/include/ImGui-dock/imgui_widgets.cpp
        ${PROJECT_SOURCE_DIR}/include/ImGui-dock/imgui_demo.cpp
        #PUBLIC imgui_impl_opengl3.cpp imgui_impl_opengl3.h
        )
target_link_libraries(ImGui_SOURCE PUBLIC OpenGL_glew)
target_include_directories(ImGui_SOURCE PUBLIC ${PROJECT_SOURCE_DIR}/include/ImGui-dock)

add_library(ImGui_layer STATIC "")
target_sources(ImGui_layer PRIVATE ImGui_layer.cpp)
target_link_libraries(ImGui_layer PUBLIC ImGui_SOURCE PRIVATE Core)
target_include_directories(ImGui_layer PRIVATE ${ENGINE_HEADERS} ${PROJECT_SOURCE_DIR}/include/ImGui-dock)
target_precompile_headers(ImGui_layer PUBLIC ImGui_layer.h)