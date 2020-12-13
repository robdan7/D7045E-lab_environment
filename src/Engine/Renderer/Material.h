#pragma once
namespace Engine {
    class Shader;
    /**
     * Not sure if material belongs to the render folder.
     * TODO move this, maybe
     */
    class Material {
    public:
        Material(std::shared_ptr<Shader> shader) : m_shader(shader){}
        virtual void bind() {
            this->m_shader->bind();
        }
        void disable() {
            this->m_shader->unbind();
        }
    private:
        std::shared_ptr<Shader> m_shader;
    };
}