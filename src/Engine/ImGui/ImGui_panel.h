#pragma once
#include "ImGui_build.h"
#include "ImGui_panel.h"
#include <iostream>
namespace Engine {
    class ImGui_panel {
    public:
        virtual void on_imgui_render() {}
    };

}


