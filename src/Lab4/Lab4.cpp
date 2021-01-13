#define API_OPENGL
#include <Engine.h>
#include <algorithm>
#include "Phong_material.h"
#include "Scene.h"
#include "GraphicsNode.h"
#include "Point_light.h"
#include "Geometry.h"
#include "Phong_material.h"

#define PI (atan(1)*4)
BEGIN_SHADER_CONST(Global_uniforms)
    SET_SHADER_CONST(Engine::Matrix4_data, viewprojection_matrix,false)
    SET_SHADER_CONST(Engine::Float3_data, viewport,false)
    SET_SHADER_CONST(Engine::Int2_data, viewport_size, false);
    SET_SHADER_CONST(Engine::Float3_data, ambient,false)
    SET_SHADER_CONST(Engine::Float3_data, diffuse,false)
    SET_SHADER_CONST(Engine::Float3_data, specular,false)
    SET_SHADER_CONST(Engine::Float4_data, sun_position,true)
END_SHADER_CONST(Global_uniforms)

class Camera_controller : public Engine::Perspective_camera, public Engine::Event_actor_st<Engine::Keyboard_key_Event, Engine::Mouse_cursor_Event> {
public:
    Camera_controller(float znear, float zfar, float fov, float aspectRatio, const glm::vec3 &upvector=glm::vec3(0,1,0))
            : Perspective_camera(znear, zfar, fov, aspectRatio, upvector) {
        this->set_callback<Engine::Mouse_cursor_Event>([this](Engine::Mouse_cursor_Event event){
            float delta_x = event.xpos-old_x;
            float delta_y = event.ypos-old_y;
            old_x = event.xpos;
            old_y = event.ypos;

            if (!this->active_camera) return;

            this->pitch -= delta_x*0.0012f;
            this->yaw -= delta_y*0.0012f;
        });

        this->set_callback<Engine::Keyboard_key_Event>([this](Engine::Keyboard_key_Event event) {
            bool action = (bool) std::min(event.action, 1);
           if (event.action == 1 && event.keycode == CL_KEY_ESCAPE) {
               if (!this->active_camera) {
                   Engine::Event_system::post<Engine::Disable_cursor_event>();
               } else {
                   Engine::Event_system::post<Engine::Show_cursor_event>();
               }
               this->active_camera = !this->active_camera;
               return;
           }

           if (event.keycode == CL_KEY_A && key_states[2] != action) {
               this->movement.x -= action != 0 ? 1 : -1;
               key_states[2] = action;
           } else if (event.keycode == CL_KEY_W && key_states[0] != action) {
               this->movement.z -= action != 0 ? 1 : -1;
               key_states[0] = action;
           } else if (event.keycode == CL_KEY_S && key_states[1] != action) {
               this->movement.z += action != 0 ? 1 : -1;
               key_states[1] = action;
           } else if (event.keycode == CL_KEY_D && key_states[3] != action) {
               this->movement.x += action != 0 ? 1 : -1;
               key_states[3] = action;
           }

           /// Clamp the transform speed to [-1, 1]
           if (this->movement.x != 0) {
               this->movement.x /= std::abs(this->movement.x);
           }
           if (this->movement.z != 0) {
               this->movement.z /= std::abs(this->movement.z);
           }
        });
    }

    void on_update() override {
        this->move_view_aligned(this->movement*0.05f);
        this->set_rotation(this->pitch, this->yaw, 0.0f);
        Perspective_camera::on_update();
        Event_actor_st::on_update();

    }

private:
    float pitch = 0, yaw = 0;
    float old_x = 0, old_y = 0;
    bool active_camera = false;
    glm::vec3 movement = glm::vec3(0,0,0);
    bool key_states[4] = {0,0,0,0};   // W, S, A, D
};


class Robot_controller : public Engine::ImGui_panel {
public:
    Robot_controller(glm::vec3 position) : m_position(position){


        /// Create raw mesh source
        Engine::Import_mesh mesh_data;
        Lab4::create_prism(mesh_data.vertices, 2, 1, 32);
        mesh_data.materials.emplace_back("", 0);
        mesh_data.materials[0].vertices = mesh_data.vertices.size() / 6;
        mesh_data.draw_type = Engine::Draw_type::STREAM;
        mesh_data.m_vertex_buffer = Engine::Vertex_buffer::Create(
                &mesh_data.vertices[0], mesh_data.vertices.size() * sizeof(mesh_data.vertices[0]),
                Engine::Raw_buffer::Access_frequency::STATIC,
                Engine::Raw_buffer::Access_type::DRAW);


        this->m_mesh = std::shared_ptr<Engine::Instanced_mesh>(new Engine::Instanced_mesh({
                                                                                                  {Engine::Shader_data::Float4, "color"},
                                                                                                  {Engine::Shader_data::Float,  "specular_power"},
                                                                                                  {Engine::Shader_data::Float,  "shininess"},
                                                                                                  {Engine::Shader_data::Mat4,   "transform"}},
                                                                                          1));

        this->m_mesh->add_LOD(mesh_data, Lab4::Phong_material::Create());

        /// ------------------       Create robot limbs       ------------------
        {
            auto mat = glm::translate(glm::mat4(1), position);
            std::vector<float> data = {0.1f, 0.6f, 1.0f, 1, 128,1};
            auto node = std::make_shared<Lab4::GeometryNode>(this->m_mesh, &data[0], mat);
            this->m_limbs.push_back(node);
        }
        this->m_rotations.resize(4);
        for (int i = 0; i < 3; ++i) {
            auto mat = glm::translate(glm::mat4(1), glm::vec3(0,2.0f,0));
            std::vector<float> data = {0.3f+(float)i/3.0f, 1.0f, 1.0f, 1,128,1};
            auto node = std::make_shared<Lab4::GeometryNode>(this->m_mesh, &data[0], mat);
            this->m_limbs.back()->add_node(node);
            this->m_limbs.push_back(node);
        }
        {
            auto mat = glm::translate(glm::mat4(1), glm::vec3(0.3f,2.0f,0));
            mat = glm::scale(mat, glm::vec3(0.4f,0.4f,0.4f));
            std::vector<float> data = {0.2f, 0.2f, 1.0f, 1, 0,0};
            this->m_grabber_A = std::make_shared<Lab4::GeometryNode>(this->m_mesh,&data[0], mat);
            mat = glm::translate(glm::mat4(1), glm::vec3(-0.3f,2.0f,0));
            mat = glm::scale(mat, glm::vec3(0.4f,0.4f,0.4f));
            this->m_grabber_B = std::make_shared<Lab4::GeometryNode>(this->m_mesh, &data[0], mat);
            this->m_limbs.back()->add_node(m_grabber_B);
            this->m_limbs.back()->add_node(m_grabber_A);
        }
    }

    void on_imgui_render() override {
        ImGui_panel::on_imgui_render();

        ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->GetWorkPos().x + 20, main_viewport->GetWorkPos().y + 500), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(550, 200), ImGuiCond_FirstUseEver);
        ImGui::Begin("Test panel");
        ImGui::SetWindowFontScale(1.2f);
        ImGui::Text("Press [ESC] to toggle camera control\nControls: [W][A][S][D] to move");  ///
        ImGui::SliderFloat("Y rotation", &this->m_absolute_rotation, -180, 180);
        ImGui::SliderFloat("Grab ", &this->m_grabber_rotation, 0, 30);
        for (int i= 1; i < this->m_rotations.size(); ++i) {
            std::string s = "X rotation node ";
            s += std::to_string(i);
            ImGui::SliderFloat(s.c_str(), &this->m_rotations[i], 0, 80);
        }


        ImGui::End();

        {
            glm::vec3 euler_angles(glm::radians(this->m_rotations[0]), glm::radians(this->m_absolute_rotation), 0);
            auto quatern = glm::quat(euler_angles);
            this->m_limbs[0]->reset_transform();
            this->m_limbs[0]->translate(this->m_position);
            this->m_limbs[0]->rotate(quatern);

            this->m_limbs[0]->translate(glm::vec3(0,1,0));
        }
        {
            this->m_grabber_B->reset_transform();
            this->m_grabber_A->reset_transform();

            float angle = glm::radians(this->m_grabber_rotation);
            this->m_grabber_A->translate(glm::vec3(0.3f,1.0f,0));
            this->m_grabber_A->scale(glm::vec3(0.4f,0.4f,0.4f));
            this->m_grabber_A->rotate(angle, glm::vec3(0,0,1));
            this->m_grabber_A->translate(glm::vec3(0,1.0f,0));

            this->m_grabber_B->translate(glm::vec3(-0.3f,1.0f,0));
            this->m_grabber_B->scale(glm::vec3(0.4f,0.4f,0.4f));
            this->m_grabber_B->rotate(-angle, glm::vec3(0,0,1));
            this->m_grabber_B->translate(glm::vec3(0,1.0f,0));
        }

        for (int i = 1; i < this->m_limbs.size(); ++i) {
            this->m_limbs[i]->reset_transform();

            glm::vec3 euler_angles(glm::radians(this->m_rotations[i]),0,0);
            auto quatern = glm::quat(euler_angles);
            this->m_limbs[i]->translate(glm::vec3(0,1.0f,0));
            this->m_limbs[i]->rotate(quatern);
            this->m_limbs[i]->translate(glm::vec3(0,1.0f,0));
        }
    }

    std::shared_ptr<Engine::Instanced_mesh> get_mesh() {
        return this->m_mesh;
    }

    std::shared_ptr<Lab4::InnerNode> get_node() {
        return this->m_limbs.front();
    }

private:
    std::shared_ptr<Engine::Instanced_mesh> m_mesh;
    //std::shared_ptr<Lab4::GeometryNode> m_arm;
    std::vector<std::shared_ptr<Lab4::GeometryNode>> m_limbs;
    std::shared_ptr<Lab4::GeometryNode> m_grabber_A, m_grabber_B;
    Engine::ImGui_panel m_panel;
    std::vector<float> m_rotations = {30,30,30,30};
    float m_absolute_rotation = 0;
    float m_grabber_rotation = 0;
    glm::vec3 m_position;
};


struct Floor {
public:
    Floor(glm::vec3 position) {
        /// -------- Shaders --------
        auto vertex = Engine::Shader::create_shader_stage(
                GL_VERTEX_SHADER,
                R"(
            #version 450 core
            layout(location = 0) in vec3 position;
            layout(location = 1) in vec3 normal;
            layout(location = 2) in vec4 instance_color;
            layout(location = 3) in vec4 alternate_color;
            layout(location = 4) in mat4 transform;


            //layout(location = 0) out vec4 vs_color;
            layout(location = 0) out vec3 vs_normal;
            layout(location = 1) out vec3 vs_position;
            layout(location = 2) out vec4 vs_color;


            layout(std140) uniform Uniform_block {
                mat4 view_matrix;
                //mat4 projection_matrix;
                vec3 viewport;
                ivec2 view_size;
                vec3 ambient;
                vec3 diffuse;
                vec3 specular;
                vec4 sun;
            } my_block;

            void main() {
                gl_Position = my_block.view_matrix*transform*vec4(position,1);
                vs_position = (transform*vec4(position,1)).xyz;
                vs_normal = normalize((transform*vec4(normal,0)).xyz);

                // Hotfix for getting row and square numbering.
                int triangle_id = (gl_VertexID-gl_VertexID%6)%4;
                int row_id = ((gl_VertexID-gl_VertexID%(6*8)))%32;

                if (triangle_id > 0) {
                    vs_color = mix(alternate_color,instance_color, row_id);
                } else {
                    vs_color = mix(instance_color,alternate_color, row_id);
                }
                //vs_color *= row_id / 16.0;
                //vs_color = mix(instance_color, alternate_color, triangle_id%2);

            }

            )");
        auto fragment = Engine::Shader::create_shader_stage(
                GL_FRAGMENT_SHADER,
                R"(
            #version 450 core
            layout(location = 0) in vec3 vs_normal;
            layout(location = 1) in vec3 world_pos;
            layout(location = 2) in vec4 vs_color;


            layout(location = 0) out vec4 diffuse_color;
            layout(location = 1) out float specular_power;
            layout(location = 2) out float shininess;
            layout(location = 3) out vec3 fragment_normal;
            layout(location = 4) out vec3 world_position;


            layout(std140) uniform Uniform_block {
                mat4 view_matrix;
                //mat4 projection_matrix;
                vec3 viewport;
                ivec2 view_size;
                vec3 ambient;
                vec3 diffuse;
                vec3 specular;
                vec4 sun;
            } my_block;

            //layout(location = 1) uniform vec4 ambient_color;

            void main() {
                diffuse_color = vs_color;
                specular_power = 1;
                fragment_normal = normalize(vs_normal);
                world_position = world_pos;
                shininess = 0;
            }
            )");

        auto shader_program = Engine::Shader::create_shader({vertex, fragment});
        shader_program->compile();

        /// The uniform bindings must be done manually.
        GLuint index= glGetUniformBlockIndex(shader_program->get_ID(), "Uniform_block");
        glUniformBlockBinding(shader_program->get_ID(), index, 0);


        /// Create raw mesh source
        Engine::Import_mesh mesh_data;
        //Lab4::create_sphere(mesh_data.vertices,64);
        Lab4::create_floor(mesh_data.vertices, 8, 8, 8);
        mesh_data.materials.emplace_back("", 0);
        mesh_data.materials[0].vertices = mesh_data.vertices.size() / 6;
        mesh_data.draw_type = Engine::Draw_type::STREAM;
        mesh_data.m_vertex_buffer = Engine::Vertex_buffer::Create(
                &mesh_data.vertices[0], mesh_data.vertices.size() * sizeof(mesh_data.vertices[0]),
                Engine::Raw_buffer::Access_frequency::STATIC,
                Engine::Raw_buffer::Access_type::DRAW);

        /// container for the pink cube
        auto my_material = std::make_shared<Engine::Material>(shader_program);
        this->m_mesh = std::shared_ptr<Engine::Instanced_mesh>(new Engine::Instanced_mesh({
                                                                                             {Engine::Shader_data::Float4, "colorA"},
                                                                                             {Engine::Shader_data::Float4, "colorB"},
                                                                                             {Engine::Shader_data::Mat4,   "transform"}},
                                                                                     1));
        this->m_mesh->add_LOD(mesh_data, my_material);

        auto mat = glm::translate(glm::mat4(1), position);
        std::vector<float> data = {0.4f, 0.4f, 0.4f, 1, 1, 1, 1, 1};

        this->m_node = std::make_shared<Lab4::GeometryNode>(this->m_mesh, &data[0], mat);

    }

    std::shared_ptr<Lab4::InnerNode> get_node() {
        return this->m_node;
    }

    std::shared_ptr<Engine::Instanced_mesh> get_mesh() {
        return this->m_mesh;
    }

private:
    std::shared_ptr<Engine::Instanced_mesh> m_mesh;
    std::shared_ptr<Lab4::GeometryNode> m_node;
};

int main(int argc, char** argv) {
    auto window = std::shared_ptr<Engine::Window>(Engine::Window::create_window());
    Global_uniforms uniforms;
    uniforms.viewport_size.m_data = glm::ivec2(window->get_width(),window->get_height());
    uniforms.update_viewport_size();

    uniforms.ambient.m_data = glm::fvec3(0.3,0.3,0.3);
    uniforms.diffuse.m_data = glm::fvec3(0.7,0.7,0.7);
    uniforms.sun_position.m_data = glm::fvec4 (0,0,0,1.0f);
    uniforms.specular.m_data = glm::fvec3(1.0f,1.0f,1.0f);
    uniforms.update_ambient();
    uniforms.update_diffuse();
    uniforms.update_specular();
    uniforms.update_sun_position();

    auto gui = Engine::ImGui_layer(window);

    /// ------------------       Camera       ------------------
    float aspect_ratio = ((float)window->get_width()/window->get_height());
    auto camera = Camera_controller(0.01f,1000.0f,glm::radians(60.0f),aspect_ratio);
    camera.set_position_axis_aligned({0.0f,1.0f,5.0f});


    /// ------------------ Create scene graph ------------------
    auto scene = std::make_shared<Lab4::InnerNode>(glm::mat4(1));
    std::vector<std::shared_ptr<Engine::Instanced_mesh>> meshes;
    Floor floor = Floor(glm::vec3(-4, 0, -4));
    meshes.push_back(floor.get_mesh());
    scene->add_node(floor.get_node());

    /// ------------------ Create robot arm ------------------
    auto arm = std::make_shared<Robot_controller>(glm::vec3(4, 0, -1));
    meshes.push_back(arm->get_mesh());
    gui.push_panel(arm);
    floor.get_node()->add_node(arm->get_node());



    /// ------------------ Create static meshes ------------------
    auto global_phong_material = Lab4::Phong_material::Create();
    {
        /// ------------------      Sphere      ------------------
        Engine::Import_mesh sphere_data;
        //Lab4::create_sphere(mesh_data.vertices,64);
        Lab4::create_sphere(sphere_data.vertices, 32);
        sphere_data.materials.emplace_back("", 0);
        sphere_data.materials[0].vertices = sphere_data.vertices.size() / 6;
        sphere_data.draw_type = Engine::Draw_type::STREAM;
        sphere_data.m_vertex_buffer = Engine::Vertex_buffer::Create(
                &sphere_data.vertices[0], sphere_data.vertices.size() * sizeof(sphere_data.vertices[0]),
                Engine::Raw_buffer::Access_frequency::STATIC,
                Engine::Raw_buffer::Access_type::DRAW);

        auto sphere_mesh = std::shared_ptr<Engine::Instanced_mesh>(new Engine::Instanced_mesh({
                                                                                                 {Engine::Shader_data::Float4, "color"},
                                                                                                 {Engine::Shader_data::Float,  "specular_power"},
                                                                                                 {Engine::Shader_data::Float,  "shininess"},
                                                                                                 {Engine::Shader_data::Mat4,   "transform"}},
                                                                                              8));
        sphere_mesh->add_LOD(sphere_data, global_phong_material);
        meshes.push_back(sphere_mesh);
        {
            for (float x = 0.5f; x < 8.0f; x += 1.0f) {
                float data[6] = {1.0f, 0.7f, 0.4f, 1.0f, 128.0f,std::pow(x/8.0f,2)};
                auto mat = glm::translate(glm::mat4(1), glm::vec3(x,3.0f,4.5f));
                mat = glm::scale(mat, glm::vec3(0.3f,0.3f,0.3f));
                auto instance = std::make_shared<Lab4::GeometryNode>(sphere_mesh, data, mat);
                floor.get_node()->add_node(instance);
            }
        }


        /// ------------------   Cone/pyramid    ------------------
        Engine::Import_mesh cone_data;
        {
            Lab4::create_cone(cone_data.vertices, 1.5f, 1, 8);
            cone_data.materials.emplace_back("", 0);
            cone_data.materials[0].vertices = cone_data.vertices.size() / 6;
            cone_data.draw_type = Engine::Draw_type::STREAM;
            cone_data.m_vertex_buffer = Engine::Vertex_buffer::Create(
                    &cone_data.vertices[0], cone_data.vertices.size() * sizeof(cone_data.vertices[0]),
                    Engine::Raw_buffer::Access_frequency::STATIC,
                    Engine::Raw_buffer::Access_type::DRAW);
        }

        auto cone_mesh = std::shared_ptr<Engine::Instanced_mesh>(new Engine::Instanced_mesh({
                                                                                                      {Engine::Shader_data::Float4, "color"},
                                                                                                      {Engine::Shader_data::Float,  "specular_power"},
                                                                                                      {Engine::Shader_data::Float,  "shininess"},
                                                                                                      {Engine::Shader_data::Mat4,   "transform"}},
                                                                                              1));
        cone_mesh->add_LOD(cone_data, global_phong_material);
        meshes.push_back(cone_mesh);
        {
            float data[6] = {1.0f, 1.0f, 1.0f, 1.0f, 512.0f,1.0f};
            auto mat = glm::translate(glm::mat4(1), glm::vec3(4.5f,0.0f,0.5f));
            auto instance = std::make_shared<Lab4::GeometryNode>(cone_mesh, data, mat);
            floor.get_node()->add_node(instance);
        }

        /// ------------------      Cube     ------------------
        Engine::Import_mesh cube_data;
        {
            Lab4::create_cube(cube_data.vertices,0.8f);
            cube_data.materials.emplace_back("", 0);
            cube_data.materials[0].vertices = cube_data.vertices.size() / 6;
            cube_data.draw_type = Engine::Draw_type::STREAM;
            cube_data.m_vertex_buffer = Engine::Vertex_buffer::Create(
                    &cube_data.vertices[0], cube_data.vertices.size() * sizeof(cube_data.vertices[0]),
                    Engine::Raw_buffer::Access_frequency::STATIC,
                    Engine::Raw_buffer::Access_type::DRAW);
        }
        auto cube_mesh = std::shared_ptr<Engine::Instanced_mesh>(new Engine::Instanced_mesh({
                                                                                                    {Engine::Shader_data::Float4, "color"},
                                                                                                    {Engine::Shader_data::Float,  "specular_power"},
                                                                                                    {Engine::Shader_data::Float,  "shininess"},
                                                                                                    {Engine::Shader_data::Mat4,   "transform"}},
                                                                                            1));
        cube_mesh->add_LOD(cube_data,global_phong_material);
        meshes.push_back(cube_mesh);
        {
            float data[6] = {1.0f, 1.0f, 1.0f, 1.0f, 512.0f,1.0f};
            auto mat = glm::translate(glm::mat4(1), glm::vec3(3.5f,0.4f,0.5f));
            auto instance = std::make_shared<Lab4::GeometryNode>(cube_mesh, data, mat);
            floor.get_node()->add_node(instance);
        }

        /// ------------------     Prism    ------------------
        Engine::Import_mesh prism_data;
        //Lab4::create_sphere(mesh_data.vertices,64);
        Lab4::create_prism(prism_data.vertices, 0.5f, 0.4f, 32);
        prism_data.materials.emplace_back("", 0);
        prism_data.materials[0].vertices = prism_data.vertices.size() / 6;
        prism_data.draw_type = Engine::Draw_type::STREAM;
        prism_data.m_vertex_buffer = Engine::Vertex_buffer::Create(
                &prism_data.vertices[0], prism_data.vertices.size() * sizeof(prism_data.vertices[0]),
                Engine::Raw_buffer::Access_frequency::STATIC,
                Engine::Raw_buffer::Access_type::DRAW);


        auto prism_mesh = std::shared_ptr<Engine::Instanced_mesh>(new Engine::Instanced_mesh({
                                                                                                  {Engine::Shader_data::Float4, "color"},
                                                                                                  {Engine::Shader_data::Float,  "specular_power"},
                                                                                                  {Engine::Shader_data::Float,  "shininess"},
                                                                                                  {Engine::Shader_data::Mat4,   "transform"}},
                                                                                          1));
        prism_mesh->add_LOD(prism_data, global_phong_material);
        meshes.push_back(prism_mesh);
        {
            for (float x = 0.5f; x < 8.0f; ++x) {
                float data[6] = {1.0f, 1.0f, 1.0f, 1.0f, 4.0f,0.5f};
                auto mat = glm::translate(glm::mat4(1), glm::vec3(x,0.25f,1.5f));
                float scale = x/10.0f;
                mat = glm::scale(mat, glm::vec3(scale,scale,scale));
                auto instance = std::make_shared<Lab4::GeometryNode>(prism_mesh, data, mat);
                floor.get_node()->add_node(instance);
            }
        }
    }




    /// ------------------      Temporary code for an ambient light source     ------------------
    auto light_vertex = Engine::Shader::create_shader_stage(GL_VERTEX_SHADER, R"(
        #version 450 core
        layout(location = 0) in vec2 position;
        layout(location = 0) noperspective out vec2 tex_coords;

        void main() {
            gl_Position = vec4(position.x, position.y,-1,1);
            tex_coords = position.xy*0.5+0.5;
            //tex_coords = vec2(1,1);
        }
    )");
    auto light_fragment = Engine::Shader::create_shader_stage(GL_FRAGMENT_SHADER, R"(
        #version 450 core
        layout(location = 0) noperspective in vec2 tex_coords;
        layout(location = 0) out vec4 frag_color;


        layout(location = 0) uniform sampler2D diffuse_texture;
        layout(location = 1) uniform sampler2D specular_texture;
        layout(location = 2) uniform sampler2D shininess_texture;
        layout(location = 3) uniform sampler2D normal_texture;
        layout(location = 4) uniform sampler2D frag_coord_texture;

        layout(std140) uniform Uniform_block {
            mat4 view_matrix;
            //mat4 projection_matrix;
            vec3 viewport;
            ivec2 view_size;
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            vec4 sun;
        } my_block;


        void main() {
            vec4 diffuse = texture(diffuse_texture, tex_coords);
            vec3 normal = texture(normal_texture, tex_coords).xyz;
            float specular_power = texture(specular_texture, tex_coords).r;
            vec3 frag_position = texture(frag_coord_texture, tex_coords).xyz;

            frag_color = vec4( diffuse.rgb*0.1,diffuse.a);
        }
    )");
    auto light_shader = Engine::Shader::create_shader({light_vertex, light_fragment});
    light_shader->compile();
    auto index= glGetUniformBlockIndex(light_shader->get_ID(), "Uniform_block");
    glUniformBlockBinding(light_shader->get_ID(), index, 0);

    float light_data[] = {
            -1,-1,
            1,-1,
            -1,1,
            1,-1,
            1,1,
            -1,1
    };
    auto light_vbo = Engine::Vertex_buffer::Create(&light_data, sizeof(light_data), Engine::Raw_buffer::Access_frequency::STATIC, Engine::Raw_buffer::Access_type::DRAW);
    light_vbo->set_layout(Engine::Buffer_layout{{Engine::Shader_data::Float2, "position"}});
    auto light_vao = Engine::Vertex_array::Create();
    light_vao->add_vertex_buffer(light_vbo);

    /// ------      Framebuffer for off-screen rendering     -------
    auto framebuffer = Engine::FrameBuffer::Create();
    auto diffuse_texture = Engine::Texture2D::Create_empty(window->get_width(), window->get_height(), GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE);
    auto specular_texture = Engine::Texture2D::Create_empty(window->get_width(), window->get_height(), GL_R32F, GL_RED,  GL_FLOAT);
    auto shinines_texture = Engine::Texture2D::Create_empty(window->get_width(), window->get_height(), GL_R32F, GL_RED, GL_FLOAT);
    auto normal_texture = Engine::Texture2D::Create_empty(window->get_width(),window->get_height(),GL_RGB32F,GL_RGB, GL_FLOAT);
    auto world_pos_texture = Engine::Texture2D::Create_empty(window->get_width(),window->get_height(),GL_RGB32F,GL_RGB, GL_FLOAT);
    auto depth_texture = Engine::Texture2D::Create_empty(window->get_width(), window->get_height(), GL_DEPTH24_STENCIL8,GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);

    framebuffer->add_write_color_texture(diffuse_texture);
    framebuffer->add_write_color_texture(specular_texture);
    framebuffer->add_write_color_texture(shinines_texture);
    framebuffer->add_write_color_texture(normal_texture);
    framebuffer->add_write_color_texture(world_pos_texture);
    framebuffer->set_depth_texture(depth_texture);

    /// ------------------      Point lights     ------------------
    auto pointlight = Lab4::GL_Point_light::Create(80);
    {
        auto light_instance = std::make_shared<Lab4::LightNode>(pointlight,glm::vec3(1.0f,1.0f,1.0f),glm::vec4(4.0f,9.0f,4.0f,1.0f),13.0f,0.5f);
        floor.get_node()->add_node(light_instance);
    }
    {
        std::srand(time(nullptr));
        for (int i = 0; i < 50; i++) {
            float x = (rand()%80)/10.0f;
            float z = (rand()%80)/10.0f;
            auto light_instance = std::make_shared<Lab4::LightNode>(pointlight,glm::vec3(1.0f,1.0f,1.0f),glm::vec4(x,0.3f,z,1),0.75f,0.75);
            floor.get_node()->add_node(light_instance);
        }
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glLineWidth(3);
    glPointSize(3);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    Engine::Render::Render_command::set_clear_color({134/255.0f,198/255.0f,247/255.0f,1});
    float angle = 0;
    while(!window->should_close()) {

        /// Update camera position
        camera.on_update();

        uniforms.viewprojection_matrix.m_data = camera.get_view_projection_matrix();
        uniforms.update_viewprojection_matrix();
        uniforms.viewport.m_data = camera.get_position();
        uniforms.update_viewport();

        //angle += 0.007;    // Remove the comment to make the world spin :)
        scene->on_update(glm::translate(glm::rotate(glm::mat4(1),angle*0.5f,glm::vec3(0,1,0)),glm::vec3(0,0,0)));

        /// Render to custom framebuffer

        framebuffer->bind_write();
        Engine::Render::Render_command::set_clear_color({0,0,0,0});
        Engine::Render::Renderer::begin_scene();
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        for (const auto& mesh : meshes) {
            mesh->on_render();
        }

        /// Second render pass to default window. This is where the "light" will shine on everything
        /// and make it visible.
        framebuffer->bind_read();
        light_shader->bind();


        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        Engine::Render::Render_command::set_clear_color({134/255.0f,198/255.0f,247/255.0f,1});
        Engine::Render::Renderer::begin_scene();

        /// Super simple ambient light source. This does a light calculation on every pixel.
        light_vao->bind();
        glDrawArrays(GL_TRIANGLES,0,6);
        light_vao->unbind();

        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        pointlight->on_render();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        gui.on_update();
        window->on_update();
    }
}