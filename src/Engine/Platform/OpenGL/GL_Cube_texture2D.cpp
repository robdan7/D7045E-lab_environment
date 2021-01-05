#include "GL_Cube_texture2D.h"
namespace Engine {

    void GL_Cube_texture2D::bind() const {
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->m_ID);
    }

    void GL_Cube_texture2D::unbind() const {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    uint32_t GL_Cube_texture2D::get_ID() const {
        return this->m_ID;
    }

    GL_Cube_texture2D::GL_Cube_texture2D(float width, float height, uint32_t internal_format, uint32_t format,
                                         uint32_t type) {
        glGenTextures(1, &this->m_ID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->m_ID);
        for (unsigned int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, internal_format, width, height, 0, format,type, NULL);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    GL_Cube_texture2D::~GL_Cube_texture2D() {
        glDeleteTextures(1,&this->m_ID);
    }
}