#pragma once
#include <Renderer/Texture2D.h>
namespace Engine {
    /**
     * Simple GL texture class with minimal features. No special filtering can be applied, yet.
     * This does not support texture loading from images or mipmaping.
     */
    class GL_Texture2D : public Texture2D{
    public:
        GL_Texture2D(const float& width, const float& height, uint32_t format, uint32_t type);
        ~GL_Texture2D();
        void bind() override;
        void unbind() override;

    private:
        uint32_t get_ID() override;

    private:
        uint32_t m_ID;
    };
}

