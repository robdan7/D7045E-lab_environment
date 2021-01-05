#pragma once
#include <memory>
namespace Engine {
    class Material;
    class Vertex_array;
    class Shader;
    namespace Render {
        enum class Polygon_type {
            DOT, LINE, TRIANGLE,QUAD
        };
        /**
         * This is a simple interface for doing instanced or streamed rendering with the same parameters.
         *
         * WORK IN PROGRESS
         */
        class Render_encoder {
        public:
            inline Render_encoder(Polygon_type polygon_type) : polygons(polygon_type) {}
            virtual void draw_instanced(const std::shared_ptr<const Engine::Material>& material,const std::shared_ptr<Engine::Vertex_array>& vertex_array, uint32_t instances) const = 0;

        protected:
            Polygon_type polygons;
        };
        class Index_renderer : public Render_encoder {
        public:
            inline Index_renderer(Polygon_type polygon_type) : Render_encoder(polygon_type) {}
            void draw_instanced(const std::shared_ptr<const Engine::Material>& material,const std::shared_ptr<Engine::Vertex_array> &vertex_array, uint32_t instances) const override;
        private:

        };
        class Stream_renderer : public Render_encoder {
        public:
            Stream_renderer(Polygon_type polygon_type, uint32_t start_vertex, uint32_t vertex_count);
            void draw_instanced(const std::shared_ptr<const Engine::Material>& material,const std::shared_ptr<Engine::Vertex_array> &vertex_array, uint32_t instances) const override;
        private:
            uint32_t start_vertex = 0;
            uint32_t vertex_count = 0;
        };
    }
}
