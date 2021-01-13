#include "Scene.h"

namespace Lab4 {

    void Node::on_update(const glm::mat4 &parent_M) {
        this->PM = parent_M* this->M;
    }

    void InnerNode::on_update(const glm::mat4 &parent_M) {
        Node::on_update(parent_M);
        for (auto& child : this->m_nodes) {
            child->on_update(this->PM);
        }
    }

    void InnerNode::add_node(std::shared_ptr<Node> node) {
        this->m_nodes.push_back(node);
    }

    GeometryNode::GeometryNode(std::shared_ptr<Engine::Instanced_mesh> mesh, const void* instance_data, const glm::mat4 &transform) : p_mesh(mesh) {
        this->M = transform;
        std::vector<char> memory(mesh->get_instance_stride());
        uint32_t instance_stride = mesh->get_instance_stride()-sizeof(transform);


        std::copy((char*)instance_data,(char*)instance_data+instance_stride, memory.begin());

        std::copy((char*)&transform[0],(char*)&transform[0]+sizeof(transform),memory.begin()+instance_stride);
        this->m_instance_ID = this->p_mesh->add_instance(&memory[0]);
    }

    void GeometryNode::translate(const glm::vec3 &position) {
        this->M = glm::translate(this->M, position);
    }

    void GeometryNode::rotate(const float &angle, const glm::vec3& axis) {
        this->M = glm::rotate(this->M, angle, axis);
    }

    void GeometryNode::on_update(const glm::mat4 &parent_M) {
        InnerNode::on_update(parent_M);

        this->p_mesh->update_instance((char*)&this->PM, this->p_mesh->get_instance_stride()-sizeof(this->PM), this->m_instance_ID);
    }

    void GeometryNode::reset_transform() {
        this->M = glm::mat4(1);
    }

    void GeometryNode::rotate(const glm::quat &rotation) {
        this->M = this->M * glm::toMat4(rotation);
    }

    void GeometryNode::scale(const glm::vec3 &scale) {
        this->M = glm::scale(this->M, scale);
    }

    LightNode::LightNode(std::shared_ptr<Lab4::GL_Point_light> light, const glm::vec3 color, glm::vec4 position, float radius, float exp_dropoff) : Engine::Point_light(color,position,radius,exp_dropoff,false){
        this->M = glm::translate(glm::mat4(1),glm::vec3(position.x,position.y,position.z));
        this->M = glm::scale(this->M, glm::vec3(radius,radius,radius));

        std::vector<char> memory(light->get_instance_stride());
        uint32_t instance_stride = light->get_instance_stride()-sizeof(this->M);

        float data[] = {
                color.r,color.g,color.b,
                radius,
                exp_dropoff
        };
        std::copy((char*)data,(char*)data+instance_stride, memory.begin());
        std::copy((char*)&this->M[0],(char*)&this->M[0]+sizeof(this->M),memory.begin()+instance_stride);
        this->p_mesh = light;
        this->m_instance_ID = this->p_mesh->add_instance(&memory[0]);
    }
}