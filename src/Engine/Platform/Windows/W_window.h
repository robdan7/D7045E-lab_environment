#pragma once

#include "Core/Window.h"
#include <Platform/OpenGL/OpenGL.h>
#include <Renderer/GLFW_context.h>

namespace Engine {
    class W_window : public Window{
    public:
        explicit W_window(const Window_properties &properties, GLFW_context* context);
        ~W_window() override;
        void on_update() override;
        [[nodiscard]] inline unsigned int get_width() const override {return m_data->width;};
        [[nodiscard]] inline unsigned int get_height() const override {return m_data->height;};
        void set_Vsync(bool vsync) override ;
        [[nodiscard]] bool has_Vsync() const override;
        void activate_resize();
        inline void* get_native_window() override {return this->m_window;}

        void show_cursor() override;

        void hide_cursor() override;

        void disable_cursor() override;

        bool should_close() override;

        std::tuple<double, double> get_cursor_pos() override;

    private:

        virtual void shutdown();
        virtual void Init(const Window_properties& properties, GLFW_context* context);
        //GLFWdindow* wandow;
        struct Window_data {
            std::string title;
            unsigned int width, height;
            bool vsync;
        };
        GLFW_context* m_graphics_context;
        Window_data* m_data;
        GLFWwindow* m_window;
    };
}






