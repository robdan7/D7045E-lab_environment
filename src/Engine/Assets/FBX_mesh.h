#pragma once
//#include <fbxdocument.h>
#include <vector>
#include <unordered_map>
#include <Renderer/Buffer.h>

namespace Engine {


    enum class Material_mapping_type {
        ALL_SAME, BY_POLYGON_VERTEX
    };

    struct FBX_material {
        std::string mapping_type;
        std::vector<uint32_t> vertex_index;
        int material_index;
        int vertices = 0;
        uint32_t starting_vertex = 0;

        FBX_material(std::string mapping_type, int material_index) : mapping_type(mapping_type),
                                                                     material_index(material_index) {}
    };

    enum class Normal_type {
        BY_POLYGON_VERTEX
    };
    struct FBX_normals {
        std::string mapping_type = "None";
        std::vector<float> normals;
    };

    struct FBX_indices {
        int polygon_size;
        std::vector<uint32_t> indices;
    };

    enum class Draw_type {
        INDEXED, STREAM
    };

    struct Import_mesh {
        bool has_normals() const {
            return !this->vertex_normals.normals.empty();
        }

        std::vector<float> vertices;
        FBX_indices indices;
        std::vector<FBX_material> materials;
        Draw_type draw_type;
        FBX_normals vertex_normals;
        std::shared_ptr<Engine::Vertex_buffer> m_vertex_buffer;
    };
}