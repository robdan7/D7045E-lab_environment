include(${PROJECT_SOURCE_DIR}/src/Engine/Engine_HEADERS.cmake)


add_library(Windows_library STATIC "")
target_sources(Windows_library PUBLIC W_window.cpp W_window.h)
target_include_directories(Windows_library PRIVATE ${ENGINE_HEADERS})


target_link_libraries(Windows_library PRIVATE OpenGL_glew Core)
#target_precompile_headers(Windows_library PUBLIC W_window.h)

