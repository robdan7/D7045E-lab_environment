#pragma once
#include <Renderer/FrameBuffer.h>
namespace Engine {
    class GL_Framebuffer : public FrameBuffer {
    public:
        GL_Framebuffer();
        ~GL_Framebuffer();
        void bind_write() override;

        void bind_read() override;

        void unbind() override;

        void add_read_color_texture(std::shared_ptr<const Texture2D> texture) override;

        void set_depth_texture(std::shared_ptr<const Texture2D> texture) override;

        void add_write_color_texture(std::shared_ptr<const Texture2D> texture) override;
    protected:
        uint32_t m_ID;
        uint32_t m_color_textures = 0;
        std::vector<std::shared_ptr<const Texture2D>> p_textures;
        std::vector<uint32_t> m_draw_buffers;
    };

}