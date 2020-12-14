#include "GL_Frustum_compute_query.h"
#include <Platform/OpenGL/GL_shader.h>
#include <Platform/OpenGL/GL_vertex_array.h>
#include <Platform/OpenGL/OpenGL.h>
#include <iostream>
#include <fstream>
#include <sstream>
namespace Engine {

    GL_Frustum_compute_query::~GL_Frustum_compute_query() {

    }

    void GL_Frustum_compute_query::execute(uint32_t instances) {
        /**
         * This executes a transform feedback query. No rasterisation is performed here.
         */
        Frustum_compute_query::execute(instances);
        this->shader_program->bind();
        this->m_vao->bind();
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, this->m_TFO);

        glEnable(GL_RASTERIZER_DISCARD);
        glBeginTransformFeedback(GL_POINTS);
        glBeginQueryIndexed(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,1, m_last_frustum_query);
        glDrawArrays(GL_POINTS, 0, instances);
        glEndQueryIndexed(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,1);
        glEndTransformFeedback();
        glDisable(GL_RASTERIZER_DISCARD);

        glFlush();  // Make sure all commands are transported into the pipeline.
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

        this->m_vao->unbind();
        this->shader_program->unbind();
        this->m_has_updated = true;
    }

    GL_Frustum_compute_query::GL_Frustum_compute_query(std::shared_ptr<Vertex_buffer> data_buffer, std::shared_ptr<Vertex_buffer> instance_data)
            : Frustum_compute_query(data_buffer, instance_data) {

        // TODO Code cleanup. String concatenation results in unnecessary temporary strings.

        /**
         * This frustum culling query creates a passthrough vertex and geometry shader
         * with a filter for points that are not within the view frustum. Note that uniforms
         * are assumed to exist in a global space. This won't work unless view and projection
         * matrices are not defined.
         */

        std::string vertex_shader = R"(
            #version 440 core
            in vec4 in_position;
//            in float in_radius;

            out Vertex_data {
                vec4 position;
//                float radius;
            } vertex_data;

            uniform Uniform_block {
                mat4 view_matrix;
                mat4 projection_matrix;
            } my_block;

        )";

        std::vector<std::string> data(this->m_result_buffer->get_layout().get_elements().size());
        //std::string data[this->m_result_buffer->get_layout().get_elements().size()];
        std::vector<Buffer_element>::const_iterator iterator = this->m_result_buffer->get_layout().begin();
        int i = 0;
        while (iterator != this->m_result_buffer->get_layout().end()) {
            data[i] = iterator->m_string_representation;
            data[i] += " instance_" + iterator->m_name;
            vertex_shader += "in " + data[i] + ";\n";
            iterator++;
            i++;

        }
        vertex_shader += "out Instance_data {\n";
        for (int x = 0; x < i; x++) {
            vertex_shader += data[x] + ";\n";
        }
        vertex_shader += R"(
            } instance_output;
            void main() {
                vec4 view_position = my_block.view_matrix*vec4(in_position.x,in_position.y,in_position.z,1);
                vertex_data.position = my_block.projection_matrix *view_position;
                vertex_data.position /= vertex_data.position.w;
                vec4 radius = my_block.projection_matrix *vec4(in_position.w,0,view_position.z,1);
                vertex_data.position.w = radius.x/radius.w;     // radius


//                vertex_data.radius = in_radius;
            )";
        iterator = this->m_result_buffer->get_layout().begin();
        while (iterator != this->m_result_buffer->get_layout().end()) {
            vertex_shader += "instance_output.instance_" + iterator->m_name + " = instance_"+iterator->m_name + ";\n";
            iterator ++;
        }
        vertex_shader += "}";

        auto vertex_stage = std::make_shared<GL_shader_stage>(OPENGL_VERTEX_SHADER, std::move(vertex_shader));

        /// -------- Geometry shader ---------
        std::ostringstream geometry_2;
        std::string geom_data = R"(
            #version 440 core
            layout (invocations = 2, points) in;
            layout (points, max_vertices = 10) out;

            in Vertex_data {
                vec4 position;
            } vertex_data[];
        )";

        geom_data += "in Instance_data {\n";
        for (int x = 0; x < i; x++) {
            geom_data += data[x] + ";\n";
        }
        geom_data += "} instance_output[];\n";

        /// Create one output buffer for each stream/LOD.
        for(int buffers = 0; buffers < 4; buffers++) {
            std::string stream = std::to_string(buffers);
            geom_data += "layout(stream = "+stream+", xfb_buffer = "+stream+", xfb_offset=0) out Geometry_data_stream_"+stream+" {\n";
            for (int x = 0; x < i; x++) {
                geom_data += data[x] + ";\n";
            }
            geom_data += "} geometry_data_stream_"+std::to_string(buffers)+";\n";
        }
        geometry_2.write(geom_data.c_str(), geom_data.size());

        geom_data = R"(
            void main() {
//                for (int i = 0; i < gl_in.length(); i++) {
                    vec3 abs_position = abs(vertex_data[0].position.xyz)-vertex_data[0].position.w;
                    if (abs_position.z <= 1 && abs_position.x < 1 && abs_position.y < 1) {
            )";

        std::string stream = std::to_string(1);
        iterator = this->m_result_buffer->get_layout().begin();
        while (iterator != this->m_result_buffer->get_layout().end()) {
            geom_data += "geometry_data_stream_"+stream+".instance_" + iterator->m_name + " = instance_output[0].instance_"+iterator->m_name + ";\n";
            geom_data += "geometry_data_stream_1.instance_" + iterator->m_name + " = instance_output[0].instance_"+iterator->m_name + ";\n";
            iterator ++;
        }

        geom_data += R"(
                EmitStreamVertex(gl_InvocationID);
                EndStreamPrimitive(gl_InvocationID);
                }
            }
            )";
        geometry_2.write(geom_data.c_str(),geom_data.size());

        auto geometry_stage = std::make_shared<GL_shader_stage>(OPENGL_GEOMETRY_SHADER, std::move(geometry_2.str()));

        this->shader_program = std::unique_ptr<GL_shader>(new GL_shader({vertex_stage, geometry_stage}));
        this->shader_program->compile();
        this->shader_program->bind();

        GLuint index= glGetUniformBlockIndex(shader_program->get_ID(), "Uniform_block");
        glUniformBlockBinding(shader_program->get_ID(), index,0);

        this->m_vao = std::make_unique<GL_vertex_array>();
        this->m_vao->add_vertex_buffer(this->m_vertices, 0);
        this->m_vao->add_vertex_buffer(this->m_vertex_data, 0);

        glGenTransformFeedbacks(1, &this->m_TFO);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, this->m_TFO);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,1,this->m_result_buffer->get_ID());
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

        glGenQueries(1,&m_last_frustum_query);
    }

    uint32_t &GL_Frustum_compute_query::fetch_result() {
        if (this->m_has_updated) {
            GLuint result;
            glGetQueryObjectuiv(m_last_frustum_query, GL_QUERY_RESULT,&result);
            this->set_result(result);
            this->m_has_updated = false;
        }
        return Frustum_compute_query::fetch_result();
    }

}