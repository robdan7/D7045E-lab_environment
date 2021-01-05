#include "Instanced_mesh.h"
#include <stdint.h>

namespace Engine {
    #define MAX std::numeric_limits<uint32_t>::max()
    Instanced_mesh::Instanced_mesh(const std::initializer_list<Buffer_element> &instance_layout, const uint32_t& reserved_instances) : m_last_deleted(MAX), m_ID_pointers(4){
        auto layout = Engine::Buffer_layout{instance_layout};
        this->m_instance_buffer_data = std::shared_ptr<Vertex_buffer>(Vertex_buffer::Create(nullptr, reserved_instances*layout.get_stride(), Vertex_buffer::Access_frequency::DYNAMIC, Vertex_buffer::Access_type::DRAW));
        this->m_instance_buffer_data->set_layout(instance_layout);
/*
        this->m_instance_bounding_spheres = std::shared_ptr<Vertex_buffer>(Vertex_buffer::Create(nullptr,reserved_instances*sizeof(float)*4, Vertex_buffer::Access_frequency::DYNAMIC, Vertex_buffer::Access_type::DRAW));
        this->m_instance_bounding_spheres->set_layout(Buffer_layout{{Shader_data::Float4, "position"}});

        this->m_frustum_pipeline = Frustum_compute_query::Create(m_instance_buffer_data, m_instance_bounding_spheres);
        */
    }

    void Instanced_mesh::add_LOD(Import_mesh& mesh, const std::shared_ptr<Material>& material) {
        //Mesh_data* mesh = Mesh_data::Create(stream);
        this->m_lod.emplace_back(mesh.m_vertex_buffer);

        Engine::Buffer_layout layout;
        if (mesh.draw_type == Draw_type::STREAM) {
            layout = {
                    {Engine::Shader_data::Float3, "position"},
                    {Engine::Shader_data::Float3, "normal"}
            };
        } else {
            layout = {
                    {Engine::Shader_data::Float3, "position"}
            };
        }
        this->m_lod.back().p_buffer->set_layout(layout);


        for (auto& mesh_material : mesh.materials) {
            if (mesh.draw_type == Draw_type::STREAM) {
                this->m_lod.back().mesh_partitions.emplace_back(Engine::Vertex_array::Create(), material, mesh_material.starting_vertex, mesh_material.vertices);
            } else {
                this->m_lod.back().mesh_partitions.emplace_back(Engine::Vertex_array::Create(), material);
                /// TODO Change STATIC to a parameter
                this->m_lod.back().mesh_partitions.back().m_vao->set_index_buffer(Engine::Index_buffer::Create(
                        &mesh_material.vertex_index[0], mesh_material.vertex_index.size() * sizeof(uint32_t), Engine::Raw_buffer::Access_frequency::STATIC));
            }
            this->m_lod.back().mesh_partitions.back().m_vao->add_vertex_buffer(this->m_lod.back().p_buffer, 0);
            this->m_lod.back().mesh_partitions.back().m_vao->add_vertex_buffer(this->m_instance_buffer_data, 1);

        }
        //delete mesh;
    }

    uint32_t Instanced_mesh::add_instance(void *data) {
        uint32_t ID;
        if (this->m_last_deleted == MAX) {
            ID = this->m_instance_IDs.size();
            this->m_ID_pointers.put(this->m_instance_IDs.size());
            this->m_instance_IDs.push_back(this->m_instance_IDs.size());
        } else {
            ID = this->m_last_deleted;
            auto previous_deleted = this->m_instance_IDs[this->m_last_deleted];
            this->m_ID_pointers.put(this->m_last_deleted);
            this->m_instance_IDs[this->m_last_deleted] = this->m_last_deleted;
            this->m_last_deleted = previous_deleted;
        }


        auto instance_data_size = this->m_instance_buffer_data->get_layout().get_stride();
        if (this->m_instance_buffer_data->get_size() < (this->m_instances+1)*instance_data_size) {
            this->m_instance_buffer_data->resize_buffer(this->m_instance_buffer_data->get_size()+instance_data_size);
            //this->m_instance_bounding_spheres->resize_buffer(this->m_instance_bounding_spheres->get_size() + this->m_instance_bounding_spheres->get_layout().get_stride());
        }

        this->m_instance_buffer_data->set_sub_data(data, this->m_instances * instance_data_size, instance_data_size);
        //float sphere[] = {position.x, position.y, position.z, radius};
        //this->m_instance_bounding_spheres->set_sub_data(sphere, this->m_instances * m_instance_bounding_spheres->get_layout().get_stride(), m_instance_bounding_spheres->get_layout().get_stride());
        this->m_instances ++;

        return ID;
    }

    void Instanced_mesh::on_render(std::shared_ptr<const Material> material) {
        for (auto& lod : this->m_lod) {
           lod.on_render(this->m_instances, material);
        }
    }

    void Instanced_mesh::on_update() {
        //this->m_frustum_pipeline->execute(this->m_instances);
    }

    void Instanced_mesh::delete_instance(uint32_t ID) {
        auto instance_data_size = this->m_instance_buffer_data->get_layout().get_stride();
        if (ID > this->m_instance_IDs.size()) {
            //CORE_LOG_ERROR("Assets::Instanced_mesh:: Tried to delete non existing ID!");
        }
        this->m_instance_IDs[ID] = this->m_last_deleted;
        this->m_last_deleted = ID;
        this->m_instances--;
        this->m_ID_pointers.remove(ID);
        if (this->m_instances) {
            this->m_instance_buffer_data->copy_sub_data(this->m_instances*instance_data_size,ID*instance_data_size,instance_data_size);
        }
    }

    void Instanced_mesh::update_instance(void *data,uint32_t offset,  uint32_t instance) {
        this->m_instance_buffer_data->set_sub_data(data, this->m_ID_pointers.index(instance) * this->m_instance_buffer_data->get_layout().get_stride()+offset, this->m_instance_buffer_data->get_layout().get_stride()-offset);
        //float sphere[] = {position.x, position.y, position.z, radius};
        //this->m_instance_bounding_spheres->set_sub_data(sphere, instance * m_instance_bounding_spheres->get_layout().get_stride(), m_instance_bounding_spheres->get_layout().get_stride());
    }

    uint32_t Instanced_mesh::get_instance_stride() {
        return this->m_instance_buffer_data->get_layout().get_stride();
    }

    /*
    Static_mesh_component::Static_mesh_component(std::shared_ptr<Instanced_mesh> mesh) : p_mesh(mesh) {
        //this->add_component<Transform>();
        //auto transform = this->get_component<Transform>();
        auto matrix = glm::mat4x4 (1);
        this->m_mesh_ID = this->p_mesh->add_instance(reinterpret_cast<float *>(&matrix[0][0]), glm::vec3(), 1.0f);
    }

    Static_mesh_component::Static_mesh_component(std::shared_ptr<Instanced_mesh> mesh, glm::vec3 position) : p_mesh(mesh) {

        //this->add_component<Transform>(position);
        //auto transform = this->get_component<Transform>();

        auto matrix = glm::mat4x4 (1);
        matrix = glm::translate(matrix, position);
        this->m_mesh_ID = this->p_mesh->add_instance(&matrix[0][0], position, 1.0f);
    }

    Static_mesh_component::Static_mesh_component(std::shared_ptr<Instanced_mesh> mesh,  Transform& transform) {
        this->p_mesh = mesh;
        this->m_mesh_ID  = this->p_mesh->add_instance(reinterpret_cast<float *>(&transform.m_matrix[0][0]), transform.m_position, 1.0f);
    }
*/
//    void Static_mesh_component::on_update() {
//        Entity::on_update();
//        auto transform = this->get_component<Transform>();
//
//        transform.m_position += glm::vec3(0,(rand()%100)/100.0f,0);
//        if(transform.m_position.y > 10) {
//            transform.m_position.y = -10.0f;
//        }
//        glm::translate(transform.m_matrix,transform.m_position);
//        this->p_mesh->update_instance(reinterpret_cast<float *>(&this->get_component<Transform>().m_matrix), this->m_mesh_ID);
//    }
}


