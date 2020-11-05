add_library(OpenGL_glew INTERFACE)

if(NOT CMAKE_CL_64)
    target_link_directories(OpenGL_glew INTERFACE ${PROJECT_SOURCE_DIR}/include/glew-2.1.0/lib/Release/Win32)
    target_include_directories(OpenGL_glew INTERFACE ${PROJECT_SOURCE_DIR}/include/glew-2.1.0/include)

    target_include_directories(OpenGL_glew INTERFACE ${PROJECT_SOURCE_DIR}/include/glfw-3.3.2x86/include)
    if (MSVC)
        target_link_directories(OpenGL_glew INTERFACE ${PROJECT_SOURCE_DIR}/include/glfw-3.3.2x86/lib-vc2019)
    else()
        target_link_directories(OpenGL_glew INTERFACE ${PROJECT_SOURCE_DIR}/include/glfw-3.3.2x86/lib-mingw)
    endif()

else()
    target_link_directories(OpenGL_glew INTERFACE ${PROJECT_SOURCE_DIR}/include/glew-2.1.0/lib/Release/x64)
    target_include_directories(OpenGL_glew INTERFACE ${PROJECT_SOURCE_DIR}/include/glew-2.1.0/include)

    target_include_directories(OpenGL_glew INTERFACE ${PROJECT_SOURCE_DIR}/include/glfw-3.3.2x64/include)
#    if (CMAKE_GENERATOR MATCHES "Visual Studio")
        target_link_directories(OpenGL_glew INTERFACE ${PROJECT_SOURCE_DIR}/include/glfw-3.3.2x64/lib-vc2019)
#    else()
#        target_link_directories(OpenGL_glew INTERFACE ${PROJECT_SOURCE_DIR}/include/glfw-3.3.2x64/lib-mingw-w64)
#    endif()
endif()

target_link_libraries(OpenGL_glew INTERFACE glew32s glfw3 gdi32 ${OPENGL_LIBS})
target_precompile_headers(OpenGL_glew INTERFACE OpenGL.h)

add_library(OpenGL_renderer STATIC GL_context.cpp GL_shader.cpp GL_buffer.cpp GL_vertex_array.cpp GL_renderer_API.cpp)
target_link_libraries(OpenGL_renderer PUBLIC Renderer PRIVATE OpenGL_glew glm_static)
target_include_directories(OpenGL_renderer PRIVATE ${ENGINE_HEADERS})
target_include_directories(OpenGL_renderer PUBLIC ${PROJECT_SOURCE_DIR}/include/glm)
target_precompile_headers(OpenGL_renderer PUBLIC GL_shader.h GL_buffer.h GL_context.h GL_renderer_API.h GL_vertex_array.h)



#add_library(ImGui INTERFACE)
