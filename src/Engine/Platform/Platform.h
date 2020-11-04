#pragma once

#include <Platform/OpenGL/GL_context.h>
#include "Renderer/Renderer_API.h"

namespace Engine {
/// The following functions creates the graphics API
    GLFW_context* create_engine_context() {
        #ifdef API_OPENGL   // Must be defined by the user.
            return new Engine::GL_context();
            #else
                //CORE_ASSERT(false, "Could not find a valid API context!")
                std::cout << "Could not find a valid API context!" << std::endl;
                return nullptr;
        #endif
    }

    Render::Renderer_API::API get_API(){
        #ifdef API_OPENGL   // Must be defined by the user.
            return Engine::Render::Renderer_API::API::OpenGL;
        #else
            //CORE_ASSERT(false, "Could not find a valid API context!")
            std::cout << "Could not find a valid API context!" << std::endl;
            return Engine::Render::Renderer_API::API::None;
        #endif
    }
}
