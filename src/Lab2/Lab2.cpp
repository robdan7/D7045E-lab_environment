#define API_OPENGL
#include <Engine.h>
#include <functional>
#include <ctime>
#include "Triangle.h"
#include "Search_tree.h"
#define PI (atan(1)*4)
BEGIN_SHADER_CONST(Global_uniforms)
    SET_SHADER_CONST(Engine::Float4_data, color);
END_SHADER_CONST(Global_uniforms)

#include "Vertex.h"
#include "Algorithms.h"
#include <iostream>

double pi = 2 * acos(0.0);
Lab2::Color color_A{86/255.0f,24/255.0f,108/255.0f};
Lab2::Color color_B{149/255.0f,29/255.0f,59/255.0f};
Lab2::Color color_C{190/155.0f,143/255.0f,28/255.0f};
Lab2::Color color_D{31/255.0f,161/255.0f,31/255.0f};

struct Singleton_state {
    Singleton_state() {
        if (created) {
            throw std::exception();
        }
        created = true;
    }
    std::vector<Lab2::Vertex> vertex_array;
    std::vector<uint32_t> hull_indices;
    std::vector<std::shared_ptr<Lab2::Triangle>> triangles;
    std::vector<Lab2::Color> triangle_colors;
    std::vector<float> triangle_array;
    std::vector<std::shared_ptr<Lab2::Triangle>> clicked_triangles;
    std::shared_ptr<Lab2::Search_tree> search_tree;

    void clear_state() {
        this->vertex_array.clear();
        this->hull_indices.clear();
        this->triangles.clear();
        this->triangle_colors.clear();
        this->triangle_array.clear();
        this->clicked_triangles.clear();
    }
private:
    static bool created;
};
bool Singleton_state::created = false;


/**
 * Create a list with randomized vertices.
 * @param list
 * @param size
 */
void randomize_points(std::vector<Lab2::Vertex>& list, uint32_t size) {
    std::srand(std::time(nullptr));
    for (uint32_t i = 0 ; i < size; ++i) {
        auto x = (rand() % (2000)-1000)/1000.0f;
        auto y = (rand() % (2000)-1000)/1000.0f;
        list.emplace_back(x,y);
    }
}

/**
 * Scramble a list that has already been filled with vertices.
 * @param list
 */
void scramble_points(std::vector<Lab2::Vertex>& list) {
    std::srand(std::time(nullptr));
    for (int i = 0; i < list.size(); ++i) {
        auto index = rand() % list.size();
        auto vertex = list[index];
        list[index] = list[i];
        list[i] = vertex;
    }
}

class Selector_panel : public Engine::ImGui_panel {
public:
    void on_imgui_render() override {
        ImGui_panel::on_imgui_render();

        ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->GetWorkPos().x + 650, main_viewport->GetWorkPos().y + 20), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_FirstUseEver);
        ImGui::Begin("Selection window");

        if (ImGui::Button("Random triangles")) {
            pressed_random = true;
        }
        if (pressed_random) {
            ImGui::SameLine();
            ImGui::Text("Selected");
        }
        if (ImGui::Button("Static triangles")) {
            pressed_random = false;
        }
        if (!pressed_random) {
            ImGui::SameLine();
            ImGui::Text("Selected");
        }

        ImGui::End();
    }
    bool is_random() {
        return this->pressed_random;
    }
private:
    bool pressed_random = false;
};

/**
 * This loads the triangle vertex buffer with points + colors.
 * @param triangle_source
 * @param triangle_dest
 * @param triangle_colors
 */
void create_triangle_array(std::vector<std::shared_ptr<Lab2::Triangle>>& triangle_source, std::vector<float>& triangle_dest, std::vector<Lab2::Color>& triangle_colors) {
    float i = 0;
    for (const auto& tri : triangle_source) {
        triangle_dest.push_back(tri->a->x);
        triangle_dest.push_back(tri->a->y);
        triangle_dest.push_back(triangle_colors[tri->triangle_ID].r);
        triangle_dest.push_back(triangle_colors[tri->triangle_ID].g);
        triangle_dest.push_back(triangle_colors[tri->triangle_ID].b);

        triangle_dest.push_back(tri->b->x);
        triangle_dest.push_back(tri->b->y);
        triangle_dest.push_back(triangle_colors[tri->triangle_ID].r);
        triangle_dest.push_back(triangle_colors[tri->triangle_ID].g);
        triangle_dest.push_back(triangle_colors[tri->triangle_ID].b);

        triangle_dest.push_back(tri->c->x);
        triangle_dest.push_back(tri->c->y);
        triangle_dest.push_back(triangle_colors[tri->triangle_ID].r);
        triangle_dest.push_back(triangle_colors[tri->triangle_ID].g);
        triangle_dest.push_back(triangle_colors[tri->triangle_ID].b);
        i++;
    }
}

/**
 * Generic helper function for initializing triangles.
 * @param state
 * @param vertex_buffer
 * @param triangle_buffer
 */
void reset_triangles_generic(Singleton_state& state,
                        std::shared_ptr<Engine::Vertex_buffer> vertex_buffer,
                        std::shared_ptr<Engine::Vertex_buffer> triangle_buffer,
                             std::shared_ptr<Engine::Index_buffer> convex_hull_buffer) {
    Lab2::sort(state.vertex_array);
    //hull_indices.clear();
    Lab2::calc_convex_hull(state.vertex_array,state.hull_indices);
    state.search_tree = Lab2::build(state.hull_indices, state.vertex_array, state.triangles);
    Lab2::split(state.search_tree, state.hull_indices, state.vertex_array, state.triangles);
    {
        std::vector<bool> visited_triangles(state.triangles.size());
        state.triangle_colors.resize(state.triangles.size());
        Lab2::color(state.triangles[0],visited_triangles,state.triangle_colors,&color_A,&color_B,&color_C,&color_D);
    }
    create_triangle_array(state.triangles, state.triangle_array, state.triangle_colors);
    vertex_buffer->reset_buffer((float*)&state.vertex_array[0],state.vertex_array.size() * 2 * sizeof(float));
    triangle_buffer->reset_buffer((float*)&state.triangle_array[0],state.triangle_array.size() * sizeof(state.triangle_array[0]));
    convex_hull_buffer->reset_buffer((uint32_t*)&state.hull_indices[0], state.hull_indices.size()*sizeof(state.hull_indices[0]));
}

/**
 * Reset function for initializing random triangles.
 * @param size
 * @param state
 * @param vertex_buffer
 * @param triangle_buffer
 */
void reset_random_triangles(uint32_t size,
                            Singleton_state& state,
                            std::shared_ptr<Engine::Vertex_buffer> vertex_buffer,
                            std::shared_ptr<Engine::Vertex_buffer> triangle_buffer,
                            std::shared_ptr<Engine::Index_buffer> convex_hull_buffer) {
    state.clear_state();
    randomize_points(state.vertex_array, size);
    reset_triangles_generic(state,vertex_buffer,triangle_buffer,convex_hull_buffer);
}

/**
 * Reset function for initializing hard coded triangles.
 * @param state
 * @param vertex_buffer
 * @param triangle_buffer
 */
void reset_fixed_triangles(Singleton_state& state,
                           std::shared_ptr<Engine::Vertex_buffer> vertex_buffer,
                           std::shared_ptr<Engine::Vertex_buffer> triangle_buffer,
                           std::shared_ptr<Engine::Index_buffer> convex_hull_buffer) {
    state.clear_state();
    state.vertex_array.emplace_back(-0.8f,-0.8f);
    state.vertex_array.emplace_back(-0.8f,0.0f);
    state.vertex_array.emplace_back(0.0f,0.8f);
    state.vertex_array.emplace_back(0.8f,0.8f);
    state.vertex_array.emplace_back(0.8f,0.0f);
    state.vertex_array.emplace_back(0.0f,-0.8f);
    //state.vertex_array.emplace_back(0.25f,-0.4f);


    state.vertex_array.emplace_back(0.4f,0.4f);


    state.vertex_array.emplace_back(-0.2f,0.0f);
    state.vertex_array.emplace_back(-0.5f,0.0f);
    state.vertex_array.emplace_back(0.0f,0.4f);
    state.vertex_array.emplace_back(0.0f,0.0f);

    state.vertex_array.emplace_back(0.8f,0.4f);

    state.vertex_array.emplace_back(0.0f,-0.4f);
    state.vertex_array.emplace_back(-0.6f,0.0f);
    state.vertex_array.emplace_back(0.4f,-0.4f);
    state.vertex_array.emplace_back(-0.4f,-0.4f);
    state.vertex_array.emplace_back(-0.4f,-0.8f);

    scramble_points(state.vertex_array);
    reset_triangles_generic(state,vertex_buffer,triangle_buffer,convex_hull_buffer);
}

int main(int argc, char** argv) {
    auto window = std::shared_ptr<Engine::Window>(Engine::Window::create_window());
    Global_uniforms uniforms;
    /// -------- GUI --------
    auto gui = Engine::ImGui_layer(window);
    auto selection_panel = std::make_shared<Selector_panel>();
    gui.push_panel(selection_panel);
    Singleton_state singleton;

    /// -------- Shaders --------
    auto vertex = Engine::Shader::create_shader_stage(
            GL_VERTEX_SHADER,
            R"(
            #version 450 core
            in vec4 position;
            in vec3 color;
            out vec3 vs_color;

            void main() {
                vs_color = color;
                gl_Position = position;
            }

            )");
    auto triangle_frag_shader = Engine::Shader::create_shader_stage(
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

    auto dot_frag_shader = Engine::Shader::create_shader_stage(
            GL_FRAGMENT_SHADER,
            R"(
            #version 450 core
            out vec4 color;
            uniform Uniform_block {
                vec4 color;
            } my_block;

            void main() {
                color = my_block.color;
            }
            )");

    auto triangle_shader = Engine::Shader::create_shader({vertex, triangle_frag_shader});
    triangle_shader->compile();
    /// The uniform bindings must be done manually :(
    GLuint index= glGetUniformBlockIndex(triangle_shader->get_ID(), "Uniform_block");
    glUniformBlockBinding(triangle_shader->get_ID(), index, 0);

    /// Special shader for points and lines.
    auto dot_shader = Engine::Shader::create_shader({vertex,dot_frag_shader});
    dot_shader->compile();

    /// -------- Buffers --------

    /// VBO for all points.
    auto point_buffer = Engine::Vertex_buffer::Create(nullptr, 0,
                                                      Engine::Raw_buffer::Access_frequency::DYNAMIC,
                                                      Engine::Raw_buffer::Access_type::DRAW);
    /// Set the layout of the vertex buffer. This tells the VAO what the vertex attributes should be.
    Engine::Buffer_layout point_layout = {{Engine::Shader_data::Float2, "position"}};
    point_buffer->set_layout(point_layout);

    /// VBO for filled triangles.
    auto triangle_buffer= Engine::Vertex_buffer::Create(nullptr, 0,
                                                        Engine::Raw_buffer::Access_frequency::DYNAMIC,
                                                        Engine::Raw_buffer::Access_type::DRAW);

    auto convex_hull_buffer = Engine::Index_buffer::Create(nullptr,0,Engine::Raw_buffer::Access_frequency::DYNAMIC);

    /// Every triangle will also have a color for each vertex.
    Engine::Buffer_layout triangulation_layout = {{Engine::Shader_data::Float2, "position"}, {Engine::Shader_data::Float3, "color"}};
    triangle_buffer->set_layout(triangulation_layout);

    /// This creates the actual triangles.
    if (selection_panel->is_random()) {
        reset_random_triangles(15, singleton, point_buffer, triangle_buffer,convex_hull_buffer);
    } else {
        reset_fixed_triangles(singleton, point_buffer, triangle_buffer,convex_hull_buffer);
    }

    /// -------- VAOs --------
    auto point_VAO = Engine::Vertex_array::Create();
    point_VAO->add_vertex_buffer(point_buffer);

    auto trianglulation_VAO = Engine::Vertex_array::Create();
    trianglulation_VAO->add_vertex_buffer(triangle_buffer);

    auto convex_hull_VAO = Engine::Vertex_array::Create();
    convex_hull_VAO->set_index_buffer(convex_hull_buffer);
    convex_hull_VAO->add_vertex_buffer(point_buffer);

    /// -------- Mouse listener that sets the triangle color --------
    std::shared_ptr<Lab2::Triangle> clicked_triangle = nullptr;
    /// This is a generic, single threaded, event listener/actor that will receive events from the application window.
    Engine::Event_actor_obj<Engine::Mouse_button_Event> mouse_listener;
    mouse_listener.set_callback_func<Engine::Mouse_button_Event>([window ,&singleton, &clicked_triangle,triangle_buffer](Engine::Mouse_button_Event event){
        if (event.action) {
            auto mouse_pos = window->get_cursor_pos();
            /// Convert screen mouse position to "world" position.
            float x = (float)std::get<0>(mouse_pos)/window->get_width()*2.0f-1.0f;
            float y = -((float)std::get<1>(mouse_pos)/window->get_height()*2.0f-1.0f);
            Lab2::Vertex v{x,y};

            if (clicked_triangle) {
                for (const auto& tri : singleton.clicked_triangles) {
                    /// This code is repeated. I know
                    float base_offset = tri->triangle_ID*5*3+2;
                    singleton.triangle_array[base_offset] = singleton.triangle_array[base_offset] - 0.4f;
                    singleton.triangle_array[base_offset + 1] = singleton.triangle_array[base_offset + 1] - 0.5f;
                    singleton.triangle_array[base_offset + 2] = singleton.triangle_array[base_offset + 2] - 0.5f;

                    singleton.triangle_array[base_offset + 5] = singleton.triangle_array[base_offset + 5] - 0.5f;
                    singleton.triangle_array[base_offset + 6] = singleton.triangle_array[base_offset + 6] - 0.5f;
                    singleton.triangle_array[base_offset + 7] = singleton.triangle_array[base_offset + 7] - 0.5f;

                    singleton.triangle_array[base_offset + 10] = singleton.triangle_array[base_offset + 10] - 0.5f;
                    singleton.triangle_array[base_offset + 11] = singleton.triangle_array[base_offset + 11] - 0.5f;
                    singleton.triangle_array[base_offset + 12] = singleton.triangle_array[base_offset + 12] - 0.5f;

                    triangle_buffer->set_sub_data(&singleton.triangle_array[base_offset], base_offset * sizeof(float), 13 * sizeof(float));
                }

            }
            singleton.clicked_triangles.clear();
            if (clicked_triangle = singleton.search_tree->search(v)) {   /// This is not a typo. Don't put == here
                //std::cout << "You clicked on " << std::to_string(clicked_triangle->triangle_ID) << std::endl;
                Lab2::color_circle(std::shared_ptr<Lab2::Triangle>(clicked_triangle),singleton.clicked_triangles);
                singleton.clicked_triangles.push_back(clicked_triangle);
                for (const auto& tri : singleton.clicked_triangles) {
                    /// This code is repeated. I know
                    float base_offset = tri->triangle_ID*5*3+2;
                    singleton.triangle_array[base_offset] = singleton.triangle_array[base_offset] + 0.4f;
                    singleton.triangle_array[base_offset + 1] = singleton.triangle_array[base_offset + 1] + 0.5f;
                    singleton.triangle_array[base_offset + 2] = singleton.triangle_array[base_offset + 2] + 0.5f;

                    singleton.triangle_array[base_offset + 5] = singleton.triangle_array[base_offset + 5] + 0.5f;
                    singleton.triangle_array[base_offset + 6] = singleton.triangle_array[base_offset + 6] + 0.5f;
                    singleton.triangle_array[base_offset + 7] = singleton.triangle_array[base_offset + 7] + 0.5f;

                    singleton.triangle_array[base_offset + 10] = singleton.triangle_array[base_offset + 10] + 0.5f;
                    singleton.triangle_array[base_offset + 11] = singleton.triangle_array[base_offset + 11] + 0.5f;
                    singleton.triangle_array[base_offset + 12] = singleton.triangle_array[base_offset + 12] + 0.5f;
                    triangle_buffer->set_sub_data(&singleton.triangle_array[base_offset], base_offset * sizeof(float), 13 * sizeof(float));
                }
            }
        }
    });


    /// Misc GL and engine commands.
    glLineWidth(2.5f);
    glPointSize(10);
    glEnable(GL_POINT_SMOOTH);
    Engine::Render::Render_command::set_clear_color({0.6,0.6,0.75,1});

    auto black = glm::vec4(0,0,0,1);
    auto white = glm::vec4 (1,1,1,1);
    bool triangle_state = selection_panel->is_random();
    while (!window->should_close()) {
        Engine::Render::Render_command::clear();
        mouse_listener.on_update();

        triangle_shader->bind();
        trianglulation_VAO->bind();
        /// Render filled triangles
        glDrawArrays(GL_TRIANGLES, 0, triangle_buffer->get_size()/sizeof(singleton.triangle_array[0]));

        dot_shader->bind();
        uniforms.color.m_data = black;
        uniforms.update_color();
        /// Render triangle outlines
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        glDrawArrays(GL_TRIANGLES, 0, triangle_buffer->get_size()/sizeof(singleton.triangle_array[0]));
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        point_VAO->bind();
        /// Render points
        glDrawArrays(GL_POINTS, 0, singleton.vertex_array.size());
        convex_hull_VAO->bind();
        uniforms.color.m_data = white;
        uniforms.update_color();
        glDrawElements(GL_POINTS,singleton.hull_indices.size(),GL_UNSIGNED_INT,nullptr);
        convex_hull_VAO->unbind();
        dot_shader->unbind();

        gui.on_update();
        /// Clear all triangles and start over if the user pressed a button
        {
            bool temp_state = selection_panel->is_random();
            if (temp_state != triangle_state) {
                triangle_state = temp_state;
                if(triangle_state) {
                    reset_random_triangles(15, singleton, point_buffer, triangle_buffer,convex_hull_buffer);
                } else {
                    reset_fixed_triangles(singleton, point_buffer, triangle_buffer,convex_hull_buffer);
                }
            }
        }
        window->on_update();
    }
    return 0;

}
