#include "Shadowmap_material.h"
namespace Lab4 {
    void Shadowmap_material::bind() const {
        Material::bind();
        auto index= glGetUniformBlockIndex(this->m_shader->get_ID(), "shadow_view_matrix");
        glUniformMatrix4fv(index,1,GL_FALSE,(float*)&this->p_camera->get_view_projection_matrix());
    }

    Shadowmap_material::Shadowmap_material(std::shared_ptr<Engine::Shader> shader,
                                           std::shared_ptr<Engine::Camera> camera) : Engine::Material(shader),p_camera(camera){

    }
}

