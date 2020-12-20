#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <Assets/Instanced_mesh.h>
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

    class GeometryNode : public Node {
    public:
        GeometryNode(std::shared_ptr<Engine::Instanced_mesh> mesh, const void* instance_data, const glm::mat4& transform = glm::mat4(1));
        void transform(const glm::vec3 &position);
        void rotate(const float &angle, const glm::vec3& axis);
        void on_update(const glm::mat4& parent_M = glm::mat4(1)) override;
    private:
        std::shared_ptr<Engine::Instanced_mesh> p_mesh;
        uint32_t m_instance_ID = 0;
    };

    class CameraNode : public Node {

    };
}
