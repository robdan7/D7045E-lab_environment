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

double pi = 2 * acos(0.0);


void randomize_points(std::vector<Lab2::Vertex>& list, int size) {
/*
    list.emplace_back(-0.7f,0.0f);
    list.emplace_back(-0.6f,-0.4f);
    list.emplace_back(0.2f,0.6f);
    list.emplace_back(0.5f,-0.15f);
    list.emplace_back(0.6f,0.05f);
    list.emplace_back(0.7f,-0.76f);
*/



    std::srand(std::time(nullptr));
    for (int i = 0 ; i < size; ++i) {
        auto x = (rand() % (2000)-1000)/1000.0f;
        auto y = (rand() % (2000)-1000)/1000.0f;
        list.emplace_back(x,y);
    }



}


int main(int argc, char** argv) {
    auto window = std::shared_ptr<Engine::Window>(Engine::Window::create_window());
    Global_uniforms uniforms;



    std::vector<Lab2::Vertex> list;
    randomize_points(list,20);

    float k =3;
    Lab2::Vertex mid_point;

    {
        float total_x = 0, total_y = 0;
        for (const auto& v : list) {
            total_x += v.x;
            total_y += v.y;
        }
        mid_point.x = total_x/list.size();
        mid_point.y = total_y/list.size();
    }

    Lab2::sort(list);
    std::vector<float> colors;
    for (const auto& vert : list) {
        colors.push_back(vert.x);
        colors.push_back(vert.y);
    }
    auto hull_indices = Lab2::calc_convex_hull(list);
    std::vector<std::shared_ptr<Lab2::Triangle>> triangles;
    auto tree = Lab2::build(hull_indices,list,triangles);





/*
    list.emplace_back(0.25f,0.25f);

    list.emplace_back(0.25,0);
    list.emplace_back(0.4f,0);
    list.emplace_back(0.3f,0.1f);
    list.emplace_back(0.5f,0.25f);
*/
    Lab2::split(tree,hull_indices,list,triangles);


    std::vector<Lab2::Color> triangle_colors(triangles.size());
    std::vector<bool> visited_triangles(triangles.size());
    Lab2::Color A{81/255.0f,170/255.0f,227/255.0f};
    Lab2::Color B{48/255.0f,8/255.0f,70/255.0f};
    Lab2::Color C{1,227/255.0f,0/255.0f};
    Lab2::Color D{168/255.0f,30/255.0f,27/255.0f};

    /*
    Lab2::Color A{1,0,0};
    Lab2::Color B{0,1,0};
    Lab2::Color C{0,0,1};
    Lab2::Color D{1,1,1};
     */
    Lab2::color(triangles[0],visited_triangles,triangle_colors,&A,&B,&C,&D);

    Lab2::Vertex& a= list[0];
    //std::find(list.begin(),list.end(), a);

    std::vector<uint32_t> triangle_indices;


    std::vector<float> triangle_array;


    float i = 0;
    for (const auto& tri : triangles) {
        auto len1 = (*tri->a-mid_point).len();
        auto len2 = (*tri->b-mid_point).len();
        auto len3 = (*tri->c-mid_point).len();
        float d = len3;
        if (len1 > len2 && len1 > len3) {
            d = len1;
        } else if (len2 > len1 && len2 > len3) {
            d = len2;
        }

        triangle_array.push_back(tri->a->x);
        triangle_array.push_back(tri->a->y);

        triangle_array.push_back(triangle_colors[tri->triangle_ID].r);
        triangle_array.push_back(triangle_colors[tri->triangle_ID].g);
        triangle_array.push_back(triangle_colors[tri->triangle_ID].b);
        /*
        triangle_array.push_back((float)(std::sin(k*pi*(d-tri->a->x)/d)+1)/2.0f);
        triangle_array.push_back((std::sin(k*pi*(d-tri->a->x)/d+2*pi/3)+1)/2.0f);
        triangle_array.push_back((std::sin(k*pi*(d-tri->a->x)/d+4*pi/3)+1)/2.0f);
        */
        triangle_array.push_back(tri->b->x);
        triangle_array.push_back(tri->b->y);
        triangle_array.push_back(triangle_colors[tri->triangle_ID].r);
        triangle_array.push_back(triangle_colors[tri->triangle_ID].g);
        triangle_array.push_back(triangle_colors[tri->triangle_ID].b);
        /*
        triangle_array.push_back((std::sin(k*pi*(d-tri->b->x)/d)+1)/2.0f);
        triangle_array.push_back((std::sin(k*pi*(d-tri->b->x)/d+2*pi/3)+1)/2.0f);
        triangle_array.push_back((std::sin(k*pi*(d-tri->b->x)/d+4*pi/3)+1)/2.0f);
        */
        triangle_array.push_back(tri->c->x);
        triangle_array.push_back(tri->c->y);
        triangle_array.push_back(triangle_colors[tri->triangle_ID].r);
        triangle_array.push_back(triangle_colors[tri->triangle_ID].g);
        triangle_array.push_back(triangle_colors[tri->triangle_ID].b);
        /*
        triangle_array.push_back((std::sin(k*pi*(d-tri->c->x)/d)+1)/2.0f);
        triangle_array.push_back((std::sin(k*pi*(d-tri->c->x)/d+2*pi/3)+1)/2.0f);
        triangle_array.push_back((std::sin(k*pi*(d-tri->c->x)/d+4*pi/3)+1)/2.0f);
        */

        triangle_indices.push_back(std::find(list.begin(),list.end(), tri->a)-list.begin());
        triangle_indices.push_back(std::find(list.begin(),list.end(), tri->b)-list.begin());
        triangle_indices.push_back(std::find(list.begin(),list.end(), tri->c)-list.begin());
        i++;

    }

    /// -------- Shaders --------
    auto vertex = Engine::Shader::create_shader_stage(
            GL_VERTEX_SHADER,
            R"(
            #version 450 core
            in vec4 position;
            in vec3 color;
            out vec3 vs_color;

            uniform Uniform_block {
                mat4 view_matrix;
                mat4 transform_matrix;
                vec4 color;
            } my_block;

            void main() {
                vs_color = color;
                gl_Position = position;
            }

            )");
    auto fragment = Engine::Shader::create_shader_stage(
            GL_FRAGMENT_SHADER,
            R"(
            #version 450 core
            in vec3 vs_color;
            out vec4 color;
            uniform Uniform_block {
                mat4 view_matrix;
                mat4 transform_matrix;
                vec4 color;
            } my_block;
            void main() {
                color = vec4(vs_color,1);
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

    auto triangle_buffer= Engine::Vertex_buffer::Create(&triangle_array[0],triangle_array.size()*sizeof(triangle_array[0]),
                                                        Engine::Raw_buffer::Access_frequency::DYNAMIC,
                                                        Engine::Raw_buffer::Access_type::DRAW);
    Engine::Buffer_layout triangulation_layout = {{Engine::Shader_data::Float2, "position"}, {Engine::Shader_data::Float3, "color"}};
    triangle_buffer->set_layout(triangulation_layout);

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

    auto trianglulation_VAO = Engine::Vertex_array::Create();
    trianglulation_VAO->add_vertex_buffer(triangle_buffer);

    /// -------- Camera --------
    float aspect_ratio = ((float)window->get_width()/window->get_height());
    auto camera = Engine::Orthographic_camera(-aspect_ratio,aspect_ratio,1,-1,0,1);
    camera.on_update(); /// This is the only update needed.

    /// Initialize camera uniform matrix.
    uniforms.view_matrix.m_data = camera.get_view_projection_matrix();
    uniforms.update_view_matrix();



    /// -------- Mouse listener that sets the triangle color --------
    std::vector<std::shared_ptr<Lab2::Triangle>> clicked_triangles;
    std::shared_ptr<Lab2::Triangle> clicked_triangle = nullptr;
    Engine::Event_actor_obj<Engine::Mouse_button_Event> mouse_listener;
    mouse_listener.set_callback_func<Engine::Mouse_button_Event>([window ,tree,triangle_buffer,&triangle_array,&clicked_triangle,&clicked_triangles](Engine::Mouse_button_Event event){
        if (event.action) {
            auto mouse_pos = window->get_cursor_pos();
            float x = (float)std::get<0>(mouse_pos)/window->get_width()*2.0f-1.0f;
            float y = -((float)std::get<1>(mouse_pos)/window->get_height()*2.0f-1.0f);

            Lab2::Vertex v{x,y};


            if (clicked_triangle) {
                for (const auto& tri : clicked_triangles) {
                    float base_offset = tri->triangle_ID*5*3+2;
                    triangle_array[base_offset] = triangle_array[base_offset]-0.4f;
                    triangle_array[base_offset+1] = triangle_array[base_offset+1]-0.4f;
                    triangle_array[base_offset+2] = triangle_array[base_offset+2]-0.4f;

                    triangle_array[base_offset+5] = triangle_array[base_offset+5]-0.4f;
                    triangle_array[base_offset+6] = triangle_array[base_offset+6]-0.4f;
                    triangle_array[base_offset+7] = triangle_array[base_offset+7]-0.4f;

                    triangle_array[base_offset+10] = triangle_array[base_offset+10]-0.4f;
                    triangle_array[base_offset+11] = triangle_array[base_offset+11]-0.4f;
                    triangle_array[base_offset+12] = triangle_array[base_offset+12]-0.4f;

                    triangle_buffer->set_sub_data(&triangle_array[base_offset],base_offset*sizeof(float),13*sizeof(float));
                }

            }
            clicked_triangles.clear();
            if (clicked_triangle = tree->search(v)) {   /// This is not a typo. Don't put == here

                Lab2::color_circle(std::shared_ptr<Lab2::Triangle>(clicked_triangle),clicked_triangles);

                std::cout << "You clicked on " << std::to_string(clicked_triangle->triangle_ID) << std::endl;
                for (const auto& tri : clicked_triangles) {
                    float base_offset = tri->triangle_ID*5*3+2;
                    triangle_array[base_offset] = triangle_array[base_offset]+0.4f;
                    triangle_array[base_offset+1] = triangle_array[base_offset+1]+0.4f;
                    triangle_array[base_offset+2] = triangle_array[base_offset+2]+0.4f;

                    triangle_array[base_offset+5] = triangle_array[base_offset+5]+0.4f;
                    triangle_array[base_offset+6] = triangle_array[base_offset+6]+0.4f;
                    triangle_array[base_offset+7] = triangle_array[base_offset+7]+0.4f;

                    triangle_array[base_offset+10] = triangle_array[base_offset+10]+0.4f;
                    triangle_array[base_offset+11] = triangle_array[base_offset+11]+0.4f;
                    triangle_array[base_offset+12] = triangle_array[base_offset+12]+0.4f;
                    triangle_buffer->set_sub_data(&triangle_array[base_offset],base_offset*sizeof(float),13*sizeof(float));
                }
            }

        }
    });




    /// Misc GL and engine commands.
    glLineWidth(3); /// TODO move to engine.
    glPointSize(10);
    glEnable(GL_POINT_SMOOTH);
    Engine::Render::Render_command::set_clear_color({0.2,0.8,0.3,1});

    auto black = glm::vec4(0,0,0,1);
    auto red = glm::vec4(1,0,0,1);
    auto yellow = glm::vec4 (0.9,0.7,0.2,1);
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    while (!window->should_close()) {
        mouse_listener.on_update();

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
        uniforms.color.m_data = red;
        uniforms.update_color();
        glDrawArrays(GL_POINTS, 0, list.size());
*/


        trianglulation_VAO->bind();
        uniforms.color.m_data = black;
        uniforms.update_color();
        //glDrawElements(GL_TRIANGLES, index_buffer->get_count()/sizeof(uint32_t), GL_UNSIGNED_INT, nullptr);
        glDrawArrays(GL_TRIANGLES, 0, triangle_buffer->get_size()/sizeof(triangle_array[0]));

        line_vertex_array->bind();

        uniforms.color.m_data = red;
        uniforms.update_color();
        glDrawArrays(GL_POINTS, 0, list.size());

        line_vertex_array->unbind();

        shader_program->unbind();

        window->on_update();
    }
    return 0;

}
