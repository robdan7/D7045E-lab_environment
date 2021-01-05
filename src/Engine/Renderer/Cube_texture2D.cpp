#include "Cube_texture2D.h"
#include <Platform/OpenGL/GL_Cube_texture2D.h>
namespace Engine {

    std::shared_ptr<Cube_texture2D>
    Cube_texture2D::Create_empty(float width, float height, uint32_t internal_format, uint32_t format, uint32_t type) {
        return std::make_shared<GL_Cube_texture2D>(width, height, internal_format, format, type);
    }
}