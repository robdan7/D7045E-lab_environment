#pragma once
#include <Core/Window.h>
#include <Platform/OpenGL/GL_Framebuffer.h>
namespace Lab4 {
class Shadow_map : public Engine::GL_Framebuffer{
    public:
        Shadow_map(int width, int height, std::shared_ptr<Engine::Window> window);

        void bind_write() override;

        void bind_read() override;

        void unbind() override;
        int get_width();
        int get_height();
        const std::shared_ptr<const Engine::Texture2D> get_gexture();
    private:
        int width, height;
        std::shared_ptr<Engine::Window> p_window;
    };
}
