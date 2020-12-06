#pragma once
#include <Renderer/Buffer.h>
#include <memory>
#include <initializer_list>
#include <Renderer/Frustum_compute_query.h>
#include <Renderer/Shader.h>
#include <Renderer/Vertex_array.h>
#include <Renderer/Render_encoder.h>
#include <Utils/Sparse_set.h>
#include "FBX_mesh.h"
#include "Renderer/Material.h"
//#include <ECS/Entity.h>
namespace Engine {

    /**
     * Demo class for mesh instancing. This is a work in progress.
     */
    class Instanced_mesh {
    private:

        struct Mesh_LOD {
        private:
            /**
             * The mesh partition contains a part of a mesh with a single material.
             * This allows for multi-material LODs, but the code for enabling it has not been
             * written yet. TODO write code
             */
            struct Mesh_partition {

                std::shared_ptr<Vertex_array> m_vao;
                std::shared_ptr<Material> m_material;
                Render::Render_encoder* m_renderer;
                Mesh_partition(std::shared_ptr<Vertex_array> array, std::shared_ptr<Material>  material) : m_vao(array) , m_material(material){
                    this->m_renderer = new Render::Index_renderer(Render::Polygon_type::TRIANGLE);
                }
                Mesh_partition(std::shared_ptr<Vertex_array> array, std::shared_ptr<Material> material, uint32_t start_vertex, uint32_t vertex_count) : m_vao(array) , m_material(material){
                    this->m_renderer = new Render::Stream_renderer(Render::Polygon_type::TRIANGLE,start_vertex,vertex_count);
                }
                void draw(uint32_t instances) {
                    this->m_renderer->draw_instanced(this->m_material,this->m_vao,instances);
                }
                ~Mesh_partition() {
                    delete this->m_renderer;
                }
            };
        public:
            std::shared_ptr<Vertex_buffer> p_buffer;
            std::vector<Mesh_partition> mesh_partitions;
            Mesh_LOD(std::shared_ptr<Vertex_buffer> vertices) : p_buffer(vertices) {}
            void on_render(uint32_t instances) {
                for (auto& material : this->mesh_partitions) {
                    material.draw(instances);
                }
            }
        };
    public:
        Instanced_mesh(const std::initializer_list<Buffer_element>& instance_layout, const uint32_t& reserved_instances);

        /**
         * Add another level of detail. Not finished yet.
         *
         * @param vertex_data
         * @param index_buffer
         * @param shader
         */
//        void add_LOD(std::shared_ptr<Vertex_buffer> vertex_data, std::shared_ptr<Index_buffer> index_buffer);

        void add_LOD(Import_mesh& mesh, const std::shared_ptr<Material>& material);

        /**
         * This should be part of an entity component. The purpose is to update all LODs.
         */
        void on_update();

        void on_render();

        /**
         * TODO remove in future update. This is just a debug for
         * how many instances that are rendered.
         * @return
         */
        int debug_info();

        /**
         * Add another instance of this mesh. This is also a work in progress.
         * @param data
         * @param position
         * @param radius
         */
        uint32_t add_instance(float* data, glm::vec3 position, float radius);
        void update_instance(float* data, uint32_t ID);
        void delete_instance(uint32_t ID);
    public:
        std::shared_ptr<Frustum_compute_query> m_frustum_pipeline;
    public:
        Sparse_set<uint32_t> m_ID_pointers;
        std::vector<uint32_t> m_instance_IDs;
        uint32_t m_last_deleted;
        std::vector<Mesh_LOD> m_lod;    /// List of all LODs.
        std::shared_ptr<Vertex_buffer> m_instance_buffer_data;  /// uncompressed instance storage.
        std::shared_ptr<Vertex_buffer> m_instance_bounding_spheres; /// Bounding spheres used for future frustum culling.
        uint32_t m_instances{};
    };

    struct Transform {
        glm::mat4x4 m_matrix;
        glm::vec3 m_position;
        Transform() : m_matrix(1.0f) {}
        Transform(glm::vec3 position);
    };

    class Static_mesh_component {
    public:
        Static_mesh_component(std::shared_ptr<Instanced_mesh> mesh);
        Static_mesh_component(std::shared_ptr<Instanced_mesh> mesh, glm::vec3 position);
        Static_mesh_component(std::shared_ptr<Instanced_mesh> mesh, Transform& transform);

//        void on_update() override;
    private:
        uint32_t m_mesh_ID;
        std::shared_ptr<Instanced_mesh> p_mesh;
    };
}

