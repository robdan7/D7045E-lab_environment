#pragma once
#include <glm/glm.hpp>
namespace Engine {
    class Light {
    public:
        //virtual void on_render() {};
        //virtual void on_update() {};
        virtual bool cast_whadows() {return this->cast_shadow;}
        glm::vec3 get_color() {return this->m_color;}
        void set_color(const glm::vec3& color) {this->m_color = color;}
        virtual glm::vec4 get_position() = 0;
    protected:
        Light(const glm::vec3& color, bool cast_shadow = false) : cast_shadow(cast_shadow), m_color(color){}
        glm::vec3 m_color;
        bool cast_shadow;
    };

    class Directional_light : public Light {
    public:
        //static std::shared_ptr<Directional_light> Create(const glm::vec3& color,const glm::vec4& position,bool cast_shadows = false);

        glm::vec4 get_position() override;

    protected:
        Directional_light(const glm::vec3& color, const glm::vec4& position, bool cast_shadow);

    protected:
        glm::vec4 m_position;
    };



    class Point_light : public Light {
    public:
        glm::vec4 get_position() override;


    public:
        //static std::shared_ptr<Point_light> Create(const glm::vec3& color,const glm::vec4& position, float radius = 0, bool cast_shadows = false);

    protected:
        Point_light(const glm::vec3& color, const glm::vec4& position,bool radius,float exp_dropoff, bool cast_shadow);

    protected:
        float radius,exp_dropoff;
        glm::vec4 m_position;
    };

}
