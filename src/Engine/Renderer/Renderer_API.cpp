#include "Renderer_API.h"
#include <Core/Core.h>
namespace Engine {
    namespace Render {
        Renderer_API::API Renderer_API::s_API = Engine::get_API();
    }
}