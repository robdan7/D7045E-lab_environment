add_library(Renderer STATIC
        Renderer.cpp
        Buffer.cpp
        Vertex_array.cpp
        Renderer_API.cpp
        Camera.cpp
        Shader.cpp
        Render_encoder.cpp
        Frustum_compute_query.cpp
        FrameBuffer.cpp
        Texture2D.cpp
        )
target_link_libraries(Renderer PUBLIC glm_static PRIVATE OpenGL_renderer)
target_include_directories(Renderer PRIVATE ${ENGINE_HEADERS} PUBLIC ${PROJECT_SOURCE_DIR}/include/glm)
target_precompile_headers(Renderer PUBLIC Renderer_API.h Renderer.h Vertex_array.h Camera.h Buffer.h Render_encoder.h)