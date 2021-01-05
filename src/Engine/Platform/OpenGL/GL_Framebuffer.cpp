#include "GL_Framebuffer.h"
#include <Renderer/Texture2D.h>
namespace Engine {

    void GL_Framebuffer::bind_write() {
        glBindFramebuffer(GL_FRAMEBUFFER, this->m_ID);
    }

    void GL_Framebuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        for (int i = 0; i < this->p_textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0+i);
            this->p_textures[i]->unbind();
        }
    }


    void GL_Framebuffer::set_depth_texture(std::shared_ptr<const Texture2D> texture) {
        glBindFramebuffer(GL_FRAMEBUFFER, this->m_ID);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, texture->get_ID(), 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        this->p_textures.push_back(texture);
        //this->m_color_textures++;

    }

    void GL_Framebuffer::add_write_color_texture(const std::shared_ptr<const Texture2D> texture) {
        this->bind_write();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+(this->m_color_textures), GL_TEXTURE_2D, texture->get_ID(), 0);
        this->m_draw_buffers.push_back(GL_COLOR_ATTACHMENT0+this->m_color_textures);
        this->p_textures.push_back(texture);
        this->m_color_textures++;
        glDrawBuffers(this->m_draw_buffers.size(), &this->m_draw_buffers[0]);

        this->unbind();
    }

    GL_Framebuffer::~GL_Framebuffer() {
        glDeleteFramebuffers(1, &this->m_ID);
    }

    GL_Framebuffer::GL_Framebuffer() {
        glCreateFramebuffers(1,&this->m_ID);
    }

    void GL_Framebuffer::bind_read() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        for (int i = 0; i < this->p_textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0+i);
            this->p_textures[i]->bind();

        }
    }

    void GL_Framebuffer::add_read_color_texture(std::shared_ptr<const Texture2D> texture) {
        this->p_textures.push_back(texture);
    }
}