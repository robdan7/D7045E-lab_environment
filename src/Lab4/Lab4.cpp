#define API_OPENGL
#include <Engine.h>
#include "Monochrome_material.h"
#include "Scene.h"
#include "GraphicsNode.h"
#include "Shadowmap_material.h"
#include "Shadow_map.h"
#include "Cube_map.h"
#include "Point_light.h"
#include "Geometry.h"

#define PI (atan(1)*4)
BEGIN_SHADER_CONST(Global_uniforms)
    SET_SHADER_CONST(Engine::Matrix4_data, viewprojection_matrix,false)
    //SET_SHADER_CONST(Engine::Matrix4_data, projection_matrix,false)
    SET_SHADER_CONST(Engine::Float3_data, viewport,false)
    SET_SHADER_CONST(Engine::Int2_data, viewport_size, false);
    SET_SHADER_CONST(Engine::Float3_data, ambient,false)
    SET_SHADER_CONST(Engine::Float3_data, diffuse,false)
    SET_SHADER_CONST(Engine::Float3_data, specular,false)
    SET_SHADER_CONST(Engine::Float4_data, sun_position,true)
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
        ImGui::SliderFloat("Camera center offset", &this->m_distance, 2,50);
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
    float m_pitch = -0.1;
    float m_distance = 20;
    int objects = 0;
    float m_fov = 45;
};

int main(int argc, char** argv) {
    auto window = std::shared_ptr<Engine::Window>(Engine::Window::create_window());
    Global_uniforms uniforms;
    uniforms.viewport_size.m_data = glm::ivec2(window->get_width(),window->get_height());
    uniforms.update_viewport_size();

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
                //mat4 projection_matrix;
                vec3 viewport;
                ivec2 view_size;
                vec3 ambient;
                vec3 diffuse;
                vec3 specular;
                vec4 sun;
                mat4 shadow_camera;
            } my_block;

            void main() {
                gl_Position = my_block.view_matrix*transform*vec4(position,1);
                vs_position = (transform*vec4(position,1)).xyz;
                vs_normal = normalize((transform*vec4(normal,0)).xyz);
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
                //mat4 projection_matrix;
                vec3 viewport;
                ivec2 view_size;
                vec3 ambient;
                vec3 diffuse;
                vec3 specular;
                vec4 sun;
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
    uniforms.sun_position.m_data = glm::fvec4 (0,0,0,1.0f);
    uniforms.specular.m_data = glm::fvec3(1.0f,1.0f,1.0f);
    //uniforms.sun_position.m_data /= glm::length(uniforms.sun_position.m_data);
    uniforms.update_ambient();
    uniforms.update_diffuse();
    uniforms.update_specular();
    uniforms.update_sun_position();

    /// -------- Camera --------
    float aspect_ratio = ((float)window->get_width()/window->get_height());
    //auto camera = Engine::Orthographic_camera(-aspect_ratio,aspect_ratio,1,-1,0,1);
    auto camera = Engine::Perspective_camera(0.01f,1000.0f,glm::radians(panel->fov()),aspect_ratio);
    camera.set_position_axis_aligned({0.0f,0.0f,10.0f});
    camera.look_at({0.0f,0.0f,0.0f});
    camera.on_update(); /// This is the only update needed.

    /*
    uniforms.view_matrix.m_data = camera.get_view_matrix();
    uniforms.projection_matrix.m_data = camera.get_projection_matrix();
    uniforms.update_view_matrix();
    uniforms.update_projection_matrix();
    */
    uniforms.viewprojection_matrix.m_data = camera.get_view_projection_matrix();
    uniforms.update_viewprojection_matrix();

    /// Create raw mesh source
    Engine::Import_mesh mesh_data;
    Lab4::create_sphere(mesh_data.vertices,64);
    mesh_data.materials.emplace_back("",0);
    mesh_data.materials[0].vertices =mesh_data.vertices.size()/6;
    mesh_data.draw_type = Engine::Draw_type::STREAM;
    mesh_data.m_vertex_buffer = Engine::Vertex_buffer::Create(
            &mesh_data.vertices[0],mesh_data.vertices.size()*sizeof(mesh_data.vertices[0]),
            Engine::Raw_buffer::Access_frequency::STATIC,
            Engine::Raw_buffer::Access_type::DRAW);

    /// container for the pink cube
    auto my_material = std::make_shared<Engine::Material>(shader_program);
    auto model_1 = std::shared_ptr<Engine::Instanced_mesh>(new Engine::Instanced_mesh({{Engine::Shader_data::Float4, "color"}, {Engine::Shader_data::Float, "specular_power"}, {Engine::Shader_data::Mat4, "transform"}}, 2));
    model_1->add_LOD(mesh_data, my_material);


    Engine::Import_mesh mesh_data2;
    Lab4::create_prism(mesh_data2.vertices,1,10,64);
    mesh_data2.materials.emplace_back("",0);
    mesh_data2.materials[0].vertices =mesh_data2.vertices.size()/6;
    mesh_data2.draw_type = Engine::Draw_type::STREAM;
    mesh_data2.m_vertex_buffer = Engine::Vertex_buffer::Create(
            &mesh_data2.vertices[0],mesh_data2.vertices.size()*sizeof(mesh_data2.vertices[0]),
            Engine::Raw_buffer::Access_frequency::STATIC,
            Engine::Raw_buffer::Access_type::DRAW);

    auto model_2 = std::shared_ptr<Engine::Instanced_mesh>(new Engine::Instanced_mesh({{Engine::Shader_data::Float4, "color"}, {Engine::Shader_data::Float, "specular_power"}, {Engine::Shader_data::Mat4, "transform"}}, 2));
    model_2->add_LOD(mesh_data2, my_material);





    auto transform = glm::translate(glm::mat4(1),glm::vec3(0,0,0));
    auto scene = Lab4::InnerNode(transform);
    {
        auto mat = glm::translate(glm::mat4(1), glm::vec3(0, -3, 0));
        std::vector<float> data = {1, 1, 1, 1, 6};
        {
            auto instance_1 = std::make_shared<Lab4::GeometryNode>(model_1, &data[0], mat);
            scene.add_node(instance_1);
        }
        {
            data = {0.1f,0.3f,1,1,0};
            mat = glm::translate(glm::mat4(1),glm::vec3(0,3,0));
            auto instance_1 = std::make_shared<Lab4::GeometryNode>(model_1, &data[0], mat);
            scene.add_node(instance_1);
        }
/*
        {
            data = {1,0.6f,0.6f,1,1};
            mat = glm::translate(glm::scale(glm::mat4(1),glm::vec3(3,1,3)),glm::vec3(0,5,0));
            auto instance_2 = std::make_shared<Lab4::GeometryNode>(model_2, &data[0], mat);
            scene.add_node(instance_2);
        }
*/
        {
            data = {0.2f, 0.8f, 0.7f, 1, 1};
            mat = glm::translate(glm::scale(glm::mat4(1), glm::vec3(3, 1, 3)), glm::vec3(0, -5, 0));
            auto instance_2 = std::make_shared<Lab4::GeometryNode>(model_2, &data[0], mat);
            scene.add_node(instance_2);
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
    auto light_vertex = Engine::Shader::create_shader_stage(GL_VERTEX_SHADER, R"(
        #version 450 core
        layout(location = 0) in vec2 position;
        noperspective out vec2 tex_coords;

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
        layout(location = 2) uniform sampler2D normal_texture;
        layout(location = 3) uniform sampler2D frag_coord_texture;
        layout(location = 4) uniform samplerCube depth_texture;

        layout(std140) uniform Uniform_block {
            mat4 view_matrix;
            //mat4 projection_matrix;
            vec3 viewport;
            ivec2 view_size;
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;
            vec4 sun;
            mat4 shadow_camera;
        } my_block;

        uniform mat4 shadow_view_matrix;


        vec3 phong(vec3 color,vec3 fragPos, vec3 normal,vec3 viewport, vec4 light, vec3 ambient, vec3 diffuse, vec3 specular) {
                vec3 light_normal = normalize(light.xyz-fragPos*light.w);
                vec3 viewDir = normalize(viewport - fragPos);
                vec3 reflect = reflect(-light_normal,normal);
                float spec_power = pow(max(dot(reflect,viewDir),0.0),32);


                float d = dot(normal,light_normal);
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

            vec4 shadow_coords = my_block.view_matrix*vec4(frag_position,1);
            //float shadow_depth = texture(depth_texture, shadow_coords.xy*0.5+0.5).r;
            float shadow_depth = texture(depth_texture, frag_position).r*100;

            float depth = shadow_coords.z*0.5+0.5;
            depth = length(frag_position);

            float my_var = shadow_depth+0.05 > depth ? 1 : 0;

            vec3 color = phong(diffuse.rgb,frag_position.xyz,normal,my_block.viewport,my_block.sun,my_block.ambient,my_block.diffuse*my_var,specular_power*my_block.specular*my_var);
            frag_color = vec4( diffuse.rgb*0.1,diffuse.a);
            gl_FragDepth = (shadow_coords.z/shadow_coords.w*0.5+0.5);
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

    auto shadowmap = std::make_shared<Lab4::Shadow_map>(720, 720, window);
    auto cube_map = std::make_shared<Lab4::Cube_map>(720,720,window);

    //auto shadow_texture = Engine::Texture2D::Create_empty(720, shadow_size, GL_DEPTH24_STENCIL8,GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
    //shadow_fbo->set_depth_texture(shadow_texture);


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
    //framebuffer->add_read_color_texture(shadowmap->get_gexture());
    framebuffer->add_read_color_texture(cube_map->get_gexture());
    framebuffer->set_depth_texture(depth_texture);




    //framebuffer->add_read_color_texture(shadow_texture);
    auto pointlight = Lab4::GL_Point_light::Create(1);
    //auto light_instance = pointlight->add_instance({1,1,1},{0,0,0},1/10.0f,0);


    {
        auto light_instance = std::make_shared<Lab4::LightNode>(pointlight,glm::vec3(1.0f,1.0f,1.0f),glm::vec4(0.0f,0.0f,0.0f,1.0f),8.0f,0.75f);
        scene.add_node(light_instance);
    }

    {
        std::srand(time(nullptr));
        for (int i = 0; i < 100; i++) {
            float x = (rand()%1000)/50.0f-10.0f;
            float z = (rand()%1000)/50.0f-10.0f;
            auto light_instance = std::make_shared<Lab4::LightNode>(pointlight,glm::vec3(1.0f,1.0f,1.0f),glm::vec4(x,-4.0f,z,1),1.0f,0.5);
            scene.add_node(light_instance);
        }

    }

    //pointlight->add_instance({1,1,1},{8,-2,5},0,0.1f);
    //pointlight->add_instance({1,1,1},{0,4.5f,0},0,0.1f);

    //glDepthMask(true);
    float angle = 0;
    while(!window->should_close()) {

        /// Update camera position
        auto matrix = glm::rotate(glm::mat4(1),panel->pitch(), glm::vec3(1,0,0));
        glm::vec4 position = matrix*glm::vec4(0,0,panel->offset(),1);
        matrix = glm::rotate(glm::mat4(1),panel->yaw(), glm::vec3(0,1,0));
        position = matrix*position;
        camera.set_position_axis_aligned(position);
        camera.set_FOV(glm::radians(panel->fov()));
        camera.on_update();
        /*
        uniforms.projection_matrix.m_data = camera.get_projection_matrix();
        uniforms.view_matrix.m_data = camera.get_view_matrix();
        uniforms.update_projection_matrix();
        uniforms.update_view_matrix();
         */
        uniforms.viewprojection_matrix.m_data = camera.get_view_projection_matrix();
        uniforms.update_viewprojection_matrix();
        uniforms.viewport.m_data = camera.get_position();
        uniforms.update_viewport();

        angle += 0.01;
        scene.on_update(glm::translate(glm::rotate(glm::mat4(1),angle*0.5f,glm::vec3(0,1,0)),glm::vec3(0,2*sin(angle),0)));

        /// Render to custom framebuffer

        framebuffer->bind_write();
        Engine::Render::Render_command::set_clear_color({0,0,0,0});
        Engine::Render::Renderer::begin_scene();
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        model_1->on_render();
        model_2->on_render();

        /*
        uniforms.projection_matrix.m_data = shadow_camera.get_projection_matrix();
        uniforms.view_matrix.m_data = shadow_camera.get_view_matrix();
        uniforms.update_view_matrix();
        uniforms.update_projection_matrix();
        */
        uniforms.viewprojection_matrix.m_data = shadow_camera.get_view_projection_matrix();
        uniforms.update_viewprojection_matrix();

        shadowmap->bind_write();
        Engine::Render::Renderer::begin_scene();
        model_1->on_render();
        model_2->on_render();
        shadowmap->unbind();


        cube_map->bind_write();
        glClear(GL_DEPTH_BUFFER_BIT);
        //shadowmap->get_material()->bind();
        //sphere_model->m_lod[0].mesh_partitions[0].m_vao->bind();
        //glDrawArraysInstanced(GL_TRIANGLES,0,mesh_data.materials[0].vertices,10);
        model_1->on_render(cube_map->get_material());
        model_2->on_render(cube_map->get_material());
        //sphere_model->on_render(cube_map->get_material());
        cube_map->unbind();


        /// Second render pass to default window. This is where the "light" will shine on everything
        /// and make it visible.
        uniforms.viewprojection_matrix.m_data = camera.get_view_projection_matrix();
        uniforms.update_viewprojection_matrix();
        uniforms.viewport.m_data = camera.get_position();
        uniforms.update_viewport();
        framebuffer->bind_read();
        light_shader->bind();

        for (int i = 0; i < 10; i++) {
            glUniform1i(i,i);   /// Quick fix for sampler bindings. Not entirely sure if this belongs in the framebuffer :/
        }


        glEnable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        Engine::Render::Render_command::set_clear_color({134/255.0f,198/255.0f,247/255.0f,1});
        Engine::Render::Renderer::begin_scene();
        //sunlight->on_render();
        /// This should be replaced by an ambient light source.
        light_vao->bind();
        glDrawArrays(GL_TRIANGLES,0,6);
        light_vao->unbind();
        //glEnable(GL_DEPTH_CLAMP);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        pointlight->on_render();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glDisable(GL_DEPTH_CLAMP);
        gui.on_update();
        window->on_update();
    }
}