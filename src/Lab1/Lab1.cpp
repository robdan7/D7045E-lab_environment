#define API_OPENGL
#include <Engine.h>
#include <tuple>

BEGIN_SHADER_CONST(user_const)
    SET_SHADER_CONST(Cecilion::Matrix4_data, view_matrix)
    SET_SHADER_CONST(Cecilion::Matrix4_data, transform_matrix)
    SET_SHADER_CONST(Cecilion::Float4_data, color);
END_SHADER_CONST(user_const)

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
    auto index_a = vec.size()/glm::vec4::length();
    vec.push_back(q0.x);
    vec.push_back(q0.y);
    vec.push_back(q0.z);
    vec.push_back(1);
    koch_helper(q0, n, cc_normal(n - q0), vec,ind, depth + 1, max_depth);
    auto index_b = vec.size()/glm::vec4::length();
    vec.push_back(n.x);
    vec.push_back(n.y);
    vec.push_back(n.z);
    vec.push_back(1);
    koch_helper(n, q1, cc_normal(q1 - n), vec,ind, depth + 1, max_depth);
    auto index_c = vec.size()/glm::vec4::length();
    vec.push_back(q1.x);
    vec.push_back(q1.y);
    vec.push_back(q1.z);
    vec.push_back(1);


    ind.push_back(index_a);
    ind.push_back(index_c);
    ind.push_back(index_b);

    koch_helper(q1, b, normal, vec,ind, depth + 1, max_depth);
}


std::tuple<std::shared_ptr<Cecilion::Vertex_array>,std::shared_ptr<Cecilion::Vertex_array>> create_snowflake(float depth) {
    std::vector<float> vec;
    std::vector<uint32_t> triangle_indices;
    float l = 1;
    float k = l/sqrt(3);
    glm::vec3 a = glm::vec3(-l/2,-k/2,0);
    glm::vec3 b = glm::vec3(0,k,0);
    glm::vec3 c = glm::vec3(l/2,-k/2,0);

    auto index_a = vec.size()/glm::vec4::length();
    vec.push_back(a.x);
    vec.push_back(a.y);
    vec.push_back(0);
    vec.push_back(1);
    //triangle_indices.push_back(0);


    koch_helper(a, b, cc_normal(b - a), vec,triangle_indices, 1, depth);
    auto index_b = vec.size()/glm::vec4::length();
    vec.push_back(b.x);
    vec.push_back(b.y);
    vec.push_back(0);
    vec.push_back(1);
    koch_helper(b, c, cc_normal(c - b), vec,triangle_indices, 1, depth);
    auto index_c = vec.size()/glm::vec4::length();
    vec.push_back(c.x);
    vec.push_back(c.y);
    vec.push_back(0);
    vec.push_back(1);
    koch_helper(c, a, cc_normal(a - c), vec,triangle_indices, 1, depth);
    //auto index_a2 = vec.size()/glm::vec4::length();
    vec.push_back(a.x);
    vec.push_back(a.y);
    vec.push_back(0);
    vec.push_back(1);

    triangle_indices.push_back(index_a);
    triangle_indices.push_back(index_c);
    triangle_indices.push_back(index_b);


    auto buffer = Cecilion::Vertex_buffer::Create(&vec[0], sizeof(float)*vec.size(),
                                                  Cecilion::Vertex_buffer::Access_frequency::STATIC,
                                                  Cecilion::Vertex_buffer::Access_type::DRAW);


    Cecilion::Buffer_layout layout = {{Cecilion::Shader_data::Float4, "position"}};
    buffer->set_layout(layout);

    auto line_strip = Cecilion::Vertex_array::Create();
    //line_strip->set_index_buffer(ind_buffer);
    line_strip->add_vertex_buffer(buffer);


    auto ind_buffer = Cecilion::Index_buffer::Create(&triangle_indices[0], sizeof(uint32_t) * triangle_indices.size());
    auto triangles = Cecilion::Vertex_array::Create();
    triangles->add_vertex_buffer(buffer);
    triangles->set_index_buffer(ind_buffer);

    return {line_strip, triangles};
}

class Test_panel : public Cecilion::ImGui_panel {
public:
    void on_imgui_render() override {
        ImGui_panel::on_imgui_render();
        bool show = true;
        ImGui::ShowDemoWindow(&show);
    }
};


int main(int argc, char** argv) {
    auto window = std::shared_ptr<Cecilion::Window>(Cecilion::Window::create_window());

    auto params = new user_const();


    auto vertex = Cecilion::Shader::create_shader_stage(
            GL_VERTEX_SHADER,
            R"(
            #version 450 core
            in vec4 position;
            //uniform mat4 view_matrix;

            uniform Uniform_block {
                mat4 view_matrix;
                mat4 transform_matrix;
                vec4 color;
            } my_block;

            void main() {
                gl_Position =my_block.view_matrix*my_block.transform_matrix*position;
                //gl_Position =position;
            }

            )");
    auto fragment = Cecilion::Shader::create_shader_stage(
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
    auto shader = Cecilion::Shader::create_shader({vertex, fragment});
    shader->compile();

    GLuint index= glGetUniformBlockIndex(shader->get_ID(), "Uniform_block");
    glUniformBlockBinding(shader->get_ID(), index,0);

    /*float vertices[] = {-0.25,-0.25,0,1,
                        0.25f,-0.25,0,1,
                        0.25f,0.25f,0,1,
                        0-0.25,0.25f,0,1};
                        */
    /*auto vertices = create_snowflake(7);
    auto buffer = Cecilion::Vertex_buffer::Create(&vertices[0], sizeof(float)*vertices.size(),
                                                  Cecilion::Vertex_buffer::Access_frequency::STATIC,
                                                  Cecilion::Vertex_buffer::Access_type::DRAW);

    Cecilion::Buffer_layout layout = {{Cecilion::Shader_data::Float4, "position"}};


    buffer->set_layout(layout);
*/

    auto vertex_array = create_snowflake(4);
    //vertex_array->add_vertex_buffer(buffer);

    float aspect_ratio = ((float)window->get_width()/window->get_height());
    auto camera = Cecilion::Orthographic_camera(-aspect_ratio,aspect_ratio,1,-1,0,1);
    camera.on_update();
    params->view_matrix.m_data = camera.get_view_projection_matrix();
    params->write((Cecilion::I_data<void *> *) &params->view_matrix);

    auto gui = Cecilion::ImGui_layer(window);
    auto panel = std::make_shared<Test_panel>();
    gui.push_panel(panel);

    auto black = glm::vec4(0,0,0,1);
    auto red = glm::vec4(1,0,0,1);

    Cecilion::Render::Render_command::set_clear_color({0.8,0.8,0.3,1});
    float angle = 0;
    glLineWidth(5);
    while (!window->should_close()) {


        params->transform_matrix.m_data = glm::rotate(glm::mat4(1.0f), angle,glm::vec3(0,0,1));

        float test = sin(angle)*0.5+1;
        params->transform_matrix.m_data = glm::scale(params->transform_matrix.m_data, glm::vec3(test,test,test));
        angle += 0.01f;


        params->write((Cecilion::I_data<void *> *) &params->transform_matrix);

        Cecilion::Render::Render_command::clear();
        //gui.on_update();

        //params->view_matrix.m_data = glm::mat4(1.0f);
        //params->view_matrix.m_data += glm::vec4(100.0f,0,0,1);

        params->color.m_data = red;
        params->write((Cecilion::I_data<void *> *) &params->color);

        shader->bind();

        std::get<1>(vertex_array)->bind();
        glDrawElements(GL_TRIANGLES,std::get<1>(vertex_array)->get_index_buffer()->get_count()/sizeof(uint32_t),GL_UNSIGNED_INT, nullptr);

        params->color.m_data = black;
        params->write((Cecilion::I_data<void *> *) &params->color);

        std::get<0>(vertex_array)->bind();
        auto count = std::get<0>(vertex_array)->get_vertex_buffer()[0]->get_size()/sizeof(float)/glm::vec4::length();
        glDrawArrays(GL_LINE_STRIP,0,count);


        std::get<0>(vertex_array)->unbind();
        shader->unbind();

        window->on_update();
    }
}