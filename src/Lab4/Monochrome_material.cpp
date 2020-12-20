#include "Monochrome_material.h"
#include <Platform/OpenGL/OpenGL.h>
namespace Lab4 {

    void Monochrome_material::bind() {
        Material::bind();
        glUniform4f(1,this->m_color.r, this->m_color.g, this->m_color.b,this->m_color.a);
    }

    Monochrome_material::Monochrome_material(std::shared_ptr<Engine::Shader> shader, const glm::vec4 &color) : Material(shader), m_color(color) {
        this->bind();
    }
}
