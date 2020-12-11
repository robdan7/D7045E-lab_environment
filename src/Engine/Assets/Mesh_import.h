#pragma once
//#include <fbxdocument.h>
#include <vector>
#include <unordered_map>
#include <Renderer/Buffer.h>

namespace Engine {


    enum class Material_mapping_type {
        ALL_SAME, BY_POLYGON_VERTEX
    };

    struct Import_material {
        std::string mapping_type;
        std::vector<uint32_t> vertex_index;
        int material_index;
        int vertices = 0;
        uint32_t starting_vertex = 0;
        Import_material(std::string mapping_type, int material_index) : mapping_type(mapping_type),
                                                                        material_index(material_index) {}
    };

    enum class Normal_type {
        BY_POLYGON_VERTEX
    };
    struct Import_normals {
        std::string mapping_type = "None";
        std::vector<float> normals;
    };

    struct Import_indices {
        int polygon_size;
        std::vector<uint32_t> indices;
    };

    enum class Draw_type {
        INDEXED, STREAM
    };


    /**
     * This was originally intended to contain parsed information from
     * an FBX file.
     */
    struct Import_mesh {

        std::vector<float> vertices;
        Import_indices indices;
        std::vector<Import_material> materials;
        Draw_type draw_type;
        Import_normals vertex_normals;
        std::shared_ptr<Engine::Vertex_buffer> m_vertex_buffer;
    };
}