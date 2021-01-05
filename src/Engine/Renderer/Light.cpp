#include "Light.h"
namespace Engine {
    /*
    std::shared_ptr<Directional_light> Directional_light::Create(const glm::vec3& color,const glm::vec4& position,bool cast_shadows) {
        return std::make_shared<GL_Directional_light>(color, position, cast_shadows);
    }
*/
    Directional_light::Directional_light(const glm::vec3& color,const glm::vec4& position,bool cast_shadow) : Light(color, cast_shadow), m_position(position){

    }

    glm::vec4 Directional_light::get_position() {
        return this->m_position;
    }
/*
    std::shared_ptr<Point_light> Point_light::Create(const glm::vec3& color,const glm::vec4& position, float radius, bool cast_shadows) {
        return std::make_shared<GL_Point_light>(color, position, radius, cast_shadows);
    }
*/
    Point_light::Point_light(const glm::vec3& color, const glm::vec4& position,bool radius,float exp_dropoff, bool cast_shadow) : Light(color, cast_shadow), radius(radius),exp_dropoff(exp_dropoff), m_position(position){

    }

    glm::vec4 Point_light::get_position() {
        return this->m_position;
    }

}
