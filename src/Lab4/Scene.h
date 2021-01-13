#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <Assets/Instanced_mesh.h>
#include "Point_light.h"
#include <Renderer/Light.h>
namespace Lab4 {
    class Node {
    public:
        virtual void on_update(const glm::mat4& parent_M = glm::mat4(1));
    protected:
        glm::mat4 M;
        glm::mat4 PM = glm::mat4(1);
    };

    class InnerNode : public Node {
    public:
        InnerNode() {}
        InnerNode(const glm::mat4 matrix) {
            Node::M = matrix;
        }
        void add_node(std::shared_ptr<Node> node);
        void on_update(const glm::mat4& parent_M = glm::mat4(1)) override;

    private:
        std::vector<std::shared_ptr<Node>> m_nodes;
    };

    class GeometryNode : public InnerNode {
    public:
        GeometryNode(std::shared_ptr<Engine::Instanced_mesh> mesh, const void* instance_data, const glm::mat4& transform = glm::mat4(1));
        void translate(const glm::vec3 &position);
        void rotate(const float &angle, const glm::vec3& axis);
        void rotate(const glm::quat& rotation);
        void scale(const glm::vec3& scale);
        void reset_transform();
        void on_update(const glm::mat4& parent_M = glm::mat4(1)) override;
    protected:
        GeometryNode() {}
    protected:
        std::shared_ptr<Engine::Instanced_mesh> p_mesh;
        uint32_t m_instance_ID = 0;
    };

    class CameraNode : public Node {

    };

class LightNode : public GeometryNode, public Engine::Point_light {
    public:
        /**
         * @param light
         * @param color
         * @param position
         * @param linear - Linear dropoff.
         * @param quadratic - Quadratic dropoff.
         * @param constant - Constant dropoff. Leave this for a max attenuation of 1.
         */
        LightNode(std::shared_ptr<Lab4::GL_Point_light> light, const glm::vec3 color, glm::vec4 position, float radius, float exp_dropoff);
    private:
        uint32_t light_ID;
    };
}
