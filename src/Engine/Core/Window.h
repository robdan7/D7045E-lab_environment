#pragma once
#include <string>
#include <Renderer/GLFW_context.h>

namespace Engine {
    /**
     * Abstract window class for Engine. The engine doesn't care what kind of window we're using.
     * It could be on Windows, Linux or Mac, Engine doesn't care.
     *
     * TODO Implement support for multiple windows. Very low priority.
     */
    struct Window_properties {
        std::string title;
        unsigned int width, height;

        Window_properties(
                const std::string &title = "Demo window",
                unsigned int width = 1280,
                unsigned int height = 720,
                GLFW_context* context = nullptr) :
                title(title), width(width), height(height) {}
    };

    /**
     * Generic window class.
     */
    class Window {
    public:
        virtual ~Window() {}
        virtual void on_update() = 0;
        virtual unsigned int get_height() const = 0;
        virtual unsigned int get_width() const = 0;
        virtual void set_Vsync(bool vsync) = 0;
        virtual bool has_Vsync() const = 0;
        static Window* create_window(Window_properties properties = Window_properties());
        virtual void* get_native_window() = 0;
        virtual void show_cursor() = 0;
        virtual void hide_cursor() = 0;
        virtual void disable_cursor() = 0;
        virtual bool should_close() = 0;
    };
}