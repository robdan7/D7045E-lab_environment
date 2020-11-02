#define API_OPENGL
#include <Engine.h>



int main(int argc, char** argv) {

    auto window = Cecilion::Window::create_window();

    while(!window->should_close()) {
        window->on_update();
    }
}