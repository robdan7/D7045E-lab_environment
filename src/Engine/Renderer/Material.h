#pragma once
namespace Engine {
    class Shader;
    class Material {
    public:
        Material(std::shared_ptr<Shader> shader) : m_shader(shader){}
        virtual void bind() const {
            this->m_shader->bind();
        }
        void disable() const {
            this->m_shader->unbind();
        }
    protected:
        std::shared_ptr<Shader> m_shader;
    };
}