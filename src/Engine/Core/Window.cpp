#include "Window.h"
#include "Core.h"

#if  defined(Engine_PLATFORM_WINDOWS_X84) || defined(Engine_PLATFORM_WINDOWS_X64)
    #include <Platform/Windows/W_window.h>
#endif


namespace Engine {
    Window *Window::create_window(Window_properties properties) {
        /// TODO remove separate argument and force the user to define properties.
    #if  defined(Engine_PLATFORM_WINDOWS_X84) || defined(Engine_PLATFORM_WINDOWS_X64)
        return new W_window(properties, Engine::create_engine_context());
    #else
        CORE_ASSERT(false, "The platform ain't windows, u dumdum!")
        return nullptr;
    #endif
    }

    Window::Window() {
        this->m_event_listener.set_callback_func<Engine::Hide_key_event>([this](auto event){
            this->hide_cursor();
        });
        this->m_event_listener.set_callback_func<Engine::Show_cursor_event>([this](auto event){
            this->show_cursor();
        });
        this->m_event_listener.set_callback_func<Engine::Disable_cursor_event>([this](auto event){
            this->disable_cursor();
        });
    }

    void Window::on_update() {
        this->m_event_listener.on_update();
    }
}

