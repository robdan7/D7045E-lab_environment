include(${PROJECT_SOURCE_DIR}/src/Engine/Engine_HEADERS.cmake)
add_library(Engine_Event STATIC Event_system.cpp)
#target_precompile_headers(Boggle_Event PUBLIC Event_actor.h Event_actor_mt.h Event_actor_st.h Event_system.h System_events.h)
target_include_directories(Engine_Event PRIVATE ${ENGINE_HEADERS})