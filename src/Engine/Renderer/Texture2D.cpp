#include "Texture2D.h"
#include <Platform/OpenGL/GL_Texture2D.h>
namespace Engine {

    std::shared_ptr<Texture2D> Texture2D::Create_empty(float width, float height, uint32_t format, uint32_t type) {
        return std::make_shared<GL_Texture2D>(width, height, format, type);
    }
}