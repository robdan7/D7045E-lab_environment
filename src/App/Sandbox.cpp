#define API_OPENGL
#include <Engine.h>

BEGIN_SHADER_CONST(user_const)
    SET_SHADER_CONST(Cecilion::Matrix4_data, view_matrix)
    SET_SHADER_CONST(Cecilion::Matrix4_data, transform_matrix)
END_SHADER_CONST(user_const)

int main(int argc, char** argv) {
    auto window = Cecilion::Window::create_window();

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
            } my_block;

            void main() {
                gl_Position =my_block.view_matrix*my_block.transform_matrix*position;
            }

            )");
    auto fragment = Cecilion::Shader::create_shader_stage(
            GL_FRAGMENT_SHADER,
            R"(
            #version 450 core
            out vec4 color;
            void main() {
                color = vec4(1,0,0,1);
            }
            )");
    auto shader = Cecilion::Shader::create_shader({vertex, fragment});
    shader->compile();

    GLuint index= glGetUniformBlockIndex(shader->get_ID(), "Uniform_block");
    glUniformBlockBinding(shader->get_ID(), index,0);

    float vertices[] = {-0.25,-0.25,0,1,
                        0.25f,-0.25,0,1,
                        0.25f,0.25f,0,1,
                        0-0.25,0.25f,0,1};
    auto buffer = Cecilion::Vertex_buffer::Create(vertices, sizeof(vertices),
                                                  Cecilion::Vertex_buffer::Access_frequency::STATIC,
                                                  Cecilion::Vertex_buffer::Access_type::DRAW);
    Cecilion::Buffer_layout layout = {{Cecilion::Shader_data::Float4, "position"}};
    buffer->set_layout(layout);
    auto vertex_array = Cecilion::Vertex_array::Create();
    vertex_array->add_vertex_buffer(buffer);
    auto camera = Cecilion::Orthographic_camera(-1,1,1,-1,0,1);


    //params->view_matrix.m_data += glm::vec4(0.35,0,0,1);

     //glm::translate(params->view_matrix.m_data, glm::vec3(0.5f,0,1));
    //params->view_matrix.m_data = glm::inverse(params->view_matrix.m_data);



    //Cecilion::Render::Render_command::enable_depth_test();
    //glm::translate(params->view_matrix.m_data, glm::vec3(0.1,0,0));
    float angle = 0;
    while (!window->should_close()) {

        camera.on_update();
        params->view_matrix.m_data = camera.get_view_projection_matrix();
        params->transform_matrix.m_data = glm::rotate(glm::mat4(1.0f), angle,glm::vec3(0,0,1));

        float test = sin(angle)*0.5+1;
        params->transform_matrix.m_data = glm::scale(params->transform_matrix.m_data, glm::vec3(test,test,test));
        angle += 0.03f;


        params->write((Cecilion::I_data<void *> *) &params->view_matrix);
        params->write((Cecilion::I_data<void *> *) &params->transform_matrix);

        Cecilion::Render::Render_command::set_clear_color({0.8,0.8,0.3,1});
        Cecilion::Render::Render_command::clear();

        //params->view_matrix.m_data = glm::mat4(1.0f);
        //params->view_matrix.m_data += glm::vec4(100.0f,0,0,1);

        shader->bind();
        vertex_array->bind();
        glDrawArrays(GL_QUADS,0,4);
        vertex_array->unbind();
        shader->unbind();
        window->on_update();
    }
}