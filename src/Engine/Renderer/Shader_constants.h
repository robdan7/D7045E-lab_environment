#pragma once
#include <cstdint>
#include <utility>
#include <Renderer/Buffer.h>
#include <iostream>
#include <iostream>

/// Test code for global shader parameters.
/// ------ Shader param data ------

namespace Engine {
    struct I_shader_constant_container;


    template<typename Data>
    class Data_governor {
    public:
        explicit Data_governor(Data* data) : p_data(data) {}
    protected:
        Data* p_data;
    };

    template<typename Data>
    struct I_data : public Engine::Buffer_element {
        friend Data_governor<Data>;
//        friend class I_shader_constant_container;
    public:
        uint32_t offset;
        Data m_data;
    public:
        const Data* get_data() {
            return &this->m_data;
        }
        [[nodiscard]] uint32_t get_offset() const {return offset;}
        virtual uint32_t stride() {return sizeof(this->m_data);}
        const uint32_t get_offset() {return this->offset;}
    protected:
        I_data(I_data<Data>&& other)  noexcept : offset(other.offset), Engine::Buffer_element(other) {
            this->m_data = other.m_data;
        }
        I_data(uint32_t offset, Engine::Shader_data type, const char* name, bool normalized = false) : Engine::Buffer_element(type,name,normalized),offset(offset) {}

    };

    struct Float_data : public I_data<float>{
        Float_data(uint32_t offset, const char* name, bool normalized = false) : I_data(offset, Engine::Shader_data::Float, name, normalized){}
    };

    struct Float2_data : public I_data<glm::vec2>{
        Float2_data(uint32_t offset, const char* name, bool normalized = false) : I_data(offset, Engine::Shader_data::Float, name, normalized){}
    };

    struct Float3_data : public I_data<glm::vec3>{
        Float3_data(uint32_t offset, const char* name, bool normalized = false) : I_data(offset, Engine::Shader_data::Float, name, normalized){}
    };

    struct Float4_data : public I_data<glm::vec4>{
        Float4_data(uint32_t offset, const char* name, bool normalized = false) : I_data(offset, Engine::Shader_data::Float, name, normalized){}
    };

    struct Int_data : public I_data<int> {
        Int_data(uint32_t offset, const char* name, bool normalized = false) : I_data(offset, Engine::Shader_data::Int, name, normalized){}
    };

    struct Int2_data : public I_data<glm::ivec2> {
        Int2_data(uint32_t offset, const char* name, bool normalized = false) : I_data(offset, Engine::Shader_data::Int, name, normalized){}
    };

    struct Int3_data : public I_data<glm::ivec3> {
        Int3_data(uint32_t offset, const char* name, bool normalized = false) : I_data(offset, Engine::Shader_data::Int, name, normalized){}
    };

    struct Int4_data : public I_data<glm::ivec4> {
        Int4_data(uint32_t offset, const char* name, bool normalized = false) : I_data(offset, Engine::Shader_data::Int, name, normalized){}
    };


    struct Matrix4_data : public I_data<glm::mat4x4> {
        Matrix4_data(uint32_t offset, const char* name, bool normalized = false) : I_data(offset, Engine::Shader_data::Mat4, name, normalized){}
    };


/// ------- Base class for shader params ------

/**
 * The code below is used to create object-oriented uniform blocks. It's still a work in progress.
 *
 * Start with BEGIN_SHADER_CONST and the name of the struct you want. Add individual uniforms with
 * SET_SHADER_CONST and finish everything with END_SHADER_CONST.
 *
 * Every uniform you add will be put in a shared buffer that automatically enforces the std140 layout.
 */

    struct I_shader_constant_container {
    private:
    protected:
        uint32_t size = 0;
        template<typename Arg>
        Arg set_arg(const char* name) {
            Arg a = Arg(size,name,false);
            this->size += a.stride();
            return std::move(a);
        }
    public:
        //virtual void write(I_data<void*>* data) = 0;
        [[nodiscard]] uint32_t get_size() const {return this->size;}
        I_shader_constant_container() = default;
    };


#define BEGIN_SHADER_CONST(Name) struct Name : public ::Engine::I_shader_constant_container {\
Name() : I_shader_constant_container() {this->m_buffer = ::Engine::Constant_buffer::Create(nullptr, this->get_size(), ::Engine::Vertex_buffer::Access_frequency::DYNAMIC, ::Engine::Vertex_buffer::Access_type::DRAW);} \
public: std::shared_ptr<Engine::Constant_buffer> m_buffer;\
public:
#define SET_SHADER_CONST(type, name) type name = this->set_arg<type>(#name); \
    void update_##name() {this->m_buffer->set_sub_data((void*)this->name.get_data(), this->name.get_offset(),this->name.stride());}

#define END_SHADER_CONST(Name) };

    /**
     * Governor class for CL_System_params
     */
//    class System_shader_constants {
//        static void test() {
//
////            params.write(nullptr);
//        }
//
//        static void request_param_lock() {
//        }
//    };
//
//
//    /**
//     * Test for float data governor.
//     */
//    class Float_governor : public Data_governor<Float_data> {
//        Float_governor(Float_data* data) : Data_governor<Float_data>(data){
//        }
//    };
//
//    class Hello {
//        void test() {
//            glm::mat4x4* data = (glm::mat4x4*)params.CL_Viewport.get_data();
//
//
//        }
//    };


}

