#pragma once

#include <vector>
#include <memory>
#include <Core/Window.h>
//#include <Core/Layers/Application_layer_st.h>

namespace Cecilion {
    class ImGui_panel;
    class ImGui_layer {
        friend class ImGui_panel;
    public:
        ImGui_layer(std::shared_ptr<Cecilion::Window> window);
        //void on_attach();
        ~ImGui_layer();
        void push_panel(std::shared_ptr<ImGui_panel> panel);
        void pop_panel(std::shared_ptr<ImGui_panel>panel);
        //static void Init();
        void on_update();
    private:
        void on_imgui_render();
    private:
////        void on_detach() override;
        void begin();
        void end();
    private:
        void update_layer_set();
        //static std::shared_ptr<ImGui_layer> s_gui_layer;

        std::vector<std::shared_ptr<ImGui_panel>> s_panels;
        std::vector<std::shared_ptr<ImGui_panel>> s_append_layer_stack;
        std::vector<std::shared_ptr<ImGui_panel>> s_delete_stack;
        float m_time = 0.0f;
    };
}


