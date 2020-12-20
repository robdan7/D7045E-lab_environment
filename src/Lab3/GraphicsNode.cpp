#include "GraphicsNode.h"
namespace Lab3 {

    GraphicsNode::GraphicsNode(std::shared_ptr<Engine::Instanced_mesh> mesh, const glm::mat4 &matrix) : m_transform(matrix), p_mesh(mesh){
        this->m_instance_ID = this->p_mesh->add_instance((float*)&this->m_transform, glm::vec3(this->m_transform[3].x,this->m_transform[3].y,this->m_transform[3].z),1);

    }

    void GraphicsNode::transform(const glm::vec3 &position) {
        this->m_transform = glm::translate(this->m_transform, position);
    }

    void GraphicsNode::rotate(const float &angle, const glm::vec3& axis) {
        this->m_transform = glm::rotate(this->m_transform, angle, axis);
    }

    void GraphicsNode::on_update() {
        this->p_mesh->update_instance((float*)&this->m_transform, 0, this->m_instance_ID);
    }
}