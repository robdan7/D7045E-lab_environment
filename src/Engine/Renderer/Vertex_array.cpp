#include "Vertex_array.h"
#include "Renderer.h"
#include <Platform/OpenGL/GL_vertex_array.h>
namespace Engine {
    Engine::Vertex_array::~Vertex_array() {

    }

    std::shared_ptr<Vertex_array> Engine::Vertex_array::Create() {
        switch (Render::Renderer::get_API()) {
            case Render::Renderer_API::API::None :
                //CORE_ASSERT(false, "Renderer_API:: No render API is defined!");
                std::cout << "Renderer_API:: No render API is defined!" << std::endl;
                break;
            case Render::Renderer_API::API::OpenGL:
                return std::static_pointer_cast<Vertex_array>(std::make_shared<GL_vertex_array>());
        }
        //CORE_ASSERT(false, "Unknown Renderer_API!");
        std::cout << "Renderer_API:: No render API is defined!" << std::endl;
        return nullptr;
    }
}

