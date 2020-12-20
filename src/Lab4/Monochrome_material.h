#pragma once
#include <Renderer/Material.h>
#include <memory>
#include <glm/glm.hpp>
namespace Lab4 {
    class Monochrome_material : public Engine::Material {
    public:
        Monochrome_material(std::shared_ptr<Engine::Shader> shader, const glm::vec4& color);
        void bind() override;

    private:
        glm::vec4 m_color;

    };
}