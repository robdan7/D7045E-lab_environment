#include "GL_Texture2D.h"
#include "OpenGL.h"
namespace Engine {


    void GL_Texture2D::bind() {
        glBindTexture(GL_TEXTURE_2D, this->m_ID);

    }

    void GL_Texture2D::unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GL_Texture2D::GL_Texture2D(const float &width, const float &height, uint32_t internal_format,uint32_t format, uint32_t type) {

        glGenTextures(1, &this->m_ID);
        glBindTexture(GL_TEXTURE_2D, this->m_ID);
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format,type, NULL);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glBindTexture(GL_TEXTURE_2D, 0);

    }

    uint32_t GL_Texture2D::get_ID() {
        return this->m_ID;
    }

    GL_Texture2D::~GL_Texture2D() {
        glDeleteTextures(1,&this->m_ID);
    }


}
