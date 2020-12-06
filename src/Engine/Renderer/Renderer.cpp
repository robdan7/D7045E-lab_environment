#include "Renderer.h"
#include <Platform/OpenGL/GL_renderer_API.h>
#include "Vertex_array.h"
#include "Shader_constants.h"
#include "Camera.h"
#include "Shader.h"
#include "Material.h"

namespace Engine {
    namespace Render {
        Renderer_API *Render_command::s_render_API = new GL_renderer_API();


        void Renderer::end_scene() {

        }

//        void Renderer::submit(const std::shared_ptr<Vertex_array> &vertex_array) {
//            vertex_array->bind();
//            Render_command::draw_indexed(vertex_array);
//            vertex_array->unbind();
//        }

//        void Renderer::submit_instanced(const std::shared_ptr<Engine::Shader> &shader,
//                                        const std::shared_ptr<Engine::Vertex_array> &vertex_array, int instances) {
//            shader->bind();
//            vertex_array->bind();
//            Render_command::draw_instanced(vertex_array, instances);
//            vertex_array->unbind();
//            shader->unbind();
//        }


        void Renderer::submit_streamed(const std::shared_ptr<Engine::Material>& material,std::shared_ptr<Engine::Vertex_array> vertex_array, Polygon_type polygon_type,
                                       uint32_t start_vertex, uint32_t vertex_count, uint32_t instances) {
            material->apply_material();
            vertex_array->bind();
            Render_command::draw_stream(vertex_array,polygon_type,start_vertex,vertex_count,instances);
            vertex_array->unbind();
            material->disable();
        }

        void Renderer::submit_indexed(const std::shared_ptr<Engine::Material> &material,
                                      std::shared_ptr<Engine::Vertex_array> vertex_array, Polygon_type polygon_type,
                                      int instances) {
            material->apply_material();
            vertex_array->bind();
            Render_command::draw_indexed(vertex_array,polygon_type,instances);
            vertex_array->unbind();
            material->disable();

        }

//        void Renderer::submit_arrays(const std::shared_ptr<Engine::Shader> &shader,
//                                const std::shared_ptr<Engine::Vertex_array> &vertex_array,
//                                int vertices, int instances) {
//            shader->bind();
//            vertex_array->bind();
//            Render_command::draw_streams_instanced(vertex_array, vertices, instances);
//            vertex_array->unbind();
//            shader->unbind();
//        }
    }

}
