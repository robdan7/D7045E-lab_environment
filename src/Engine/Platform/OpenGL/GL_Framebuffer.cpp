#include "GL_Framebuffer.h"
#include <Renderer/Texture2D.h>
namespace Engine {

    void GL_Framebuffer::bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, this->m_ID);
    }

    void GL_Framebuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    void GL_Framebuffer::set_depth_texture(std::shared_ptr<Texture2D> texture) {
        this->bind();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->get_ID());
        this->unbind();
    }

    void GL_Framebuffer::add_color_texture(std::shared_ptr<Texture2D> texture) {
        this->bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+(this->m_color_textures++), GL_TEXTURE_2D, texture->get_ID(), 0);
        this->unbind();
    }

    GL_Framebuffer::~GL_Framebuffer() {
        glDeleteFramebuffers(1, &this->m_ID);
    }

    GL_Framebuffer::GL_Framebuffer() {
        glCreateFramebuffers(1,&this->m_ID);
    }
}