#pragma once
#include <Renderer/FrameBuffer.h>
namespace Engine {
    class GL_Framebuffer : public FrameBuffer {
    public:
        GL_Framebuffer();
        ~GL_Framebuffer();
        void bind() override;

        void unbind() override;


        void set_depth_texture(std::shared_ptr<Texture2D> texture) override;

        void add_color_texture(std::shared_ptr<Texture2D> texture) override;
    private:
        uint32_t m_ID;
        uint32_t m_color_textures = 0;
    };

}