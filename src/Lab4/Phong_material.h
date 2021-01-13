#pragma once
#include <Renderer/Material.h>
#include <memory>
#include <glm/glm.hpp>
namespace Lab4 {
    /**
     * Phong materials take in 4 instance parameters: Instance transform, diffuse color, specular power and shininess.
     * Make sure these are present in your instance buffer.
     */
    class Phong_material : public Engine::Material {
    public:
        static std::shared_ptr<Phong_material> Create();
    private:
        Phong_material(std::shared_ptr<Engine::Shader> shader);

    };
}