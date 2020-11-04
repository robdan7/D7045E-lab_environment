#define API_OPENGL
#include <Engine.h>
#include <cmath>
#include <tuple>
#include <functional>
#define PI (atan(1)*4)

BEGIN_SHADER_CONST(Global_uniforms)
    SET_SHADER_CONST(Engine::Matrix4_data, view_matrix)
    SET_SHADER_CONST(Engine::Matrix4_data, transform_matrix)
    SET_SHADER_CONST(Engine::Float4_data, color);
END_SHADER_CONST(Global_uniforms)

/**
 * Lerp-a-derp-a schmerp lurp.
 * @param a
 * @param b
 * @param f
 * @return
 */
glm::vec2 lerp(const glm::vec2& a, const glm::vec2& b, float f) {
    return a+f*(b-a);
}

/**
 * Return the counter clockwise normal of a vector.
 * @param vec
 * @return
 */
glm::vec2 cc_normal(const glm::vec2 vec) {
    auto result = glm::vec2(-vec.y,vec.x);
    result /= glm::length(result);
    return result;
}


void koch_helper(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& normal, std::vector<float>& vec, std::vector<uint32_t>& ind, uint32_t depth, uint32_t max_depth) {
    if (depth >= max_depth) {
        return;
    }

    auto q0 = lerp(p0, p1, 1 / 3.0f);
    auto q1 = lerp(p0, p1, 2 / 3.0f);
    float h = std::sqrt(3/4.0f) * glm::length(p1 - p0) / 3;  /// Distance from [p0,p1] to a.
    auto a = lerp(p0, p1, 0.5f) + normal * h;

    koch_helper(p0, q0, normal, vec, ind, depth + 1, max_depth);
    auto index_q0 = vec.size() / glm::vec2::length();
    vec.push_back(q0.x);
    vec.push_back(q0.y);

    koch_helper(q0, a, cc_normal(a - q0), vec, ind, depth + 1, max_depth);
    auto index_a = vec.size() / glm::vec2::length();
    vec.push_back(a.x);
    vec.push_back(a.y);

    koch_helper(a, q1, cc_normal(q1 - a), vec, ind, depth + 1, max_depth);
    auto index_q1 = vec.size() / glm::vec2::length();
    vec.push_back(q1.x);
    vec.push_back(q1.y);

    koch_helper(q1, p1, normal, vec, ind, depth + 1, max_depth);

    /// Create a triangle from stored indices.
    ind.push_back(index_q0);
    ind.push_back(index_q1);
    ind.push_back(index_a);
}


void create_snowflake(const std::shared_ptr<Engine::Vertex_buffer>& vertex_buffer,const std::shared_ptr<Engine::Index_buffer>& index_buffer,uint32_t depth) {
    std::vector<float> vec;
    std::vector<uint32_t> triangle_indices;
    float l = 1;
    float k = l/(float)std::sqrt(3);
    auto a = glm::vec2(-l/2,-k/2);
    auto b = glm::vec2(0,k);
    auto c = glm::vec2(l/2,-k/2);

    auto index_a = vec.size()/glm::vec2::length();
    vec.push_back(a.x);
    vec.push_back(a.y);

    koch_helper(a, b, cc_normal(b - a), vec,triangle_indices, 1, depth);
    auto index_b = vec.size()/glm::vec2::length();
    vec.push_back(b.x);
    vec.push_back(b.y);

    koch_helper(b, c, cc_normal(c - b), vec,triangle_indices, 1, depth);
    auto index_c = vec.size()/glm::vec2::length();
    vec.push_back(c.x);
    vec.push_back(c.y);

    koch_helper(c, a, cc_normal(a - c), vec,triangle_indices, 1, depth);
    vec.push_back(a.x);
    vec.push_back(a.y);

    triangle_indices.push_back(index_a);
    triangle_indices.push_back(index_c);
    triangle_indices.push_back(index_b);

    vertex_buffer->reset_buffer(&vec[0], sizeof(float)*vec.size());
    index_buffer->reset_buffer(&triangle_indices[0], sizeof(uint32_t) * triangle_indices.size());
}

class Slider_panel : public Engine::ImGui_panel {
public:
    void on_imgui_render() override {
        ImGui_panel::on_imgui_render();

        ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->GetWorkPos().x + 650, main_viewport->GetWorkPos().y + 20), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_FirstUseEver);
        ImGui::Begin("Slider window");
        ImGui::SliderInt("Depth", &this->m_value, 1, 7);
        ImGui::End();
    }
    int get_value() {
        return this->m_value;
    }
private:
    int m_value = 1;
};


int main(int argc, char** argv) {
    /// -------- Window and uniforms --------
    auto window = std::shared_ptr<Engine::Window>(Engine::Window::create_window());
    auto uniforms = Global_uniforms();

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
                gl_Position =my_block.view_matrix * my_block.transform_matrix * position;
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
    auto vertex_buffer = Engine::Vertex_buffer::Create(nullptr, 0,
                                                         Engine::Vertex_buffer::Access_frequency::DYNAMIC,
                                                         Engine::Vertex_buffer::Access_type::DRAW);

    /// Set the layout of the vertex buffer. This tells the VAO what the vertex attributes should be.
    Engine::Buffer_layout layout = {{Engine::Shader_data::Float2, "position"}};
    vertex_buffer->set_layout(layout);

    auto index_buffer = Engine::Index_buffer::Create(nullptr, 0,Engine::Raw_buffer::Access_frequency::DYNAMIC);


    /// -------- VAOs --------
    auto line_vertex_array = Engine::Vertex_array::Create();
    line_vertex_array->add_vertex_buffer(vertex_buffer);

    auto triangle_vertex_array = Engine::Vertex_array::Create();
    triangle_vertex_array->add_vertex_buffer(vertex_buffer);
    triangle_vertex_array->set_index_buffer(index_buffer);

    /// Fill the buffers with vertices.
    create_snowflake(vertex_buffer, index_buffer, 1);

    /// -------- Camera --------
    float aspect_ratio = ((float)window->get_width()/window->get_height());
    auto camera = Engine::Orthographic_camera(-aspect_ratio,aspect_ratio,1,-1,0,1);
    camera.on_update(); /// This is the only update needed.

    /// Initialize camera uniform matrix.
    uniforms.view_matrix.m_data = camera.get_view_projection_matrix();
    uniforms.write((Engine::I_data<void *> *) &uniforms.view_matrix);

    /// -------- GUI --------
    auto gui = Engine::ImGui_layer(window);
    auto panel = std::make_shared<Slider_panel>();
    gui.push_panel(panel);


    /// -------- Variables --------
    auto black = glm::vec4(0,0,0,1);
    auto red = glm::vec4(1,0,0,1);
    float angle = 0;
    int current_depth = 1;

    /// Misc GL and engine commands.
    glLineWidth(3); /// TODO move to engine.
    Engine::Render::Render_command::set_clear_color({0.8,0.8,0.3,1});

    while (!window->should_close()) {
        angle = std::fmod(angle+0.025f,8*PI);   /// Reset angle after 1 full rotation around window center.
        /// Circle around window center
        uniforms.transform_matrix.m_data = glm::rotate(glm::mat4(1.0f), 0.25f*angle, glm::vec3(0, 0, 1));
        uniforms.transform_matrix.m_data = glm::translate(uniforms.transform_matrix.m_data, glm::vec3(0.75f,0,0));
        /// Rotate around object origin.
        uniforms.transform_matrix.m_data = glm::rotate(uniforms.transform_matrix.m_data, angle, glm::vec3(0, 0, 1));

        float scale = sin(angle) * 0.5f + 1;
        uniforms.transform_matrix.m_data = glm::scale(uniforms.transform_matrix.m_data, glm::vec3(scale, scale, scale));


        /// Update transform uniform.
        uniforms.write((Engine::I_data<void *> *) &uniforms.transform_matrix);

        /// Clear frame buffer and update GUI
        Engine::Render::Render_command::clear();

        shader_program->bind();

        /// Draw filled snowflake.
        uniforms.color.m_data = red;
        uniforms.write((Engine::I_data<void *> *) &uniforms.color);
        triangle_vertex_array->bind();
        glDrawElements(GL_TRIANGLES,index_buffer->get_count()/sizeof(uint32_t),GL_UNSIGNED_INT, nullptr);

        /// Draw snowflake lines.
        uniforms.color.m_data = black;
        uniforms.write((Engine::I_data<void *> *) &uniforms.color);
        line_vertex_array->bind();
        glDrawArrays(GL_LINE_STRIP,0,line_vertex_array->get_vertex_buffer()[0]->get_size()/sizeof(float)/glm::vec2::length());


        line_vertex_array->unbind();
        shader_program->unbind();

        /// Update GUI and snowflake depth.
        gui.on_update();
        if (panel->get_value() != current_depth) {
            create_snowflake(vertex_buffer, index_buffer, panel->get_value());
            current_depth = panel->get_value();
        }
        window->on_update();
    }
}