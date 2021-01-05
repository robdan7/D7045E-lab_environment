#include "Point_light.h"
#include <Assets/Instanced_mesh.h>
#include "Geometry.h"
namespace Lab4 {

    /// ---------- Point light ----------
    GL_Point_light::GL_Point_light(const std::initializer_list<Engine::Buffer_element>& instance_layout, const uint32_t& reserved_instances):
        Engine::Instanced_mesh(instance_layout,reserved_instances){
        /*
        float light_data[] = {
                -1,-1,
                1,-1,
                -1,1,
                1,-1,
                1,1,
                -1,1
        };
        auto light_vbo = Engine::Vertex_buffer::Create(&light_data, sizeof(light_data), Engine::Raw_buffer::Access_frequency::STATIC, Engine::Raw_buffer::Access_type::DRAW);
        light_vbo->set_layout(Engine::Buffer_layout{{Engine::Shader_data::Float2, "position"}});
        this->m_vao = Engine::Vertex_array::Create();
        this->m_vao->add_vertex_buffer(light_vbo);
*/

    }
/*
    void GL_Point_light::on_render() {
        this->m_vao->bind();
        this->m_shader->bind();
        glDrawArrays(GL_TRIANGLES,0,6);
        this->m_vao->unbind();
        this->m_shader->unbind();
    }



    void GL_Point_light::on_update() {

    }
*/
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

        Engine::Import_mesh light_mesh;
        light_mesh.vertices = {
                -1,-1,0, 0,0,1,
                1,-1,0, 0,0,1,
                -1,1,0, 0,0,1,
                1,-1,0, 0,0,1,
                1,1,0, 0,0,1,
                -1,1,0, 0,0,1
        };
        //create_sphere(light_mesh.vertices,16);
        light_mesh.materials.emplace_back("", 0);
        light_mesh.materials[0].vertices = light_mesh.vertices.size() / 6;
        light_mesh.draw_type = Engine::Draw_type::STREAM;
        light_mesh.m_vertex_buffer = Engine::Vertex_buffer::Create(
                &light_mesh.vertices[0], light_mesh.vertices.size() * sizeof(light_mesh.vertices[0]),
                Engine::Raw_buffer::Access_frequency::STATIC,
                Engine::Raw_buffer::Access_type::DRAW);

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
            //mat4 projection_matrix;
            vec3 viewport;
            ivec2 view_size;
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            vec4 sun;
            mat4 shadow_camera;
        } my_block;



        void main() {
            vs.world_position = (transform*vec4(0,0,0,1));
            vec4 screen_position = my_block.view_matrix*vs.world_position;

            gl_Position = vec4(position*radius*sqrt(2)/screen_position.w*vec3(1,float(my_block.view_size.x)/my_block.view_size.y,1)+vec3(screen_position.xy/screen_position.w,0),1);
            /*
            float rad = radius / screen_position.w;
            vec2 aspect_ratio = vec2(1,float(my_block.view_size.x)/my_block.view_size.y);
            vec2 screen_xy = position.xy*rad*sqrt(2)*aspect_ratio;
            gl_Position = screen_position + vec4(screen_xy,-rad,0);
            gl_Position.w = -1;
*/
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
        layout(location = 2) uniform sampler2D normal_texture;
        layout(location = 3) uniform sampler2D frag_coord_texture;
        layout(location = 4) uniform samplerCube depth_texture;



        //uniform mat4 shadow_view_matrix;


        vec3 phong(vec3 color,vec3 fragPos, vec3 normal,vec3 viewport, vec4 light, vec3 ambient, vec3 diffuse, vec3 specular) {
                vec3 light_normal = normalize(light.xyz-fragPos*light.w);
                vec3 viewDir = normalize(viewport - fragPos);
                vec3 reflect = reflect(-light_normal,normal);
                float spec_power = pow(max(dot(reflect,viewDir),0.0),32);


                float d = dot(normal,light_normal);
                if (d > 0) {
                    vec3 specular_light = specular*spec_power;
                    vec3 diffuse_light = diffuse*d;
                    return color*(ambient+diffuse_light+ specular_light);
                } else {
                    return color*ambient;
                }


            }

        layout(std140) uniform Uniform_block {
            mat4 view_matrix;
            //mat4 projection_matrix;
            vec3 viewport;
            ivec2 view_size;
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            vec4 sun;
            mat4 shadow_camera;
        } my_block;

        void main() {
            vec2 screen_tex_coors = gl_FragCoord.xy/my_block.view_size;

            vec4 diffuse = texture(diffuse_texture, screen_tex_coors);
            vec3 normal = texture(normal_texture, screen_tex_coors).xyz;
            float specular_power = texture(specular_texture, screen_tex_coors).r;
            vec3 frag_position = texture(frag_coord_texture, screen_tex_coors).xyz;
            /*
            vec4 shadow_coords = my_block.view_matrix*vec4(frag_position,1);
            //float shadow_depth = texture(depth_texture, shadow_coords.xy*0.5+0.5).r;
            float shadow_depth = texture(depth_texture, frag_position).r*100;

            float depth = shadow_coords.z*0.5+0.5;
            depth = length(frag_position);

            float my_var = shadow_depth+0.05 > depth ? 1 : 0;
            */
            //vec3 color = phong(vs_light_color,frag_position.xyz,normal,my_block.viewport,vs_world_position,my_block.ambient,vs_light_color,specular_power*vs_light_color);


            float distance = length(vs.world_position.xyz-frag_position.xyz);
            vec3 light_normal = normalize(vs.world_position.xyz-frag_position.xyz);
            float d = dot(normal,light_normal);
            //d = 1;
            float attenuation = pow(1-min(distance,vs.radius)/vs.radius,vs.dropoff);
            frag_color = diffuse*vec4(vs.light_color,1)*d*attenuation;
            //frag_color = vec4(1,0,1,1); // Debug color


            vec4 view_coords = my_block.view_matrix*vec4(frag_position,1);
            //gl_FragDepth = (view_coords.z/view_coords.w*0.5+0.5);
        }
    )");

        auto shader = Engine::Shader::create_shader({light_vertex, light_fragment});
        shader->compile();

        auto my_material = std::make_shared<Engine::Material>(shader);
        //light_model->add_LOD(light_mesh, my_material);

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