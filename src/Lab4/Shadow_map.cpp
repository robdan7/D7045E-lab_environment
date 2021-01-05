#include "Shadow_map.h"
#include <Platform/OpenGL/OpenGL.h>
#include <Renderer/Texture2D.h>
namespace Lab4 {

    void Shadow_map::bind_write() {
        Engine::GL_Framebuffer::bind_write();
        glViewport(0,0,this->width,this->height);
        glCullFace(GL_FRONT);
    }

    void Shadow_map::bind_read() {
        Engine::GL_Framebuffer::bind_read();
        glViewport(0,0,this->width,this->height);
    }

    void Shadow_map::unbind() {
        Engine::GL_Framebuffer::unbind();
        glViewport(0,0,this->p_window->get_width(), this->p_window->get_height());
        glCullFace(GL_BACK);
    }

    Shadow_map::Shadow_map(int width, int height, std::shared_ptr<Engine::Window> window) : width(width), height(height), p_window(window){
        //this->m_shadow_texture = Engine::Texture2D::Create_empty(width, height, GL_DEPTH24_STENCIL8,GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
        this->set_depth_texture(Engine::Texture2D::Create_empty(width, height, GL_DEPTH24_STENCIL8,GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8));
    }

    int Shadow_map::get_width() {
        return 0;
    }

    int Shadow_map::get_height() {
        return 0;
    }

    const std::shared_ptr<const Engine::Texture2D> Shadow_map::get_gexture() {
        return this->p_textures.at(0);
    }
}