#pragma once
#include <Renderer/Light.h>
#include <memory>
#include <Renderer/FrameBuffer.h>
#include <Assets/Instanced_mesh.h>
namespace Lab4 {
class GL_Point_light : public Engine::Instanced_mesh {
public:
    /**
     *
     * @param color
     * @param position
     * @param linear - Linear dropoff
     * @param quadratic
     * @param continuous - Continuous dropoff.
     * @return
     */
    uint32_t add_instance(glm::vec3  color, glm::vec3 position, float linear, float quadratic,float continuous=1);

    static std::shared_ptr<GL_Point_light> Create(uint32_t reserved_instances);
private:
        GL_Point_light(const std::initializer_list<Engine::Buffer_element>& instance_layout, const uint32_t& reserved_instances);
    };

}
