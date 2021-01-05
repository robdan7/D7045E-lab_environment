#pragma once
#include <Renderer/Buffer.h>
#include <memory>
#include <initializer_list>
#include <Renderer/Frustum_compute_query.h>
#include <Renderer/Shader.h>
#include <Renderer/Vertex_array.h>
#include <Renderer/Render_encoder.h>
#include <Utils/Sparse_set.h>
#include "Mesh_import.h"
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
                void draw(uint32_t instances,std::shared_ptr<const Material> material = nullptr) {
                    if (material != nullptr) {
                        this->m_renderer->draw_instanced(material,this->m_vao,instances);
                    } else {
                        this->m_renderer->draw_instanced(this->m_material,this->m_vao,instances);
                    }
                }
                ~Mesh_partition() {
                    delete this->m_renderer;
                }
            };
        public:
            std::shared_ptr<Vertex_buffer> p_buffer;
            std::vector<Mesh_partition> mesh_partitions;
            Mesh_LOD(std::shared_ptr<Vertex_buffer> vertices) : p_buffer(vertices) {}
            void on_render(uint32_t instances,std::shared_ptr<const Material> material = nullptr) {
                for (auto& mesh : this->mesh_partitions) {
                    mesh.draw(instances,material);
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
        virtual void on_update();

        virtual void on_render(std::shared_ptr<const Material> material = nullptr);

        /**
         * Add another instance of this mesh. This is also a work in progress.
         * @param data
         */
        uint32_t add_instance(void* data);
        void update_instance(void* data,uint32_t offset, uint32_t ID);
        void delete_instance(uint32_t ID);

        uint32_t get_instance_stride();

    protected:
        Sparse_set<uint32_t> m_ID_pointers;
        std::vector<uint32_t> m_instance_IDs;
        uint32_t m_last_deleted;
        std::vector<Mesh_LOD> m_lod;    /// List of all LODs.
        std::shared_ptr<Vertex_buffer> m_instance_buffer_data;  /// uncompressed instance storage.
        //std::shared_ptr<Vertex_buffer> m_instance_bounding_spheres; /// Bounding spheres used for future frustum culling.
        uint32_t m_instances{};
    };

    /**
     * Test class for Game entities with mesh component.
     * This has been purposely removed due to the use of external libraries.
     */
     /*
    class Static_mesh_component {//: public ECS::Entity{
    public:
        Static_mesh_component(std::shared_ptr<Instanced_mesh> mesh);
        Static_mesh_component(std::shared_ptr<Instanced_mesh> mesh, glm::vec3 position);
        Static_mesh_component(std::shared_ptr<Instanced_mesh> mesh, Transform& transform);

//        void on_update() override;
    private:
        uint32_t m_mesh_ID;
        std::shared_ptr<Instanced_mesh> p_mesh;
    };
      */
}

