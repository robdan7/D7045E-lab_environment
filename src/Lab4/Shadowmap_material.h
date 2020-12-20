#pragma once
#include <Renderer/Material.h>
#include <Renderer/Shader.h>
#include <Renderer/Camera.h>
namespace Lab4 {
    class Shadowmap_material : public Engine::Material {
    public:
        Shadowmap_material(std::shared_ptr<Engine::Shader> shader, std::shared_ptr<Engine::Camera> camera);

        void bind() override;
    private:
        std::shared_ptr<Engine::Camera> p_camera;
    };
}

