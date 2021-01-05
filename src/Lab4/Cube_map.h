#pragma once
#include <Platform/OpenGL/GL_Framebuffer.h>
#include <Platform/OpenGL/GL_Cube_texture2D.h>
#include <Core/Window.h>
#include <Renderer/Camera.h>
#include <Renderer/Shader.h>
#include <Renderer/Material.h>
namespace Lab4{
class Cube_map : public Engine::GL_Framebuffer{
    public:
        Cube_map(int width, int height, std::shared_ptr<Engine::Window> window);

        void bind_write() override;

        void bind_read() override;

        void unbind() override;
        int get_width();
        int get_height();
        const std::shared_ptr<const Engine::Texture2D> get_gexture();
        std::shared_ptr<const Engine::Material> get_material();
    private:
        int width, height;
        std::shared_ptr<Engine::Window> p_window;
        //std::vector<Engine::Perspective_camera> m_cameras;
        std::vector<glm::mat4> m_camera_matrices;
        //std::shared_ptr<Engine::Shader> m_shader;
        std::shared_ptr<Engine::Material> m_material;
        //std::shared_ptr<Engine:> m_shadow_texture;
    };

}

