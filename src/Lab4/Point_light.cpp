#include "Point_light.h"
#include <Assets/Instanced_mesh.h>
#include "Geometry.h"
namespace Lab4 {

    /// ---------- Point light ----------
    GL_Point_light::GL_Point_light(const std::initializer_list<Engine::Buffer_element>& instance_layout, const uint32_t& reserved_instances):
        Engine::Instanced_mesh(instance_layout,reserved_instances){}

    uint32_t GL_Point_light::add_instance(glm::vec3  color, glm::vec3 position, float linear, float quadratic,float continuous) {
        float data[] = {
                color.r,color.g,color.b,
                position.x,position.y,position.z,1,
                continuous,
                linear,
                quadratic
        };
        return Instanced_mesh::add_instance(data);
    }

    std::shared_ptr<GL_Point_light> GL_Point_light::Create(uint32_t reserved_instances) {
        auto light_vertex = Engine::Shader::create_shader_stage(GL_VERTEX_SHADER, R"(
        #version 450 core
        layout(location = 0) in vec3 position;
        // ---- instance attributes ----
        layout(location = 2) in vec3 color;
        layout(location = 3) in float radius;
        layout(location = 4) in float dropoff;
        layout(location = 5) in mat4 transform;

        out Vertex_output {
            vec2 tex_coords;
            vec3 light_color;
            vec4 world_position;
            float radius;
            float dropoff;
        } vs;

        layout(std140) uniform Uniform_block {
            mat4 view_matrix;
            vec3 viewport;
            ivec2 view_size;
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            vec4 sun;
        } my_block;



        void main() {
            vs.world_position = (transform*vec4(0,0,0,1));
            vec4 screen_position = my_block.view_matrix*vs.world_position;

            gl_Position = vec4(position*radius*sqrt(2)/screen_position.w*vec3(1,float(my_block.view_size.x)/my_block.view_size.y,1)+vec3(screen_position.xy/screen_position.w,0),1);

            vs.tex_coords = (gl_Position.xy/gl_Position.w*0.5+0.5);
            vs.light_color = color;
            vs.radius = radius;
            vs.dropoff = dropoff;
        }
    )");

        auto light_fragment = Engine::Shader::create_shader_stage(GL_FRAGMENT_SHADER, R"(
        #version 450 core
        in Vertex_output {
            vec2 tex_coords;
            vec3 light_color;
            vec4 world_position;
            float radius;
            float dropoff;
        } vs;
        layout(location = 0) out vec4 frag_color;


        layout(location = 0) uniform sampler2D diffuse_texture;
        layout(location = 1) uniform sampler2D specular_texture;
        layout(location = 2) uniform sampler2D shininess_texture;
        layout(location = 3) uniform sampler2D normal_texture;
        layout(location = 4) uniform sampler2D frag_coord_texture;

        layout(std140) uniform Uniform_block {
            mat4 view_matrix;
            //mat4 projection_matrix;
            vec3 viewport;
            ivec2 view_size;
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            vec4 sun;
        } my_block;


        vec3 phong(vec3 fragPos, vec3 normal,vec3 viewport, vec4 light, vec3 light_color, float specular_factor, float shininess) {
            vec3 light_normal = normalize(light.xyz-fragPos*light.w);
            vec3 viewDir = normalize(viewport - fragPos);
            vec3 reflect = reflect(-light_normal,normal);
            float spec_power = pow(max(dot(reflect,viewDir),0.0),specular_factor)*shininess;


            float d = dot(normal,light_normal);
            if (d > 0) {
                vec3 specular_light = light_color*spec_power;
                vec3 diffuse_light = light_color*d;
                return (diffuse_light+ specular_light);
            } else {
                return vec3(0,0,0);
            }
        }

        void main() {
            vec2 screen_tex_coords = gl_FragCoord.xy/my_block.view_size;

            vec4 diffuse = texture(diffuse_texture, screen_tex_coords);
            vec3 normal = texture(normal_texture, screen_tex_coords).xyz;
            float specular_power = texture(specular_texture, screen_tex_coords).r;
            float shininess = texture(shininess_texture, screen_tex_coords).r;
            vec3 frag_position = texture(frag_coord_texture, screen_tex_coords).xyz;

            vec3 calculated_light = phong(frag_position, normal, my_block.viewport,vs.world_position,vs.light_color,specular_power,shininess);

            float distance = length(vs.world_position.xyz-frag_position.xyz);
            float attenuation = pow(1-min(distance,vs.radius)/vs.radius,vs.dropoff);
            frag_color = diffuse*vec4(calculated_light,1)*attenuation;

            //frag_color = vec4(0,0,0,1); // Debug color
        }
    )");

        auto shader = Engine::Shader::create_shader({light_vertex, light_fragment});
        shader->compile();

        Engine::Import_mesh light_mesh;
        light_mesh.vertices = {
                -1,-1,0, 0,0,1,
                1,-1,0, 0,0,1,
                -1,1,0, 0,0,1,
                1,-1,0, 0,0,1,
                1,1,0, 0,0,1,
                -1,1,0, 0,0,1
        };

        light_mesh.materials.emplace_back("", 0);
        light_mesh.materials[0].vertices = light_mesh.vertices.size() / 6;
        light_mesh.draw_type = Engine::Draw_type::STREAM;
        light_mesh.m_vertex_buffer = Engine::Vertex_buffer::Create(
                &light_mesh.vertices[0], light_mesh.vertices.size() * sizeof(light_mesh.vertices[0]),
                Engine::Raw_buffer::Access_frequency::STATIC,
                Engine::Raw_buffer::Access_type::DRAW);
        auto my_material = std::make_shared<Engine::Material>(shader);

        auto light = std::shared_ptr<GL_Point_light>(new GL_Point_light({
                                            {Engine::Shader_data::Float3, "color"},
                                            {Engine::Shader_data::Float, "radius"},
                                            {Engine::Shader_data::Float, "dropoff"},
                                            {Engine::Shader_data::Mat4, "transform"}
                                            }, reserved_instances));
        light->add_LOD(light_mesh, my_material);
        light->m_lod[0].mesh_partitions[0].m_material->bind();
        for (int i= 0; i < 6; i++) {
            glUniform1i(i,i);   /// Quick fix for sampler bindings. Not entirely sure if this belongs here :/
        }
        light->m_lod[0].mesh_partitions[0].m_material->disable();
        return light;
    }

}