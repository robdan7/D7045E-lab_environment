#define API_OPENGL
#include <Engine.h>
#include <functional>
#include <ctime>
#include "Triangle.h"
#include "Search_tree.h"
#define PI (atan(1)*4)

BEGIN_SHADER_CONST(Global_uniforms)
    SET_SHADER_CONST(Engine::Matrix4_data, view_matrix)
    SET_SHADER_CONST(Engine::Matrix4_data, transform_matrix)
    SET_SHADER_CONST(Engine::Float4_data, color);
END_SHADER_CONST(Global_uniforms)

#include "Vertex.h"
#include "Algorithms.h"
#include <iostream>

void randomize_points(std::vector<Lab2::Vertex>& list, int size) {
    std::srand(std::time(nullptr));
        for (int i = 0 ; i < size; ++i) {
            auto x = (rand() % (180)-90)/100.0f;
            auto y = (rand() % (180)-90)/100.0f;
            list.emplace_back(x,y);
        }

}

int main(int argc, char** argv) {
    auto window = std::shared_ptr<Engine::Window>(Engine::Window::create_window());
    Global_uniforms uniforms;

    std::vector<Lab2::Vertex> list;
    randomize_points(list,10);

    Lab2::sort(list);
    std::vector<float> colors;
    for (const auto& vert : list) {
        colors.push_back(vert.x);
        colors.push_back(vert.y);
    }

    auto hull_indices = Lab2::calc_convex_hull(list);

    std::vector<Lab2::Triangle> triangles;
    Lab2::build(hull_indices,list,triangles);


    Lab2::Vertex& a= list[0];
    //std::find(list.begin(),list.end(), a);

    std::vector<uint32_t> triangle_indices;
    float i = 0;
    for (const auto& tri : triangles) {
        triangle_indices.push_back(std::find(list.begin(),list.end(), tri.a)-list.begin());
        triangle_indices.push_back(std::find(list.begin(),list.end(), tri.b)-list.begin());
        triangle_indices.push_back(std::find(list.begin(),list.end(), tri.c)-list.begin());
/*
        colors.push_back(0);
        colors.push_back(0);
        colors.push_back(0);
        colors.push_back(0);
*/
    }

    /// -------- Shaders --------
    auto vertex = Engine::Shader::create_shader_stage(
            GL_VERTEX_SHADER,
            R"(
            #version 450 core
            in vec4 position;

            uniform Uniform_block {
                mat4 view_matrix;
                mat4 transform_matrix;
                vec4 color;
            } my_block;

            void main() {
                gl_Position = position;
            }

            )");
    auto fragment = Engine::Shader::create_shader_stage(
            GL_FRAGMENT_SHADER,
            R"(
            #version 450 core
            out vec4 color;
            uniform Uniform_block {
                mat4 view_matrix;
                mat4 transform_matrix;
                vec4 color;
            } my_block;
            void main() {
                color = my_block.color;
            }
            )");

    auto shader_program = Engine::Shader::create_shader({vertex, fragment});
    shader_program->compile();

    /// The uniform bindings must be done manually.
    GLuint index= glGetUniformBlockIndex(shader_program->get_ID(), "Uniform_block");
    glUniformBlockBinding(shader_program->get_ID(), index, 0);

    /// -------- Buffers --------

    auto vertex_buffer = Engine::Vertex_buffer::Create(&list[0], list.size()*2*sizeof(float),
                                                       Engine::Raw_buffer::Access_frequency::STATIC,
                                                       Engine::Raw_buffer::Access_type::DRAW);

    //auto color_buffer = Engine::Vertex_buffer::Create(&colors[0], colors.size()*sizeof(float), Engine::Raw_buffer::Access_frequency::STATIC,Engine::Raw_buffer::Access_type::DRAW);
    //Engine::Buffer_layout color_layout = {{Engine::Shader_data::Float2, "test"}};


    /// Set the layout of the vertex buffer. This tells the VAO what the vertex attributes should be.
    Engine::Buffer_layout layout = {{Engine::Shader_data::Float2, "position"}};
    vertex_buffer->set_layout(layout);

    auto index_buffer = Engine::Index_buffer::Create(&triangle_indices[0], triangle_indices.size()*sizeof(uint32_t),Engine::Raw_buffer::Access_frequency::DYNAMIC);

    /// -------- VAOs --------
    auto triangle_fan = Engine::Vertex_array::Create();
    triangle_fan->add_vertex_buffer(vertex_buffer);
    //triangle_fan->add_vertex_buffer(color_buffer);
    triangle_fan->set_index_buffer(index_buffer);


    auto line_vertex_array = Engine::Vertex_array::Create();
    line_vertex_array->add_vertex_buffer(vertex_buffer);
    line_vertex_array->set_index_buffer(index_buffer);

    /// -------- Camera --------
    float aspect_ratio = ((float)window->get_width()/window->get_height());
    auto camera = Engine::Orthographic_camera(-aspect_ratio,aspect_ratio,1,-1,0,1);
    camera.on_update(); /// This is the only update needed.

    /// Initialize camera uniform matrix.
    uniforms.view_matrix.m_data = camera.get_view_projection_matrix();
    uniforms.update_view_matrix();

    /// Misc GL and engine commands.
    glLineWidth(3); /// TODO move to engine.
    glPointSize(10);
    glEnable(GL_POINT_SMOOTH);
    Engine::Render::Render_command::set_clear_color({0.2,0.8,0.3,1});

    auto black = glm::vec4(0,0,0,1);
    auto red = glm::vec4(1,0,0,1);
    auto yellow = glm::vec4 (0.9,0.7,0.2,1);
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    while (!window->should_close()) {

        /// Circle around window center
        uniforms.transform_matrix.m_data = glm::rotate(glm::mat4(1.0f), 0.0f, glm::vec3(0, 0, 1));
        uniforms.transform_matrix.m_data = glm::translate(uniforms.transform_matrix.m_data, glm::vec3(0.75f,0,0));
        /// Rotate around object origin.
        uniforms.transform_matrix.m_data = glm::rotate(uniforms.transform_matrix.m_data, 0.0f, glm::vec3(0, 0, 1));

        float scale = sin(0.0f) * 0.5f + 1;
        uniforms.transform_matrix.m_data = glm::scale(uniforms.transform_matrix.m_data, glm::vec3(scale, scale, scale));


        /// Update transform uniform.
        uniforms.update_transform_matrix();

        Engine::Render::Render_command::clear();

        shader_program->bind();
        line_vertex_array->bind();
        /*
        uniforms.color.m_data = yellow;
        uniforms.update_color();
        glDrawElements(GL_TRIANGLE_FAN, index_buffer->get_count()/sizeof(uint32_t), GL_UNSIGNED_INT, nullptr);
        */

        uniforms.color.m_data = red;
        uniforms.update_color();
        glDrawArrays(GL_POINTS, 0, list.size());

/*
        uniforms.color.m_data = black;
        uniforms.update_color();
        //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        glDrawElements(GL_LINE_STRIP, index_buffer->get_count()/sizeof(uint32_t), GL_UNSIGNED_INT, nullptr);
        //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        //glDrawElements(GL_POINTS, index_buffer->get_count()/sizeof(uint32_t), GL_UNSIGNED_INT, nullptr);
        line_vertex_array->unbind();
*/

        triangle_fan->bind();
        uniforms.color.m_data = black;
        uniforms.update_color();
        glDrawElements(GL_TRIANGLES, index_buffer->get_count()/sizeof(uint32_t), GL_UNSIGNED_INT, nullptr);

        line_vertex_array->bind();

        uniforms.color.m_data = red;
        uniforms.update_color();
        glDrawArrays(GL_POINTS, 0, list.size());

        triangle_fan->unbind();

        shader_program->unbind();


        window->on_update();
    }
    return 0;
}
