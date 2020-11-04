include(${PROJECT_SOURCE_DIR}/src/Engine/Engine_HEADERS.cmake)

# --- Add core library ---
add_library(Core STATIC "")
target_sources(Core PUBLIC Window.cpp)
target_include_directories(Core PRIVATE ${ENGINE_HEADERS})
target_link_libraries(Core PUBLIC Renderer PRIVATE Platform)

# Only precompile PUBLIC headers if they are used outside of Engine_Core.
target_precompile_headers(Core PUBLIC Window.h )
