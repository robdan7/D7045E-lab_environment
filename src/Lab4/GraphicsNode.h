#include "glm/glm.hpp"
#include <memory>
#include <Assets/Instanced_mesh.h>
namespace Lab3 {
    class GraphicsNode {
    public:
        GraphicsNode(std::shared_ptr<Engine::Instanced_mesh> mesh, const glm::mat4& matrix = glm::mat4(1));
        void transform(const glm::vec3 &position);
        void rotate(const float &angle, const glm::vec3& axis);
        void on_update();
    private:
        std::shared_ptr<Engine::Instanced_mesh> p_mesh;
        glm::mat4 m_transform;
        uint32_t m_instance_ID = 0;
    };
}
