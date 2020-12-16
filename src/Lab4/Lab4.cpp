#define API_OPENGL
#include <Engine.h>

#define PI (atan(1)*4)
BEGIN_SHADER_CONST(Global_uniforms)
    SET_SHADER_CONST(Engine::Matrix4_data, view_matrix,false)
    SET_SHADER_CONST(Engine::Matrix4_data, projection_matrix,false)
    SET_SHADER_CONST(Engine::Float3_data, viewport,false)
    SET_SHADER_CONST(Engine::Float3_data, ambient,false)
    SET_SHADER_CONST(Engine::Float3_data, diffuse,false)
    SET_SHADER_CONST(Engine::Float3_data, specular,false)
    SET_SHADER_CONST(Engine::Float3_data, sun_position,true)
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
        ImGui::SliderFloat("Camera center offset", &this->m_distance, 2,10);
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
    float m_distance = 3;
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
            //layout(location = 0) out vec4 vs_color;
            layout(location = 0) out vec3 vs_normal;
            layout(location = 1) out vec3 vs_position;

            layout(std140) uniform Uniform_block {
                mat4 view_matrix;
                mat4 projection_matrix;
                vec3 viewport;
                vec3 ambient;
                vec3 diffuse;
                vec3 specular;
                vec3 sun;
            } my_block;
            void main() {
                gl_Position = my_block.projection_matrix * my_block.view_matrix*vec4(position,1);
                vs_position = position;
                vs_normal = normal;
            }

            )");
    auto fragment = Engine::Shader::create_shader_stage(
            GL_FRAGMENT_SHADER,
            R"(
            #version 450 core
            layout(location = 0) in vec3 vs_normal;
            layout(location = 1) in vec3 fragPos;

            out vec4 diffuse_color;
            out float specular_power;
            out vec3 fragment_normal;
            out vec3 world_position;

            layout(std140) uniform Uniform_block {
                mat4 view_matrix;
                mat4 projection_matrix;
                vec3 viewport;
                vec3 ambient;
                vec3 diffuse;
                vec3 specular;
                vec3 sun;
            } my_block;

            void main() {

                diffuse_color = vec4(vec3(0.6,0.6,0.7),1);
                specular_power = 1.5;
                fragment_normal = normalize(vs_normal);
                world_position = fragPos;
            }
            )");

    auto shader_program = Engine::Shader::create_shader({vertex, fragment});
    shader_program->compile();

    /// The uniform bindings must be done manually.
    GLuint index= glGetUniformBlockIndex(shader_program->get_ID(), "Uniform_block");
    glUniformBlockBinding(shader_program->get_ID(), index, 0);

    uniforms.ambient.m_data = glm::fvec3(0.3,0.3,0.3);
    uniforms.diffuse.m_data = glm::fvec3(0.7,0.7,0.7);
    uniforms.sun_position.m_data = glm::fvec3 (0,1,0.4f);
    uniforms.specular.m_data = glm::fvec3(1.0f,1.0f,1.0f);
    uniforms.sun_position.m_data /= glm::length(uniforms.sun_position.m_data);
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

    std::vector<float> vertices;
    create_prism(vertices,1,1,32);
    /*
    vertices.emplace_back(0,0,0);
    vertices.emplace_back(-1,0,0);
    vertices.emplace_back(-1,1,0);
    */
    auto vbo = Engine::Vertex_buffer::Create(&vertices[0],vertices.size()*sizeof(vertices[0]), Engine::Raw_buffer::Access_frequency::STATIC, Engine::Raw_buffer::Access_type::DRAW);
    vbo->set_layout(Engine::Buffer_layout{{Engine::Shader_data::Float3, "position"},{Engine::Shader_data::Float3, "normal",true}});
    auto vao = Engine::Vertex_array::Create();
    vao->add_vertex_buffer(vbo);


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glLineWidth(3);
    glPointSize(3);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    Engine::Render::Render_command::set_clear_color({134/255.0f,198/255.0f,247/255.0f,1});

    auto screen_vertex = Engine::Shader::create_shader_stage(GL_VERTEX_SHADER, R"(
        #version 450 core
        layout(location = 0) in vec2 position;
        out vec2 tex_coords;

        void main() {
            gl_Position = vec4(position.x, position.y,0,1);
            tex_coords = position.xy*0.5+0.5;
            //tex_coords = vec2(1,1);
        }
    )");

    auto screen_frag = Engine::Shader::create_shader_stage(GL_FRAGMENT_SHADER, R"(
        #version 450 core
        layout(location = 0) in vec2 tex_coords;
        layout(location = 0) out vec4 frag_color;

        layout(location = 0) uniform sampler2D diffuse_texture;
        layout(location = 1) uniform sampler2D specular_texture;
        layout(location = 2) uniform sampler2D normal_texture;
        layout(location = 3) uniform sampler2D frag_coord_texture;

        layout(std140) uniform Uniform_block {
            mat4 view_matrix;
            mat4 projection_matrix;
            vec3 viewport;
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            vec3 sun;
        } my_block;


        vec3 phong(vec3 color,vec3 fragPos, vec3 normal,vec3 viewport, vec3 light, vec3 ambient, vec3 diffuse, vec3 specular) {
                vec3 viewDir = normalize(viewport - fragPos);
                vec3 reflect = reflect(-light,normal);
                float spec_power = pow(max(dot(reflect,viewDir),0.0),32);

                vec3 specular_light = specular*spec_power;
                vec3 diffuse_light = diffuse*dot(normal,light);

                return color*(ambient+diffuse_light+ specular_light);
            }



        void main() {
            vec3 diffuse = texture(diffuse_texture, tex_coords);
            vec3 normal = texture(normal_texture, tex_coords);
            float specular_power = texture(specular_texture, tex_coords).r;
            vec3 frag_position = texture(frag_coord_texture, tex_coords).rgb;


            vec3 color = phong(diffuse,frag_position.xyz,normal,my_block.viewport,my_block.sun,my_block.ambient,my_block.diffuse,my_block.specular);
            frag_color = vec4(diffuse.rgb,1);
        }
    )");

    auto screen_shader = Engine::Shader::create_shader({screen_vertex, screen_frag});
    screen_shader->compile();

    screen_shader->bind();
    glUniform1i(0, 0);
    glUniform1i(1, 1);
    glUniform1i(2, 2);
    glUniform1i(3, 3);

    float data[] = {
            -1,-1,
            1,-1,
            -1,1,
            1,-1,
            1,1,
            -1,1
    };
    auto screen_vbo = Engine::Vertex_buffer::Create(&data,sizeof(data),Engine::Raw_buffer::Access_frequency::STATIC,Engine::Raw_buffer::Access_type::DRAW);
    screen_vbo->set_layout(Engine::Buffer_layout{{Engine::Shader_data::Float2, "position"}});
    auto screen_vao = Engine::Vertex_array::Create();
    screen_vao->add_vertex_buffer(screen_vbo);

    auto framebuffer = Engine::FrameBuffer::Create();
    auto diffuse_texture = Engine::Texture2D::Create_empty(window->get_width(), window->get_height(), GL_RGBA, GL_UNSIGNED_BYTE);
    auto specular_texture = Engine::Texture2D::Create_empty(window->get_width(), window->get_height(),  GL_RED,  GL_FLOAT);
    auto normal_texture = Engine::Texture2D::Create_empty(window->get_width(),window->get_height(),GL_RGBA, GL_UNSIGNED_BYTE);
    auto world_pos_texture = Engine::Texture2D::Create_empty(window->get_width(),window->get_height(),GL_RGB, GL_FLOAT);
    auto depth_texture = Engine::Texture2D::Create_empty(window->get_width(), window->get_height(), GL_DEPTH_COMPONENT, GL_FLOAT);
    framebuffer->add_color_texture(diffuse_texture);
    framebuffer->add_color_texture(specular_texture);
    framebuffer->add_color_texture(normal_texture);
    framebuffer->add_color_texture(world_pos_texture);
    framebuffer->set_depth_texture(depth_texture);
    framebuffer->bind();
    GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4, buffers);

    glActiveTexture(GL_TEXTURE0);
    diffuse_texture->bind();

    glActiveTexture(GL_TEXTURE1);
    specular_texture->bind();

    glActiveTexture(GL_TEXTURE2);
    normal_texture->bind();

    glActiveTexture(GL_TEXTURE3);
    world_pos_texture->bind();

    while(!window->should_close()) {

        framebuffer->bind();
        Engine::Render::Render_command::set_clear_color({134/255.0f,198/255.0f,247/255.0f,1});
        Engine::Render::Renderer::begin_scene();
        glEnable(GL_DEPTH_TEST);

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

        vao->bind();
        shader_program->bind();
        glDrawArrays(GL_TRIANGLES,0,vertices.size());
        gui.on_update();

        framebuffer->unbind();
        //Engine::Render::Render_command::set_clear_color({134/255.0f,198/255.0f,247/255.0f,1});
        Engine::Render::Renderer::begin_scene();
        screen_vao->bind();
        screen_shader->bind();
        glDisable(GL_DEPTH_TEST);

        glDrawArrays(GL_TRIANGLES,0,6);
        glEnable(GL_DEPTH_TEST);
        screen_vao->unbind();

        window->on_update();
    }
}