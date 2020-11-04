#include "ImGui_layer.h"

#include "ImGui_build.h"
//#include <Platform/OpenGL/imgui_impl_opengl3.h>
#include <Platform/OpenGL/OpenGL.h>

//#include <Event/Async_inbox.h>
#include "ImGui_panel.h"
// TODO Make sure the ImGui interface can handle multiple threads.
namespace Cecilion {
    //std::vector<std::shared_ptr<ImGui_panel>> ImGui_layer::s_panels = std::vector<std::shared_ptr<ImGui_panel>>();
    //std::vector<std::shared_ptr<ImGui_panel>> ImGui_layer::s_append_layer_stack = std::vector<std::shared_ptr<ImGui_panel>>();
    //std::vector<std::shared_ptr<ImGui_panel>> ImGui_layer::s_delete_stack = std::vector<std::shared_ptr<ImGui_panel>>();
    //std::shared_ptr<ImGui_layer> ImGui_layer::s_gui_layer = nullptr;
/*
    void ImGui_layer::on_attach() {
        //CORE_ASSERT(!this->p_parent, "Layer was attached without a parent!");


        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();


        Application& app = Application::get();
        GLFWwindow* window = static_cast<GLFWwindow*>(app.get_window().get_native_window());

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 410");
    }
    */

//    void ImGui_layer::on_detach() {
//        ImGui_ImplOpenGL3_Shutdown();
//        ImGui_ImplGlfw_Shutdown();
//        ImGui::DestroyContext();
//    }

//    void ImGui_layer::on_update() {
//        Application_layer_st::on_update();
//
//        ImGuiIO& io = ImGui::GetIO();
//        Application& app = Application::get();
//        io.DisplaySize = ImVec2(app.get_window().get_width(), app.get_window().get_height());
//
//        float time = (float)glfwGetTime();
//        io.DeltaTime = this->m_time > 0.0f ? (time - this->m_time) : (1.0f / 60.0f);
//        this->m_time = time;
//
//        ImGui_ImplOpenGL3_NewFrame();
//        ImGui::NewFrame();
//
//        static bool show = true;
//        ImGui::ShowDemoWindow(&show);
//
//        ImGui::Render();
//        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//    }

    void ImGui_layer::begin() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        static bool opt_fullscreen_persistant = true;
        bool opt_fullscreen = opt_fullscreen_persistant;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen) {
            ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->GetWorkPos());
            ImGui::SetNextWindowSize(viewport->GetWorkSize());
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        bool p_open = true;
        //ImGui::Begin("DockSpace", &p_open, window_flags);
        ImGui::PopStyleVar();

        if (opt_fullscreen)
           ImGui::PopStyleVar(2);
        //ImGuiID dockspace_id = ImGui::GetID("DockSpace");
        //ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

        /*
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                //if (ImGui::MenuItem("Exit")) Cecilion::Event_system::post<Cecilion::Window_close_event>();
                ImGui::Separator();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                //if (ImGui::MenuItem("Exit")) Cecilion::Event_system::post<Cecilion::Window_close_event>();
                ImGui::Separator();
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Dummy")) {
                //if (ImGui::MenuItem("Exit")) Cecilion::Event_system::post<Cecilion::Window_close_event>();
                ImGui::Separator();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        //ImGui::End();
         */
    }

    void ImGui_layer::end() {
//        ImGuiIO& io = ImGui::GetIO();
//        Application& app = Application::get();
//        io.DisplaySize = ImVec2(app.get_window().get_width(), app.get_window().get_height());

        /// Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void ImGui_layer::on_update() {
        this->begin();
        this->on_imgui_render();
        this->end();
    }


    ImGui_layer::~ImGui_layer() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGui_layer::on_imgui_render() {
        update_layer_set();
        for (auto &layer : this->s_panels) {
            layer->on_imgui_render();
        }
//        bool show = true;
//        ImGui::ShowDemoWindow(&show);


//        ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
//        ImGui::Begin("One window");
//        ImGui::Button("Hello button");
//        ImGui::End();
//
//        ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
//        ImGui::Begin("Demo window!");
//        ImGui::Button("Hello button");
//        ImGui::Text("Woooow hello world!");
//        ImGui::End();
//
//        ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
//        ImGui::Begin("Another window");
//        ImGui::Button("Hello button");
//        ImGui::End();
    }

/*
    void ImGui_layer::Init() {
        if (ImGui_layer::s_gui_layer != nullptr) {
            //CORE_LOG_ERROR("ImGui::ImGui_layer:: Gui layer has already been initialized!");
            return;
        }
        Cecilion::Layer<>* imgui = new Cecilion::ImGui_layer();
        std::shared_ptr<Cecilion::Layer<>> gui = std::make_shared<ImGui_layer>();
        ImGui_layer::s_gui_layer = std::static_pointer_cast<ImGui_layer>(gui);
        Cecilion::Application::get().append_layer(gui);
    }
*/
    void ImGui_layer::push_panel(std::shared_ptr<ImGui_panel> panel) {
        /*if (ImGui_layer::s_gui_layer == nullptr) {
            //CORE_LOG_WARN("ImGui::ImGui_layer:: ImGui layer has not been initialized yet. Panel will not be rendered!");
            return;
        }*/
        this->s_append_layer_stack.push_back(panel);
    }


    void ImGui_layer::pop_panel(std::shared_ptr<ImGui_panel> panel) {
        ImGui_layer::s_delete_stack.push_back(panel);
    }

    void ImGui_layer::update_layer_set() {


        for (auto& layer :this->s_append_layer_stack) {
            this->s_panels.push_back(layer);
        }
        this->s_append_layer_stack.clear();
        for (auto& layer : this->s_delete_stack) {
            auto pointer = std::find(this->s_panels.begin(), this->s_panels.end(), layer);
            if(pointer != this->s_panels.end()) {
                if (pointer->get() == this->s_panels.back().get()) {
                    this->s_panels.pop_back();
//                    std::cout << "Panel size: " << std::to_string(ImGui_layer::s_panels.size()) << std::endl;
                    continue;
                }
                pointer->reset(this->s_panels.back().get());
                this->s_panels.pop_back();
//                std::cout << "Panel size: " << std::to_string(ImGui_layer::s_panels.size()) << std::endl;
            } else {
                //CORE_LOG_WARN("Imgui::ImGui_layer:: Tried to delete non existing panel from ImGui!");
            }
        }
        this->s_delete_stack.clear();
    }

    ImGui_layer::ImGui_layer(std::shared_ptr<Cecilion::Window> window) {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();

        // Setup Platform/Renderer bindings

        ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window->get_native_window()), true);
        ImGui_ImplOpenGL3_Init("#version 410");
    }


}