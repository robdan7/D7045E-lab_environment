#pragma once
#include <Renderer/Shader.h>
namespace Engine {
    class Material {
    public:
        Material(std::shared_ptr<Shader> shader) : m_shader(shader){}
        virtual void apply_material() {
            this->m_shader->bind();
        }
        void disable() {
            this->m_shader->unbind();
        }
    private:
        std::shared_ptr<Shader> m_shader;
    };
}