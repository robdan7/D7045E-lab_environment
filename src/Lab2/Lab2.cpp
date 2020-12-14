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

#include "Algorithms.h"
#include "Color.h"

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
    std::cout << "OpenGL driver version: "<< glGetString(GL_VERSION) << std::endl;

    /// -------- Shaders --------
    auto vertex = Engine::Shader::create_shader_stage(
            GL_VERTEX_SHADER,
            R"(
            #version 330 core
            in vec4 position;

            void main() {
                gl_Position = position;
            }

            )");
    auto triangle_frag_shader = Engine::Shader::create_shader_stage(
            GL_FRAGMENT_SHADER,
            R"(
            #version 330 core
            out vec4 color;

            void main() {
                color = vec4(1,0,0,1);
            }
            )");

    auto shader = Engine::Shader::create_shader({vertex, triangle_frag_shader});
    shader->compile();


    /// Misc GL and engine commands.
    Engine::Render::Render_command::set_clear_color({0.6,0.6,0.75,1});
    float vertices[] = {
            0,0,
            0.5f,0,
            0.5f,0.5f
    };
    GLuint ID;
    glCreateBuffers(1, &ID);
    glBindBuffer(GL_ARRAY_BUFFER, (GLuint)(ID));
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,GL_STATIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,0);

    shader->bind();
    while (!window->should_close()) {
        Engine::Render::Render_command::clear();

        glDrawArrays(GL_TRIANGLES,0,3);

        window->on_update();
    }
    return 0;

}
