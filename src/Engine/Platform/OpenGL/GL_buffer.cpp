//
// Created by Robin on 2020-04-03.
//
#include <fstream>
#include "GL_buffer.h"
#include "OpenGL.h"
namespace Cecilion {
    uint32_t GL_constant_buffer::bindings = 0;

    class GL_Raw_buffer {
    public:

        static uint32_t calc_draw_type(Raw_buffer::Access_frequency frequency, Raw_buffer::Access_type type) {
            uint32_t draw_type = 0;
            // TODO there must be a better way of doing this and still make it dynamic and generic.
            switch(frequency) {
                case Raw_buffer::Access_frequency::STATIC:
                    switch (type) {
                        case Raw_buffer::Access_type::DRAW:
                            draw_type = GL_STATIC_DRAW;
                            break;
                        case Raw_buffer::Access_type::READ:
                            draw_type = GL_STATIC_READ;
                            break;
                        case Raw_buffer::Access_type::COPY:
                            draw_type = GL_STATIC_COPY;
                            break;
                    }
                    break;
                case Raw_buffer::Access_frequency::DYNAMIC:
                    switch (type) {
                        case Raw_buffer::Access_type::DRAW:
                            draw_type = GL_DYNAMIC_DRAW;
                            break;
                        case Raw_buffer::Access_type::READ:
                            draw_type = GL_DYNAMIC_READ;
                            break;
                        case Raw_buffer::Access_type::COPY:
                            draw_type = GL_DYNAMIC_COPY;
                            break;
                    }
                    break;
                case Raw_buffer::Access_frequency::STREAM:
                    switch (type) {
                        case Raw_buffer::Access_type::DRAW:
                            draw_type = GL_STREAM_DRAW;
                            break;
                        case Raw_buffer::Access_type::READ:
                            draw_type = GL_STREAM_READ;
                            break;
                        case Raw_buffer::Access_type::COPY:
                            draw_type = GL_STREAM_COPY;
                            break;
                    }
                    break;
            }
            return draw_type;
        }

        static uint32_t init(void *vertices, uint32_t size, uint32_t draw_type, uint32_t gl_type) {
            uint32_t ID = 0;
            glCreateBuffers(1, &ID);
            glBindBuffer(gl_type, reinterpret_cast<GLuint>(ID));
            glBufferData(gl_type, size, vertices, draw_type);
            glBindBuffer(gl_type, 0);
            return ID;
        }

        static void bind(uint32_t type, uint32_t ID) {
            glBindBuffer(type, reinterpret_cast<GLuint>(ID));
        }

        static void unbind(uint32_t type) {
            glBindBuffer(type, 0);
        }

        static void reset_buffer(uint32_t type, uint32_t ID, void *vertices, uint32_t size, uint32_t draw_type) {
            glBindBuffer(type, reinterpret_cast<GLuint>(ID));
            glBufferData(type, size, vertices, draw_type);
            glBindBuffer(type,0);
//        Raw_buffer::set_size(size);
        }

        static void set_sub_data(uint32_t type, uint32_t ID, float *vertices, uint32_t offset, uint32_t size) {
            glBindBuffer(type, reinterpret_cast<GLuint>(ID));
            glBufferSubData(type, offset, size, vertices);
            glBindBuffer(type,0);
        }

        static void resize_buffer(uint32_t type, uint32_t ID, uint32_t old_size, uint32_t new_size, uint32_t draw_type) {

            glBindBuffer(GL_COPY_READ_BUFFER, reinterpret_cast<GLuint>(ID));
            uint8_t data[new_size];
            glGetBufferSubData(GL_COPY_READ_BUFFER, 0, old_size < new_size ? old_size : new_size, &data);
            reset_buffer(type, ID, (float*)data, new_size, draw_type);

            glBindBuffer(GL_COPY_READ_BUFFER, 0);
        }

        static void delete_buffer(uint32_t ID) {
            glDeleteBuffers(1, reinterpret_cast<const GLuint *>(&ID));
        }

        static void dump_to_file(const char* file, uint32_t ID, uint32_t size) {
            glBindBuffer(GL_COPY_READ_BUFFER, reinterpret_cast<GLuint>(ID));
            char data[size];
            glGetBufferSubData(GL_COPY_READ_BUFFER, 0, size, &data);

            /// Get transform feedback primitives.
            std::ofstream output_file;
            output_file.open(file);
            output_file.write(data, size);
            output_file.close();
        }

        static void copy_buffer_sub_data(uint32_t ID, const uint32_t old_index, const uint32_t new_index, const uint32_t byte_size) {
            glCopyNamedBufferSubData(ID,ID,old_index,new_index,byte_size);
        }
    };

    /// -------------- Vertex buffer --------------

    GL_vertex_buffer::GL_vertex_buffer(void* vertices, uint32_t size, Access_frequency frequency, Access_type type) : Vertex_buffer(size) {
        this->m_GL_draw_type = GL_Raw_buffer::calc_draw_type(frequency, type);
        this->m_buffer_ID = GL_Raw_buffer::init(vertices, size, this->m_GL_draw_type, GL_ARRAY_BUFFER);
        this->set_size(size);
    }


    void GL_vertex_buffer::bind() {
        GL_Raw_buffer::bind(GL_ARRAY_BUFFER, this->m_buffer_ID);
    }

    void GL_vertex_buffer::unbind() {
        GL_Raw_buffer::unbind(GL_ARRAY_BUFFER);
    }

    GL_vertex_buffer::~GL_vertex_buffer() {
        GL_Raw_buffer::delete_buffer(this->m_buffer_ID);
    }

    int GL_vertex_buffer::get_instance_divisor() {
        return this->m_divisor;
    }

    void GL_vertex_buffer::set_instance_divisor(int divisor) {
        this->m_divisor = divisor;
    }

    void GL_vertex_buffer::reset_buffer(float *vertices, uint32_t size) {
        GL_Raw_buffer::reset_buffer(GL_ARRAY_BUFFER, this->m_buffer_ID, vertices, size, this->m_GL_draw_type);
        this->set_size(size);
    }

    void GL_vertex_buffer::set_sub_data(float *vertices, uint32_t offset,uint32_t size) {
        GL_Raw_buffer::set_sub_data(GL_ARRAY_BUFFER,this->m_buffer_ID, vertices, offset, size);
    }

    void GL_vertex_buffer::resize_buffer(uint32_t size) {
        GL_Raw_buffer::resize_buffer(GL_ARRAY_BUFFER, this->m_buffer_ID, this->get_size(), size,this->m_GL_draw_type);
        this->set_size(size);
    }

    void GL_vertex_buffer::dump_to_file(const char *file) {
        GL_Raw_buffer::dump_to_file(file, this->get_ID(), this->get_size());
    }

    void GL_vertex_buffer::copy_sub_data(uint32_t old_index, uint32_t new_index, uint32_t data_size) {
        GL_Raw_buffer::copy_buffer_sub_data(this->m_buffer_ID,old_index, new_index, data_size);
    }

    /// -------------- Index buffer --------------

    GL_index_buffer::GL_index_buffer(uint32_t *indices, uint32_t count, Raw_buffer::Access_frequency frequency) : m_count(count) {
        auto draw_type = GL_Raw_buffer::calc_draw_type(frequency, Raw_buffer::Access_type::DRAW);
        /*glCreateBuffers(1, &this->m_buffer_ID);
        this->bind();
        /// TODO Enable index buffer layouts.
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
         */
        this->m_buffer_ID = GL_Raw_buffer::init(indices, count, draw_type, GL_ARRAY_BUFFER);
    }

    GL_index_buffer::~GL_index_buffer() {
        glDeleteBuffers(1, &this->m_buffer_ID);
    }

    void GL_index_buffer::bind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, reinterpret_cast<GLuint>(this->m_buffer_ID));
    }

    void GL_index_buffer::unbind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void GL_index_buffer::reset_buffer(uint32_t *vertices, uint32_t size) {
        GL_Raw_buffer::reset_buffer(GL_ELEMENT_ARRAY_BUFFER, this->m_buffer_ID, vertices, size, GL_STATIC_DRAW);
        this->m_count = size;
    }


    /// -------------- Uniform buffer --------------

    void GL_constant_buffer::bind() {
        GL_Raw_buffer::bind(GL_UNIFORM_BUFFER, this->m_buffer_ID);
    }

    void GL_constant_buffer::unbind() {
        GL_Raw_buffer::unbind(GL_UNIFORM_BUFFER);
    }

    void GL_constant_buffer::reset_buffer(float *vertices, uint32_t size) {
        GL_Raw_buffer::reset_buffer(GL_UNIFORM_BUFFER, this->m_buffer_ID, vertices, size, this->m_GL_draw_type);
        this->set_size(size);
    }

    void GL_constant_buffer::set_sub_data(float *vertices, uint32_t offset, uint32_t size) {
        GL_Raw_buffer::set_sub_data(GL_UNIFORM_BUFFER,this->m_buffer_ID, vertices, offset, size);
    }

    void GL_constant_buffer::resize_buffer(uint32_t size) {
        GL_Raw_buffer::resize_buffer(GL_UNIFORM_BUFFER, this->m_buffer_ID, this->get_size(), size,this->m_GL_draw_type);
        this->set_size(size);
    }

    GL_constant_buffer::GL_constant_buffer(void *vertices, uint32_t size,
                                           Raw_buffer::Access_frequency frequency,
                                           Raw_buffer::Access_type type) : Constant_buffer(size){
        this->m_GL_draw_type = GL_Raw_buffer::calc_draw_type(frequency, type);
        this->m_buffer_ID = GL_Raw_buffer::init(vertices, size, this->m_GL_draw_type, GL_UNIFORM_BUFFER);
        this->m_binding_point = bindings++;
        glBindBufferBase(GL_UNIFORM_BUFFER, this->m_binding_point, this->m_buffer_ID);
    }

    GL_constant_buffer::GL_constant_buffer(uint32_t size,
                                           Raw_buffer::Access_frequency frequency,
                                           Raw_buffer::Access_type type) :
            GL_constant_buffer(nullptr, size, frequency, type) {
    }

    void GL_constant_buffer::dump_to_file(const char *file) {
        GL_Raw_buffer::dump_to_file(file, this->get_ID(), this->get_size());
    }

    void GL_constant_buffer::copy_sub_data(uint32_t old_index, uint32_t new_index, uint32_t data_size) {
        GL_Raw_buffer::copy_buffer_sub_data(this->m_buffer_ID,old_index, new_index, data_size);
    }


}

