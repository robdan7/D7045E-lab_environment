#pragma once
#include <Renderer/Cube_texture2D.h>
namespace Engine {
    class GL_Cube_texture2D : public Cube_texture2D {
    public:
        GL_Cube_texture2D(float width, float height, uint32_t internal_format, uint32_t format, uint32_t type);
        ~GL_Cube_texture2D();
        void bind() const override;

        void unbind() const override;

        uint32_t get_ID() const override;
    private:
        uint32_t m_ID;
    };
}
