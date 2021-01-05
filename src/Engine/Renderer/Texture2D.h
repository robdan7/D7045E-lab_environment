#pragma once
#include <memory>
namespace Engine {
    class Texture2D {
    public:
        virtual void bind() const = 0;
        virtual void unbind() const = 0;
        virtual uint32_t get_ID() const = 0;
        static std::shared_ptr<Texture2D> Create_empty(float width, float height, uint32_t internal_format, uint32_t format, uint32_t type);
    };

}


