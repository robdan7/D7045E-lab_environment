#pragma once
#include <memory>
namespace Engine {
    class Texture2D;
    class FrameBuffer {
    public:
        virtual void bind_write() = 0;
        virtual void bind_read() = 0;
        virtual void unbind() = 0;
        virtual void set_depth_texture(std::shared_ptr<Texture2D> texture) = 0;
        virtual void add_write_color_texture(std::shared_ptr<Texture2D> texture) = 0;
        virtual void add_read_color_texture(std::shared_ptr<Texture2D> texture) = 0;
        static std::shared_ptr<FrameBuffer> Create();


    };
}

