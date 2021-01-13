#include "Phong_material.h"
#include <Platform/OpenGL/OpenGL.h>
namespace Lab4 {

    Phong_material::Phong_material(std::shared_ptr<Engine::Shader> shader) : Material(shader) {

    }

    std::shared_ptr<Phong_material> Phong_material::Create() {
        /// -------- Shaders --------
        auto vertex = Engine::Shader::create_shader_stage(
                GL_VERTEX_SHADER,
                R"(
            #version 450 core

            layout(location = 0) in vec3 position;
            layout(location = 1) in vec3 normal;
            layout(location = 2) in vec4 instance_color;
            layout(location = 3) in float specular_power;
            layout(location = 4) in float shininess;
            layout(location = 5) in mat4 transform;

            out VS_output {
                layout(location = 0) out vec3 normal;
                layout(location = 1) out vec3 position;
                layout(location = 2) out vec4 color;
                layout(location = 3) out float spec_power;
                layout(location = 4) out float shininess;
            } vs_out;

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

            void main() {
                gl_Position = my_block.view_matrix*transform*vec4(position,1);
                vs_out.position = (transform*vec4(position,1)).xyz;
                vs_out.normal = normalize((transform*vec4(normal,0)).xyz);


                vs_out.color = instance_color;
                vs_out.spec_power = specular_power;
                vs_out.shininess = shininess;
            }

            )");
        auto fragment = Engine::Shader::create_shader_stage(
                GL_FRAGMENT_SHADER,
                R"(
            #version 450 core
            in VS_output {
                layout(location = 0) vec3 normal;
                layout(location = 1) vec3 position;
                layout(location = 2) vec4 color;
                layout(location = 3) float spec_power;
                layout(location = 4) float shininess;
            } vs_out;

            layout(location = 0) out vec4 diffuse_color;
            layout(location = 1) out float specular_power;
            layout(location = 2) out float shininess;
            layout(location = 3) out vec3 fragment_normal;
            layout(location = 4) out vec3 world_position;


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
                diffuse_color = vs_out.color;
                specular_power = vs_out.spec_power;
                fragment_normal = normalize(vs_out.normal);
                world_position = vs_out.position;
                shininess = vs_out.shininess;
            }
            )");

        auto shader_program = Engine::Shader::create_shader({vertex, fragment});
        shader_program->compile();

        /// The uniform bindings must be done manually.
        GLuint index= glGetUniformBlockIndex(shader_program->get_ID(), "Uniform_block");
        glUniformBlockBinding(shader_program->get_ID(), index, 0);

        auto mat = new Phong_material(shader_program);
        return std::shared_ptr<Phong_material>(mat);
    }
}
