#include "FrameBuffer.h"
#include "Platform/OpenGL/GL_Framebuffer.h"
namespace  Engine {
    std::shared_ptr<FrameBuffer> FrameBuffer::Create() {
        return std::make_shared<GL_Framebuffer>();
    }
}

