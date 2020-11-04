#define API_OPENGL
#include <Engine.h>
#include <tuple>

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
glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float f) {
    return a+f*(b-a);
}

/**
 * Return the counter clockwise normal of a line/vector.
 * @param vec
 * @return
 */
glm::vec3 cc_normal(glm::vec3 vec) {
    auto result = glm::vec3(-vec.y,vec.x,0);
    result /= glm::length(result);
    return result;
}


void koch_helper(glm::vec3 a, glm::vec3 b, glm::vec3 normal, std::vector<float>& vec, std::vector<uint32_t>& ind, int depth, int max_depth) {
    if (depth >= max_depth) {
        return;
    }

    auto q0 = lerp(a,b,1/3.0f);
    auto q1 = lerp(a,b,2/3.0f);
    float h = sqrt(3/4.0f)*glm::length(b-a)/3;

    auto n = lerp(a,b, 0.5f)+normal*h;
    koch_helper(a, q0, normal, vec,ind, depth + 1, max_depth);
    auto index_a = vec.size()/glm::vec2::length();
    vec.push_back(q0.x);
    vec.push_back(q0.y);
    //vec.push_back(q0.z);
    //vec.push_back(1);
    koch_helper(q0, n, cc_normal(n - q0), vec,ind, depth + 1, max_depth);
    auto index_b = vec.size()/glm::vec2::length();
    vec.push_back(n.x);
    vec.push_back(n.y);
    //vec.push_back(n.z);
    //vec.push_back(1);
    koch_helper(n, q1, cc_normal(q1 - n), vec,ind, depth + 1, max_depth);
    auto index_c = vec.size()/glm::vec2::length();
    vec.push_back(q1.x);
    vec.push_back(q1.y);
    //vec.push_back(q1.z);
    //vec.push_back(1);


    ind.push_back(index_a);
    ind.push_back(index_c);
    ind.push_back(index_b);

    koch_helper(q1, b, normal, vec,ind, depth + 1, max_depth);
}


void create_snowflake(std::shared_ptr<Engine::Vertex_buffer> vertex_buffer,std::shared_ptr<Engine::Index_buffer> index_buffer,float depth) {
    std::vector<float> vec;
    std::vector<uint32_t> triangle_indices;
    float l = 1;
    float k = l/sqrt(3);
    glm::vec3 a = glm::vec3(-l/2,-k/2,0);
    glm::vec3 b = glm::vec3(0,k,0);
    glm::vec3 c = glm::vec3(l/2,-k/2,0);

    auto index_a = vec.size()/glm::vec2::length();
    vec.push_back(a.x);
    vec.push_back(a.y);
    //vec.push_back(0);
    //vec.push_back(1);
    //triangle_indices.push_back(0);


    koch_helper(a, b, cc_normal(b - a), vec,triangle_indices, 1, depth);
    auto index_b = vec.size()/glm::vec2::length();
    vec.push_back(b.x);
    vec.push_back(b.y);
    //vec.push_back(0);
    //vec.push_back(1);
    koch_helper(b, c, cc_normal(c - b), vec,triangle_indices, 1, depth);
    auto index_c = vec.size()/glm::vec2::length();
    vec.push_back(c.x);
    vec.push_back(c.y);
    //vec.push_back(0);
    //vec.push_back(1);
    koch_helper(c, a, cc_normal(a - c), vec,triangle_indices, 1, depth);
    //auto index_a2 = vec.size()/glm::vec4::length();
    vec.push_back(a.x);
    vec.push_back(a.y);
    //vec.push_back(0);
    //vec.push_back(1);

    triangle_indices.push_back(index_a);
    triangle_indices.push_back(index_c);
    triangle_indices.push_back(index_b);

    vertex_buffer->reset_buffer(&vec[0], sizeof(float)*vec.size());
    index_buffer->reset_buffer(&triangle_indices[0], sizeof(uint32_t) * triangle_indices.size());

/*
    auto buffer = Engine::Vertex_buffer::Create(&vec[0], sizeof(float)*vec.size(),
                                                  Engine::Vertex_buffer::Access_frequency::STATIC,
                                                  Engine::Vertex_buffer::Access_type::DRAW);


    Engine::Buffer_layout layout = {{Engine::Shader_data::Float4, "position"}};
    buffer->set_layout(layout);

    auto line_strip = Engine::Vertex_array::Create();
    //line_strip->set_index_buffer(ind_buffer);
    line_strip->add_vertex_buffer(buffer);


    auto ind_buffer = Engine::Index_buffer::Create(&triangle_indices[0], sizeof(uint32_t) * triangle_indices.size());
    auto triangles = Engine::Vertex_array::Create();
    triangles->add_vertex_buffer(buffer);
    triangles->set_index_buffer(ind_buffer);

    return {line_strip, triangles};
    */
}

class Test_panel : public Engine::ImGui_panel {
public:
    void on_imgui_render() override {
        ImGui_panel::on_imgui_render();
        bool show = true;
        ImGuiWindowFlags window_flags = 0;


        // We specify a default position/size in case there's no data in the .ini file.
        // We only do it to make the demo applications a little more welcoming, but typically this isn't required.
        ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->GetWorkPos().x + 650, main_viewport->GetWorkPos().y + 20), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_FirstUseEver);
        ImGui::Begin("my_window");
        ImGui::SliderInt("depth", &this->m_value, 1, 7);
        ImGui::End();
        //ImGui::ShowDemoWindow(&show);
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
    auto uniforms = new Global_uniforms();

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

    auto index_buffer = Engine::Index_buffer::Create(nullptr, 0,Engine::Raw_buffer::Access_frequency::DYNAMIC); /// TODO Enable index buffer layouts.


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
    uniforms->view_matrix.m_data = camera.get_view_projection_matrix();
    uniforms->write((Engine::I_data<void *> *) &uniforms->view_matrix);

    /// -------- GUI --------
    auto gui = Engine::ImGui_layer(window);
    auto panel = std::make_shared<Test_panel>();
    gui.push_panel(panel);


    /// -------- Variables --------
    auto black = glm::vec4(0,0,0,1);
    auto red = glm::vec4(1,0,0,1);
    float angle = 0;
    int current_depth = 1;

    /// Misc GL and engine commands.
    glLineWidth(2); /// TODO move to engine.
    Engine::Render::Render_command::set_clear_color({0.8,0.8,0.3,1});

    while (!window->should_close()) {
        /// Update transform.
        float scale = sin(angle) * 0.5 + 1;
        uniforms->transform_matrix.m_data = glm::rotate(glm::mat4(1.0f), 0.25f*angle, glm::vec3(0, 0, 1));
        uniforms->transform_matrix.m_data = glm::translate(uniforms->transform_matrix.m_data, glm::vec3(0.75f,0,0));
        uniforms->transform_matrix.m_data = glm::rotate(uniforms->transform_matrix.m_data, angle, glm::vec3(0, 0, 1));
        uniforms->transform_matrix.m_data = glm::scale(uniforms->transform_matrix.m_data, glm::vec3(scale, scale, scale));
        angle += 0.02f;

        /// Update translation uniform.
        uniforms->write((Engine::I_data<void *> *) &uniforms->transform_matrix);

        /// Clear frame buffer and update GUI
        Engine::Render::Render_command::clear();

        shader_program->bind();

        /// Draw filled snowflake.
        uniforms->color.m_data = red;
        uniforms->write((Engine::I_data<void *> *) &uniforms->color);
        triangle_vertex_array->bind();
        glDrawElements(GL_TRIANGLES,index_buffer->get_count()/sizeof(uint32_t),GL_UNSIGNED_INT, nullptr);

        /// Draw snowflake lines.
        uniforms->color.m_data = black;
        uniforms->write((Engine::I_data<void *> *) &uniforms->color);
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

    delete uniforms;
}