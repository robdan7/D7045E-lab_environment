#include "Cube_map.h"
namespace Lab4 {

    Cube_map::Cube_map(int width, int height, std::shared_ptr<Engine::Window> window) : width(width), height(height), p_window(window){
        Engine::GL_Framebuffer::set_depth_texture(Engine::Cube_texture2D::Create_empty(width,height,GL_DEPTH24_STENCIL8,GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8));

        glm::mat4 perspective = glm::perspective(glm::radians(90.0f),1.0f,0.0f,100.0f);
        glm::vec3 light_coords = glm::vec3(0,0,0);

        this->m_camera_matrices.push_back(perspective * glm::lookAt(light_coords, light_coords + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
        this->m_camera_matrices.push_back(perspective * glm::lookAt(light_coords, light_coords + glm::vec3( -1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
        this->m_camera_matrices.push_back(perspective * glm::lookAt(light_coords, light_coords + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0,0.0, 1.0)));
        this->m_camera_matrices.push_back(perspective * glm::lookAt(light_coords, light_coords + glm::vec3( 0.0, -1.0, 0.0), glm::vec3(0.0,0.0, 1.0)));
        this->m_camera_matrices.push_back(perspective * glm::lookAt(light_coords, light_coords + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
        this->m_camera_matrices.push_back(perspective * glm::lookAt(light_coords, light_coords + glm::vec3( 0.0, 0.0, -1.0), glm::vec3(0.0,-1.0, 0.0)));


        auto cube_vert = Engine::Shader::create_shader_stage(GL_VERTEX_SHADER, R"(
        #version 450 core
        layout(location = 0) in vec3 position;
        layout(location = 1) in vec3 normal;
        layout(location = 2) in vec4 instance_color;
        layout(location = 3) in float specular_power;
        layout(location = 4) in mat4 transform;

        void main() {
            gl_Position = transform * vec4(position, 1.0);
        }
    )");

        auto cube_geometry = Engine::Shader::create_shader_stage(GL_GEOMETRY_SHADER, R"(
        #version 450 core
        layout (triangles) in;
        layout (triangle_strip, max_vertices=18) out;

        uniform mat4 shadowMatrices[6];

        out vec4 FragPos; // FragPos from GS (output per emitvertex))

        void main() {
            for(int face = 0; face < 6; ++face) {
                gl_Layer = face; // built-in variable that specifies to which face we render.
                for(int i = 0; i < 3; ++i) { // for each triangle vertex
                    FragPos = gl_in[i].gl_Position;
                    gl_Position = shadowMatrices[face] * FragPos;
                    EmitVertex();
                }
                EndPrimitive();
            }
        }
    )");

        auto cube_frag = Engine::Shader::create_shader_stage(GL_FRAGMENT_SHADER, R"(
        #version 450 core
        in vec4 FragPos;

        //uniform vec3 lightPos;
        //uniform float far_plane;

        void main() {
            float lightDistance = length(FragPos.xyz);

            lightDistance = lightDistance / 100.0;

            gl_FragDepth = lightDistance;
        }
    )");

        auto shader = Engine::Shader::create_shader({cube_vert, cube_geometry,cube_frag});
        shader->compile();
        shader->bind();
        glUniformMatrix4fv(glGetUniformLocation(shader->get_ID(),"shadowMatrices"),6,GL_FALSE,(float*)&this->m_camera_matrices[0][0]);
        this->m_material = std::make_shared<Engine::Material>(shader);
    }

    void Cube_map::bind_write() {
        GL_Framebuffer::bind_write();
        glViewport(0,0,this->width,this->height);
        glCullFace(GL_FRONT);
    }

    void Cube_map::bind_read() {
        GL_Framebuffer::bind_read();
        glViewport(0,0,this->width,this->height);
    }

    void Cube_map::unbind() {
        GL_Framebuffer::unbind();
        glViewport(0,0,this->p_window->get_width(), this->p_window->get_height());
        glCullFace(GL_BACK);
    }

    int Cube_map::get_width() {
        return this->width;
    }

    int Cube_map::get_height() {
        return this->height;
    }

    const std::shared_ptr<const Engine::Texture2D> Cube_map::get_gexture() {
        return this->p_textures.at(0);
    }

    std::shared_ptr<const Engine::Material> Cube_map::get_material() {
        return this->m_material;
    }
}