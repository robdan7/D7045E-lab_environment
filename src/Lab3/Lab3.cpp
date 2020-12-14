#define API_OPENGL
#include <Engine.h>
#include "GraphicsNode.h"
#include "Monochrome_material.h"
#define PI (atan(1)*4)

/// Global uniform container.
BEGIN_SHADER_CONST(Global_uniforms)
    SET_SHADER_CONST(Engine::Matrix4_data, view_matrix)
    SET_SHADER_CONST(Engine::Matrix4_data, projection_matrix)
END_SHADER_CONST(Global_uniforms)

/**
 * Disclaimer: I changed the simple, uniform and distance based, lighting to a regular diffuse light.
 * It's so much prettier
 */

class Settings_panel : public Engine::ImGui_panel {
public:
    void on_imgui_render() override {
        ImGui_panel::on_imgui_render();

        ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->GetWorkPos().x + 20, main_viewport->GetWorkPos().y + 500), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(550, 200), ImGuiCond_FirstUseEver);
        ImGui::Begin("Settings");
        ImGui::SetWindowFontScale(1.2f);
        std::string text = "Objects rendered: ";
        text += std::to_string(this->objects);
        ImGui::Text(text.c_str());
        ImGui::SliderFloat("Camera yaw", &this->m_yaw, -PI, PI);
        ImGui::SliderFloat("Camera pitch",&this->m_pitch, -PI/2+0.01,PI/2-0.01);
        ImGui::SliderFloat("Camera center offset", &this->m_distance, 5,130);
        ImGui::SliderFloat("Camera FOV", &this->m_fov, 10, 90);
        /*if (ImGui::Button("Toggle view frustum")) {
            this->frustum_update = !this->frustum_update;
        }*/
        ImGui::SameLine();
        /*if (this->frustum_update) {
            ImGui::Text("Enabled");
        } else {
            ImGui::Text("Disabled");
        }*/
        ImGui::End();
    }
    const float& yaw() {
        return this->m_yaw;
    }
    const float& pitch() {
        return this->m_pitch;
    }
    const float& offset() {
        return this->m_distance;
    }
    /*const bool& update_frustum() {
        return this->frustum_update;
    }*/
    const float& fov() {
        return this->m_fov;
    }
    void set_objects(int obj) {
        this->objects = obj;
    }

private:
    float m_yaw = 0;
    float m_pitch = -0.5;
    float m_distance = 15;
    //bool frustum_update = true;
    int objects = 0;
    float m_fov = 45;
};



int main(int argc, char** argv) {
    auto window = std::shared_ptr<Engine::Window>(Engine::Window::create_window());
    Global_uniforms uniforms;

    auto gui = Engine::ImGui_layer(window);
    auto panel = std::make_shared<Settings_panel>();
    gui.push_panel(panel);

    /// -------- Shaders --------
    auto vertex = Engine::Shader::create_shader_stage(
            GL_VERTEX_SHADER,
            R"(
            #version 450 core
            layout(location = 0) in vec3 position;
            layout(location = 1) in vec3 normal;
            out vec4 vs_color;
            layout(location = 2) in mat4 transform;

            uniform Uniform_block {
                mat4 view_matrix;
                mat4 projection_matrix;
            } my_block;

            layout(location = 1) uniform vec4 diffuse_color;

            void main() {
                vs_color = diffuse_color*max(0.1,dot(normal, normalize(vec3(0.3,0.7,0.2))));
                vec4 in_position = vec4(position,1);
                gl_Position = my_block.projection_matrix * my_block.view_matrix*transform*in_position;
            }

            )");
    auto fragment = Engine::Shader::create_shader_stage(
            GL_FRAGMENT_SHADER,
            R"(
            #version 450 core
            in vec4 vs_color;
            out vec4 color;

            uniform Uniform_block {
                mat4 view_matrix;
                mat4 projection_matrix;
            } my_block;


            void main() {
                color = vs_color;
            }
            )");

    auto shader_program = Engine::Shader::create_shader({vertex, fragment});
    shader_program->compile();

    /// The uniform bindings must be done manually.
    GLuint index= glGetUniformBlockIndex(shader_program->get_ID(), "Uniform_block");
    glUniformBlockBinding(shader_program->get_ID(), index, 0);


    /// -------- Camera --------
    float aspect_ratio = ((float)window->get_width()/window->get_height());
    auto camera = Engine::Perspective_camera(0.01f,1000.0f,panel->fov(),aspect_ratio);
    camera.set_position_axis_aligned({0.0f,0.0f,30.0f});
    camera.look_at({0.0f,0.0f,0.0f});
    camera.on_update(); /// This is the only update needed.

    uniforms.view_matrix.m_data = camera.get_view_matrix();
    uniforms.projection_matrix.m_data = camera.get_projection_matrix();
    uniforms.update_view_matrix();
    uniforms.update_projection_matrix();

    /// Create raw mesh source
    Engine::Import_mesh mesh_data;

    /// vertex, normal
    mesh_data.vertices = {-0.5f, -0.5f, 0.5f, 0, 0, 1,
                          0.5f, -0.5f, 0.5f, 0, 0, 1,
                          0.5f, 0.5f, 0.5f, 0, 0, 1,
                          -0.5f, -0.5f, 0.5f, 0, 0, 1,
                          0.5f, 0.5f, 0.5f, 0, 0, 1,
                          -0.5f, 0.5f, 0.5f, 0, 0, 1,

                          0.5f,-0.5f,-0.5f,0,0,-1,
                          -0.5f,-0.5f,-0.5f,0,0,-1,
                          -0.5f,0.5f,-0.5f,0,0,-1,
                          0.5f,-0.5f,-0.5f,0,0,-1,
                          -0.5f,0.5f,-0.5f,0,0,-1,
                          0.5f,0.5f,-0.5f,0,0,-1,

                          0.5f,-0.5f,0.5f,1,0,0,
                          0.5f,-0.5f,-0.5f,1,0,0,
                          0.5f,0.5f,-0.5f,1,0,0,
                          0.5f,-0.5f,0.5f,1,0,0,
                          0.5f,0.5f,-0.5f,1,0,0,
                          0.5f,0.5f,0.5f,1,0,0,

                          -0.5f,-0.5f,-0.5f,-1,0,0,
                          -0.5f,-0.5f,0.5f,-1,0,0,
                          -0.5f,0.5f,0.5f,-1,0,0,
                          -0.5f,-0.5,-0.5f,-1,0,0,
                          -0.5f,0.5f,0.5f,-1,0,0,
                          -0.5f,0.5f,-0.5f,-1,0,0,

                          0.5f,0.5f,0.5f,0,1,0,
                          0.5f,0.5f,-0.5f,0,1,0,
                          -0.5f,0.5f,-0.5f,0,1,0,
                          0.5f,0.5f,0.5f,0,1,0,
                          -0.5f,0.5f,-0.5f,0,1,0,
                          -0.5f,0.5f,0.5f,0,1,0,

                          -0.5f,-0.5f,-0.5f,0,-1,0,
                          0.5f,-0.5f,-0.5f,0,-1,0,
                          0.5f,-0.5f,0.5,0,-1,0,
                          -0.5f,-0.5f,0.5,0,-1,0,
                          -0.5f,-0.5f,-0.5,0,-1,0,
                          0.5f,-0.5f,0.5f,0,-1,0
                          };
    mesh_data.materials.emplace_back("",0);
    mesh_data.materials[0].vertices =mesh_data.vertices.size()/6;
    mesh_data.draw_type = Engine::Draw_type::STREAM;
    mesh_data.m_vertex_buffer = Engine::Vertex_buffer::Create(
            &mesh_data.vertices[0],mesh_data.vertices.size()*sizeof(mesh_data.vertices[0]),
            Engine::Raw_buffer::Access_frequency::STATIC,
            Engine::Raw_buffer::Access_type::DRAW);



    int instances = 5000;

    /// Create container for all the green cubes
    auto green_material = std::make_shared<Lab3::Monochrome_material>(shader_program, glm::vec4(0.4f, 0.8f, 0.3f, 1.0f));
    auto green_model = Engine::Instanced_mesh({{Engine::Shader_data::Mat4, "transform"}}, instances);
    green_model.add_LOD(mesh_data, green_material);

    /// container for the pink cube
    auto pink_material = std::make_shared<Lab3::Monochrome_material>(shader_program, glm::vec4(1, 114/255.0f, 166/255.0f, 1.0f));
    auto pink_model = std::shared_ptr<Engine::Instanced_mesh>(new Engine::Instanced_mesh({{Engine::Shader_data::Mat4, "transform"}}, 1));
    pink_model->add_LOD(mesh_data,pink_material);

    /// Add instances of green cubes at random positions
    std::srand(time(nullptr));
    for (int i = 0; i < instances; i++) {
        float x= rand()%100-50;
        float y= rand()%100-50;
        float z= rand()%100-50;
        auto matrix = glm::translate(glm::mat4(1), glm::vec3(x,y,z));
        green_model.add_instance((float*)&matrix, {x, y, z}, 1);
    }

    auto mat = glm::mat4(1);
    auto instance = Lab3::GraphicsNode(pink_model,mat); /// Pink cube graphics node
    pink_model->on_update();
    green_model.on_update();


    Engine::Event_actor_obj<Engine::Keyboard_key_Event> mouse_listener;
    mouse_listener.set_callback_func<Engine::Keyboard_key_Event>([&instance](Engine::Keyboard_key_Event event){
        glm::vec3 position = glm::vec3(0,0,0);
        if (event.keycode == CL_KEY_W) {
            position.y += 0.1f;
        } else if (event.keycode == CL_KEY_A) {
            position.x -= 0.1f;
        } else if (event.keycode == CL_KEY_D) {
            position.x += 0.1f;
        } else if (event.keycode == CL_KEY_S) {
            position.y -= 0.1f;
        } else if  (event.keycode == CL_KEY_E) {
            position.z -= 0.1f;
        } else if (event.keycode == CL_KEY_C) {
            position.z += 0.1f;
        }
        instance.transform(position);
        instance.on_update();
    });

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    Engine::Render::Render_command::set_clear_color({134/255.0f,198/255.0f,247/255.0f,1});
    while(!window->should_close()) {
        Engine::Render::Renderer::begin_scene();
        mouse_listener.on_update();

        /// disabling frustum updates will lock all visible blocks, but the camera can still move.
        /*if (panel->update_frustum()) {
            pink_model->on_update();
            green_model.on_update();
        }*/
        green_model.on_render();
        pink_model->on_render();
        gui.on_update();
        panel->set_objects(Engine::Render::Renderer::get_debug()->rendered_objects);

        /// Update camera position
        auto matrix = glm::rotate(glm::mat4(1),panel->pitch(), glm::vec3(1,0,0));
        glm::vec4 position = matrix*glm::vec4(0,0,panel->offset(),1);
        matrix = glm::rotate(glm::mat4(1),panel->yaw(), glm::vec3(0,1,0));
        position = matrix*position;
        camera.set_position_axis_aligned(position);
        camera.set_FOV(panel->fov()/180.0f*PI);
        camera.on_update();
        uniforms.projection_matrix.m_data = camera.get_projection_matrix();
        uniforms.view_matrix.m_data = camera.get_view_matrix();
        uniforms.update_projection_matrix();
        uniforms.update_view_matrix();

        window->on_update();
    }


   return 0;
}
