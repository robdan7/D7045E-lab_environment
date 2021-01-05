#pragma once
#include <memory>
#include "Texture2D.h"
namespace Engine {
    class Cube_texture2D : public Texture2D{
    public:
        static std::shared_ptr<Cube_texture2D> Create_empty(float width, float height, uint32_t internal_format, uint32_t format, uint32_t type);
    };
}