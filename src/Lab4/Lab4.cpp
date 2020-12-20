#define API_OPENGL
#include <Engine.h>
#include "Monochrome_material.h"
#include "Scene.h"
#include "GraphicsNode.h"
#include "Shadowmap_material.h"

#define PI (atan(1)*4)
BEGIN_SHADER_CONST(Global_uniforms)
    SET_SHADER_CONST(Engine::Matrix4_data, view_matrix,false)
    SET_SHADER_CONST(Engine::Matrix4_data, projection_matrix,false)
    SET_SHADER_CONST(Engine::Float3_data, viewport,false)
    SET_SHADER_CONST(Engine::Float3_data, ambient,false)
    SET_SHADER_CONST(Engine::Float3_data, diffuse,false)
    SET_SHADER_CONST(Engine::Float3_data, specular,false)
    SET_SHADER_CONST(Engine::Float3_data, sun_position,true)
    SET_SHADER_CONST(Engine::Matrix4_data, shadow_camera, false);
END_SHADER_CONST(Global_uniforms)

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
        ImGui::SliderFloat("Camera center offset", &this->m_distance, 2,20);
        ImGui::SliderFloat("Camera FOV", &this->m_fov, 10, 90);
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
    int objects = 0;
    float m_fov = 45;
};

struct Vertex {
    float x,y,z;
    Vertex(float x, float y, float z) : x(x),y(y),z(z) {}
};

void sphere_helper(std::vector<float>& vertices, float phi, float z, bool up, float angle) {
    float phir = phi/180.0f*PI;
    for (float theta = -180; theta <= 180; theta += angle) {
        float thetar = theta/180.0f*PI;
        float theta20 = (theta+angle)/180.0f*PI;

        glm::vec3 a(sin(thetar)*cos(phir),cos(thetar)*cos(phir),sin(phir));
        glm::vec3 c(sin(theta20)*cos(phir),cos(theta20)*cos(phir),sin(phir));


        vertices.push_back(a.x);
        vertices.push_back(a.y);
        vertices.push_back(a.z);
        vertices.push_back(a.x);
        vertices.push_back(a.y);
        vertices.push_back(a.z);

        if (up) {
            //vertices.emplace_back(0,0,z);
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(z);
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(z);
        }

        vertices.push_back(c.x);
        vertices.push_back(c.y);
        vertices.push_back(c.z);
        vertices.push_back(c.x);
        vertices.push_back(c.y);
        vertices.push_back(c.z);

        if (!up) {
            //vertices.emplace_back(0,0,z);
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(z);
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(z);
        }

    }
}


void create_sphere(std::vector<float>& vertices, int segments) {
    float angle = 360.0f/segments;
    //vertices.emplace_back(0,0,-1);
    sphere_helper(vertices,-80,-1.0f,false,angle);
    for (float phi = -80; phi < 80; phi += angle) {
        float phir = phi/180.0f*PI;
        float phir20 = (phi+angle)/180.0f*PI;

        for (float theta = -180; theta <= 180; theta += angle) {
            float thetar = theta / 180.0f * PI;
            float theta20 = (theta + angle) / 180.0f * PI;

            glm::vec3 a(sin(thetar) * cos(phir), cos(thetar) * cos(phir), sin(phir));
            glm::vec3 b(sin(thetar) * cos(phir20), cos(thetar) * cos(phir20), sin(phir20));
            glm::vec3 c(sin(theta20) * cos(phir), cos(theta20) * cos(phir), sin(phir));
            glm::vec3 d(sin(theta20) * cos(phir20), cos(theta20) * cos(phir20), sin(phir20));
            glm::vec3 normal(0, 0, 0);


            vertices.push_back(a.x);
            vertices.push_back(a.y);
            vertices.push_back(a.z);
            vertices.push_back(a.x);
            vertices.push_back(a.y);
            vertices.push_back(a.z);

            vertices.push_back(b.x);
            vertices.push_back(b.y);
            vertices.push_back(b.z);
            vertices.push_back(b.x);
            vertices.push_back(b.y);
            vertices.push_back(b.z);

            vertices.push_back(c.x);
            vertices.push_back(c.y);
            vertices.push_back(c.z);
            vertices.push_back(c.x);
            vertices.push_back(c.y);
            vertices.push_back(c.z);

            vertices.push_back(b.x);
            vertices.push_back(b.y);
            vertices.push_back(b.z);
            vertices.push_back(b.x);
            vertices.push_back(b.y);
            vertices.push_back(b.z);

            vertices.push_back(d.x);
            vertices.push_back(d.y);
            vertices.push_back(d.z);
            vertices.push_back(d.x);
            vertices.push_back(d.y);
            vertices.push_back(d.z);

            vertices.push_back(c.x);
            vertices.push_back(c.y);
            vertices.push_back(c.z);
            vertices.push_back(c.x);
            vertices.push_back(c.y);
            vertices.push_back(c.z);
        }
    }
    sphere_helper(vertices,80,1.0f,true,angle);
}

void create_prism(std::vector<float>& vertices, float height, float width, int segments) {
    float angle = 2*PI/segments;
    float w = width/2;
    float h = height/2;
    for (float phi = -PI; phi < PI; phi += angle) {
        float phi2 = phi + angle;
        /// top
        vertices.push_back(0);
        vertices.push_back(h);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);
        vertices.push_back(0);

        vertices.push_back(w*sin(phi));
        vertices.push_back(h);
        vertices.push_back(w*cos(phi));
        vertices.push_back(0);
        vertices.push_back(1);
        vertices.push_back(0);

        vertices.push_back(w*sin(phi2));
        vertices.push_back(h);
        vertices.push_back(w*cos(phi2));
        vertices.push_back(0);
        vertices.push_back(1);
        vertices.push_back(0);

        /// circumference
        vertices.push_back(w*sin(phi));
        vertices.push_back(-h);
        vertices.push_back(w*cos(phi));
        vertices.push_back(sin(phi));   /// normal
        vertices.push_back(0);
        vertices.push_back(cos(phi));

        vertices.push_back(w*sin(phi2));
        vertices.push_back(-h);
        vertices.push_back(w*cos(phi2));
        vertices.push_back(sin(phi2));   /// normal
        vertices.push_back(0);
        vertices.push_back(cos(phi2));

        vertices.push_back(w*sin(phi));
        vertices.push_back(h);
        vertices.push_back(w*cos(phi));
        vertices.push_back(sin(phi));   /// normal
        vertices.push_back(0);
        vertices.push_back(cos(phi));



        vertices.push_back(w*sin(phi2));
        vertices.push_back(-h);
        vertices.push_back(w*cos(phi2));
        vertices.push_back(sin(phi2));   /// normal
        vertices.push_back(0);
        vertices.push_back(cos(phi2));

        vertices.push_back(w*sin(phi2));
        vertices.push_back(h);
        vertices.push_back(w*cos(phi2));
        vertices.push_back(sin(phi2));   /// normal
        vertices.push_back(0);
        vertices.push_back(cos(phi2));

        vertices.push_back(w*sin(phi));
        vertices.push_back(h);
        vertices.push_back(w*cos(phi));
        vertices.push_back(sin(phi));   /// normal
        vertices.push_back(0);
        vertices.push_back(cos(phi));

        /// bottom
        vertices.push_back(0);
        vertices.push_back(-h);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(-1);
        vertices.push_back(0);

        vertices.push_back(w*sin(phi2));
        vertices.push_back(-h);
        vertices.push_back(w*cos(phi2));
        vertices.push_back(0);
        vertices.push_back(-1);
        vertices.push_back(0);

        vertices.push_back(w*sin(phi));
        vertices.push_back(-h);
        vertices.push_back(w*cos(phi));
        vertices.push_back(0);
        vertices.push_back(-1);
        vertices.push_back(0);
    }
}

void create_pyramid(std::vector<float>& vertices, float height, float width, int segments) {
    float angle = 2*PI/segments;
    float w = width/2;
    glm::vec3 old_norm = glm::vec3();
    glm::vec3 norm = glm::vec3();
    glm::vec3 new_norm = glm::vec3();
    {
        glm::vec3 a = glm::vec3 (w*(sin(-PI)-sin(-PI-angle)), 0,w*(cos(-PI)-cos(-PI-angle)));
        glm::vec3 b = glm::vec3 (-a.x,height,-a.z);
        old_norm = glm::cross(a,b);

        a = glm::vec3 (w*(sin(-PI+angle)-sin(-PI)), 0,w*(cos(-PI+angle)-cos(-PI)));
        b = glm::vec3 (-a.x,height,-a.z);
        norm = glm::cross(a,b);

    }
    for (float phi = - PI; phi < PI; phi += angle) {
        float phi2 = phi + angle;
        float phi3 = phi2 + angle;
        /// Bottom
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(-1);
        vertices.push_back(0);

        vertices.push_back(w*sin(phi2));
        vertices.push_back(0);
        vertices.push_back(w*cos(phi2));
        vertices.push_back(0);
        vertices.push_back(-1);
        vertices.push_back(0);

        vertices.push_back(w*sin(phi));
        vertices.push_back(0);
        vertices.push_back(w*cos(phi));
        vertices.push_back(0);
        vertices.push_back(-1);
        vertices.push_back(0);


        /// Top
        glm::vec3 a = glm::vec3 (w*(sin(phi3)-sin(phi2)), 0,w*(cos(phi3)-cos(phi2)));
        glm::vec3 b = glm::vec3 (-a.x,height,-a.z);
        new_norm = glm::cross(a,b);

        vertices.push_back(w*sin(phi));
        vertices.push_back(0);
        vertices.push_back(w*cos(phi));
        glm::vec3 norm_left = (old_norm+norm)/2.0f;
        vertices.push_back(norm_left.x);
        vertices.push_back(norm_left.y);
        vertices.push_back(norm_left.z);

        vertices.push_back(w*sin(phi2));
        vertices.push_back(0);
        vertices.push_back(w*cos(phi2));
        glm::vec3 norm_right = (norm+new_norm)/2.0f;
        vertices.push_back(norm_right.x);
        vertices.push_back(norm_right.y);
        vertices.push_back(norm_right.z);

        vertices.push_back(0);
        vertices.push_back(height);
        vertices.push_back(0);
        vertices.push_back(0);
        vertices.push_back(1);
        vertices.push_back(0);

        old_norm = norm;
        norm = new_norm;
    }
}

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
            layout(location = 2) in vec4 instance_color;
            layout(location = 3) in float specular_power;
            layout(location = 4) in mat4 transform;


            //layout(location = 0) out vec4 vs_color;
            layout(location = 0) out vec3 vs_normal;
            layout(location = 1) out vec3 vs_position;
            layout(location = 2) out vec4 vs_color;
            layout(location = 3) out float vs_spec_power;


            layout(std140) uniform Uniform_block {
                mat4 view_matrix;
                mat4 projection_matrix;
                vec3 viewport;
                vec3 ambient;
                vec3 diffuse;
                vec3 specular;
                vec3 sun;
                mat4 shadow_camera;
            } my_block;

            void main() {
                gl_Position = my_block.projection_matrix * my_block.view_matrix*transform*vec4(position,1);
                vs_position = (transform*vec4(position,1)).xyz;
                vs_normal = normal;
                vs_color = instance_color;
                vs_spec_power = specular_power;

            }

            )");
    auto fragment = Engine::Shader::create_shader_stage(
            GL_FRAGMENT_SHADER,
            R"(
            #version 450 core
            layout(location = 0) in vec3 vs_normal;
            layout(location = 1) in vec3 world_pos;
            layout(location = 2) in vec4 vs_color;
            layout(location = 3) in float vs_spec_power;


            layout(location = 0) out vec4 diffuse_color;
            layout(location = 1) out float specular_power;
            layout(location = 2) out vec3 fragment_normal;
            layout(location = 3) out vec3 world_position;


            layout(std140) uniform Uniform_block {
                mat4 view_matrix;
                mat4 projection_matrix;
                vec3 viewport;
                vec3 ambient;
                vec3 diffuse;
                vec3 specular;
                vec3 sun;
                mat4 shadow_camera;
            } my_block;

            //layout(location = 1) uniform vec4 ambient_color;

            void main() {
                diffuse_color = vs_color;
                specular_power = vs_spec_power;
                fragment_normal = normalize(vs_normal);
                world_position = world_pos;
            }
            )");

    auto shader_program = Engine::Shader::create_shader({vertex, fragment});
    shader_program->compile();

    /// The uniform bindings must be done manually.
    GLuint index= glGetUniformBlockIndex(shader_program->get_ID(), "Uniform_block");
    glUniformBlockBinding(shader_program->get_ID(), index, 0);

    uniforms.ambient.m_data = glm::fvec3(0.3,0.3,0.3);
    uniforms.diffuse.m_data = glm::fvec3(0.7,0.7,0.7);
    uniforms.sun_position.m_data = glm::fvec3 (0.3f,1,0.3f);
    uniforms.specular.m_data = glm::fvec3(1.0f,1.0f,1.0f);
    //uniforms.sun_position.m_data /= glm::length(uniforms.sun_position.m_data);
    uniforms.update_ambient();
    uniforms.update_diffuse();
    uniforms.update_specular();
    uniforms.update_sun_position();

    /// -------- Camera --------
    float aspect_ratio = ((float)window->get_width()/window->get_height());
    //auto camera = Engine::Orthographic_camera(-aspect_ratio,aspect_ratio,1,-1,0,1);
    auto camera = Engine::Perspective_camera(0.01f,1000.0f,panel->fov(),aspect_ratio);
    camera.set_position_axis_aligned({0.0f,0.0f,10.0f});
    camera.look_at({0.0f,0.0f,0.0f});
    camera.on_update(); /// This is the only update needed.

    uniforms.view_matrix.m_data = camera.get_view_matrix();
    uniforms.projection_matrix.m_data = camera.get_projection_matrix();
    uniforms.update_view_matrix();
    uniforms.update_projection_matrix();


    /// Create raw mesh source
    Engine::Import_mesh mesh_data;
    create_pyramid(mesh_data.vertices,1,2,64);
    mesh_data.materials.emplace_back("",0);
    mesh_data.materials[0].vertices =mesh_data.vertices.size()/6;
    mesh_data.draw_type = Engine::Draw_type::STREAM;
    mesh_data.m_vertex_buffer = Engine::Vertex_buffer::Create(
            &mesh_data.vertices[0],mesh_data.vertices.size()*sizeof(mesh_data.vertices[0]),
            Engine::Raw_buffer::Access_frequency::STATIC,
            Engine::Raw_buffer::Access_type::DRAW);

    /// container for the pink cube
    auto my_material = std::make_shared<Engine::Material>(shader_program);
    auto sphere_model = std::shared_ptr<Engine::Instanced_mesh>(new Engine::Instanced_mesh({{Engine::Shader_data::Float4, "color"}, {Engine::Shader_data::Float, "specular_power"}, {Engine::Shader_data::Mat4, "transform"}}, 2));
    sphere_model->add_LOD(mesh_data, my_material);

    auto transform = glm::translate(glm::mat4(1),glm::vec3(-4.5f,0,-4.5f));
    auto scene = Lab4::InnerNode(transform);
    for (float x = 0; x <= 6; x+=2.0f) {
        for (float z = 0; z <= 9; z+=3.0f) {
            for (float y = 0; y <= 6; y+=2.0f) {
                auto mat = glm::translate(glm::mat4(1),glm::vec3(x,y,z));
                //auto color = glm::vec4(x/9.0f,0.5f+y/9.0f,z/9.0f,1);
                float data[] = {(x)/6.0f,(y)/6,(z)/9,1,6+x};
                auto instance = std::make_shared<Lab4::GeometryNode>(sphere_model, &data, mat);
                scene.add_node(instance);
            }

        }
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glLineWidth(3);
    glPointSize(3);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    Engine::Render::Render_command::set_clear_color({134/255.0f,198/255.0f,247/255.0f,1});
    auto light_vertex = Engine::Shader::create_shader_stage(GL_VERTEX_SHADER, R"(
        #version 450 core
        layout(location = 0) in vec2 position;
        out vec2 tex_coords;

        void main() {
            gl_Position = vec4(position.x, position.y,0,1);
            tex_coords = position.xy*0.5+0.5;
            //tex_coords = vec2(1,1);
        }
    )");

    auto light_fragment = Engine::Shader::create_shader_stage(GL_FRAGMENT_SHADER, R"(
        #version 450 core
        layout(location = 0) in vec2 tex_coords;
        layout(location = 0) out vec4 frag_color;


        layout(location = 0) uniform sampler2D diffuse_texture;
        layout(location = 1) uniform sampler2D specular_texture;
        layout(location = 2) uniform sampler2D normal_texture;
        layout(location = 3) uniform sampler2D frag_coord_texture;
        layout(location = 4) uniform sampler2D depth_texture;

        layout(std140) uniform Uniform_block {
            mat4 view_matrix;
            mat4 projection_matrix;
            vec3 viewport;
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            vec3 sun;
            mat4 shadow_camera;
        } my_block;

        uniform mat4 shadow_view_matrix;


        vec3 phong(vec3 color,vec3 fragPos, vec3 normal,vec3 viewport, vec3 light, vec3 ambient, vec3 diffuse, vec3 specular) {
                vec3 viewDir = normalize(viewport - fragPos);
                vec3 reflect = reflect(-light,normal);
                float spec_power = pow(max(dot(reflect,viewDir),0.0),32);


                float d = dot(normal,light);
                if (d > 0) {
                    vec3 specular_light = specular*spec_power;
                    vec3 diffuse_light = diffuse*d;
                    return color*(ambient+diffuse_light+ specular_light);
                } else {
                    return color*ambient;
                }


            }



        void main() {
            vec4 diffuse = texture(diffuse_texture, tex_coords);
            vec3 normal = texture(normal_texture, tex_coords).xyz;
            float specular_power = texture(specular_texture, tex_coords).r;
            vec3 frag_position = texture(frag_coord_texture, tex_coords).xyz;

            vec4 shadow_coords = my_block.projection_matrix * my_block.view_matrix*vec4(frag_position,1);
            float shadow_depth = texture(depth_texture, shadow_coords.xy*0.5+0.5).r;

            float depth = shadow_coords.z*0.5+0.5;

            float my_var = shadow_depth > depth ? 1 : 0;

            vec3 color = phong(diffuse.rgb,frag_position.xyz,normal,my_block.viewport,my_block.sun,my_block.ambient,my_block.diffuse*my_var,specular_power*my_block.specular*my_var);
/*
            if (shadow_depth > depth) {
                frag_color = vec4( color,diffuse.a);
            } else {
                frag_color = vec4(1,0,1,diffuse.a);
            }
*/
            frag_color = vec4( color,diffuse.a);

            //frag_color = vec4( shadow_depth-depth,shadow_depth-depth,shadow_depth-depth,diffuse.a);


        }
    )");

    auto light_shader = Engine::Shader::create_shader({light_vertex, light_fragment});
    light_shader->compile();
    index= glGetUniformBlockIndex(light_shader->get_ID(), "Uniform_block");
    glUniformBlockBinding(light_shader->get_ID(), index, 0);




    /*
    glUniform1i(0, 0);
    glUniform1i(1, 1);
    glUniform1i(2, 2);
    glUniform1i(3, 3);
    */
    auto shadow_camera = Engine::Orthographic_camera(-12,12,12,-12,-20,20);
    shadow_camera.set_position_axis_aligned(glm::vec3(0,0,0));
    //shadow_camera.look_at(-uniforms.sun_position.m_data);
    shadow_camera.look_at(-uniforms.sun_position.m_data);
    shadow_camera.on_update();

    light_shader->bind();
    uniforms.shadow_camera.m_data = shadow_camera.get_view_projection_matrix();
    uniforms.update_shadow_camera();
    //index = glGetUniformLocation(light_shader->get_ID(), "shadow_view_matrix");
    //glUniformMatrix4fv(index,1,GL_FALSE,(float*)&shadow_camera.get_view_projection_matrix()[0][0]);

    //auto shadow_mat = std::make_shared<Lab4::Shadowmap_material>(light_shader, shadow_camera);

    auto shadow_fbo = Engine::FrameBuffer::Create();
    float shadow_size = 2048;
    auto shadow_texture = Engine::Texture2D::Create_empty(shadow_size, shadow_size, GL_DEPTH24_STENCIL8,GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
    shadow_fbo->set_depth_texture(shadow_texture);


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


    auto framebuffer = Engine::FrameBuffer::Create();
    auto diffuse_texture = Engine::Texture2D::Create_empty(window->get_width(), window->get_height(), GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE);
    auto specular_texture = Engine::Texture2D::Create_empty(window->get_width(), window->get_height(), GL_RED, GL_RED,  GL_UNSIGNED_BYTE);
    auto normal_texture = Engine::Texture2D::Create_empty(window->get_width(),window->get_height(),GL_RGB32F,GL_RGB, GL_FLOAT);
    auto world_pos_texture = Engine::Texture2D::Create_empty(window->get_width(),window->get_height(),GL_RGB32F,GL_RGB, GL_FLOAT);


    auto depth_texture = Engine::Texture2D::Create_empty(window->get_width(), window->get_height(), GL_DEPTH24_STENCIL8,GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);

    framebuffer->add_write_color_texture(diffuse_texture);
    framebuffer->add_write_color_texture(specular_texture);
    framebuffer->add_write_color_texture(normal_texture);
    framebuffer->add_write_color_texture(world_pos_texture);
    framebuffer->add_read_color_texture(shadow_texture);
    framebuffer->set_depth_texture(depth_texture);




    //framebuffer->add_read_color_texture(shadow_texture);


    //glDepthMask(true);
    float angle = 0;
    while(!window->should_close()) {

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
        uniforms.viewport.m_data = camera.get_position();
        uniforms.update_viewport();

        angle += 0.02;
        scene.on_update(glm::translate(glm::mat4(1),glm::vec3(0,sin(angle),0)));

        /// Render to custom framebuffer
        framebuffer->bind_write();
        Engine::Render::Render_command::set_clear_color({0,0,0,0});
        Engine::Render::Renderer::begin_scene();
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        sphere_model->on_render();

        uniforms.projection_matrix.m_data = shadow_camera.get_projection_matrix();
        uniforms.view_matrix.m_data = shadow_camera.get_view_matrix();
        uniforms.update_view_matrix();
        uniforms.update_projection_matrix();


        shadow_fbo->bind_write();
        glCullFace(GL_FRONT);
        glViewport(0,0,shadow_size,shadow_size);
        Engine::Render::Render_command::set_clear_color({0,0,0,0});
        Engine::Render::Renderer::begin_scene();

        sphere_model->on_render();
        shadow_fbo->unbind();
        glViewport(0,0,window->get_width(),window->get_height());
        glCullFace(GL_BACK);
/*
        uniforms.projection_matrix.m_data = camera.get_projection_matrix();
        uniforms.view_matrix.m_data = camera.get_view_matrix();
        uniforms.update_projection_matrix();
        uniforms.update_view_matrix();
*/
        /// Second render pass to default window. This is where the "light" will shine on everything
        /// and make it visible.
        light_shader->bind();
        framebuffer->bind_read();
        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        Engine::Render::Render_command::set_clear_color({134/255.0f,198/255.0f,247/255.0f,1});
        Engine::Render::Renderer::begin_scene();
        light_vao->bind();
        glDrawArrays(GL_TRIANGLES,0,6);
        light_vao->unbind();

        gui.on_update();
        window->on_update();
    }
}