#include "Geometry.h"
#define PI (atan(1)*4)
namespace Lab4 {

    struct Vertex {
        float x,y,z;
        Vertex(float x, float y, float z) : x(x),y(y),z(z) {}
    };

    void sphere_helper(std::vector<float>& vertices, float phi, float z, bool up, float angle) {
        float phir = phi/180.0f*PI;
        for (float theta = -180; theta < 180; theta += angle) {
            float thetar = theta/180.0f*PI;
            float theta20 = (theta+angle)/180.0f*PI;

            glm::vec3 a(sin(thetar)*cos(phir),cos(thetar)*cos(phir),sin(phir));
            glm::vec3 c(sin(theta20)*cos(phir),cos(theta20)*cos(phir),sin(phir));


            vertices.push_back(a.x);
            vertices.push_back(a.y);
            vertices.push_back(a.z);
            vertices.push_back(a.x);
            vertices.push_back(a.y);
            vertices.push_back(a.z);

            if (up) {
                //vertices.emplace_back(0,0,z);
                vertices.push_back(0);
                vertices.push_back(0);
                vertices.push_back(z);
                vertices.push_back(0);
                vertices.push_back(0);
                vertices.push_back(z);
            }

            vertices.push_back(c.x);
            vertices.push_back(c.y);
            vertices.push_back(c.z);
            vertices.push_back(c.x);
            vertices.push_back(c.y);
            vertices.push_back(c.z);

            if (!up) {
                //vertices.emplace_back(0,0,z);
                vertices.push_back(0);
                vertices.push_back(0);
                vertices.push_back(z);
                vertices.push_back(0);
                vertices.push_back(0);
                vertices.push_back(z);
            }

        }
    }


    void create_sphere(std::vector<float>& vertices, int segments) {
        float angle = 360.0f/segments;
        //vertices.emplace_back(0,0,-1);
        sphere_helper(vertices,-90+angle,-1.0f,false,angle);
        for (float phi = -90+angle; phi < 90-angle; phi += angle) {
            float phir = phi/180.0f*PI;
            float phir20 = (phi+angle)/180.0f*PI;

            for (float theta = -180; theta <= 180; theta += angle) {
                float thetar = theta / 180.0f * PI;
                float theta20 = (theta + angle) / 180.0f * PI;

                glm::vec3 a(sin(thetar) * cos(phir), cos(thetar) * cos(phir), sin(phir));
                glm::vec3 b(sin(thetar) * cos(phir20), cos(thetar) * cos(phir20), sin(phir20));
                glm::vec3 c(sin(theta20) * cos(phir), cos(theta20) * cos(phir), sin(phir));
                glm::vec3 d(sin(theta20) * cos(phir20), cos(theta20) * cos(phir20), sin(phir20));
                glm::vec3 normal(0, 0, 0);


                vertices.push_back(a.x);
                vertices.push_back(a.y);
                vertices.push_back(a.z);
                vertices.push_back(a.x);
                vertices.push_back(a.y);
                vertices.push_back(a.z);

                vertices.push_back(b.x);
                vertices.push_back(b.y);
                vertices.push_back(b.z);
                vertices.push_back(b.x);
                vertices.push_back(b.y);
                vertices.push_back(b.z);

                vertices.push_back(c.x);
                vertices.push_back(c.y);
                vertices.push_back(c.z);
                vertices.push_back(c.x);
                vertices.push_back(c.y);
                vertices.push_back(c.z);

                vertices.push_back(b.x);
                vertices.push_back(b.y);
                vertices.push_back(b.z);
                vertices.push_back(b.x);
                vertices.push_back(b.y);
                vertices.push_back(b.z);

                vertices.push_back(d.x);
                vertices.push_back(d.y);
                vertices.push_back(d.z);
                vertices.push_back(d.x);
                vertices.push_back(d.y);
                vertices.push_back(d.z);

                vertices.push_back(c.x);
                vertices.push_back(c.y);
                vertices.push_back(c.z);
                vertices.push_back(c.x);
                vertices.push_back(c.y);
                vertices.push_back(c.z);
            }
        }
        sphere_helper(vertices,90+angle,1.0f,true,angle);
    }

    void create_prism(std::vector<float>& vertices, float height, float width, int segments) {
        float angle = 2*PI/segments;
        float w = width/2;
        float h = height/2;
        for (float phi = -PI; phi < PI; phi += angle) {
            float phi2 = phi + angle;
            /// top
            vertices.push_back(0);
            vertices.push_back(h);
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(1);
            vertices.push_back(0);

            vertices.push_back(w*sin(phi));
            vertices.push_back(h);
            vertices.push_back(w*cos(phi));
            vertices.push_back(0);
            vertices.push_back(1);
            vertices.push_back(0);

            vertices.push_back(w*sin(phi2));
            vertices.push_back(h);
            vertices.push_back(w*cos(phi2));
            vertices.push_back(0);
            vertices.push_back(1);
            vertices.push_back(0);

            /// circumference
            vertices.push_back(w*sin(phi));
            vertices.push_back(-h);
            vertices.push_back(w*cos(phi));
            vertices.push_back(sin(phi));   /// normal
            vertices.push_back(0);
            vertices.push_back(cos(phi));

            vertices.push_back(w*sin(phi2));
            vertices.push_back(-h);
            vertices.push_back(w*cos(phi2));
            vertices.push_back(sin(phi2));   /// normal
            vertices.push_back(0);
            vertices.push_back(cos(phi2));

            vertices.push_back(w*sin(phi));
            vertices.push_back(h);
            vertices.push_back(w*cos(phi));
            vertices.push_back(sin(phi));   /// normal
            vertices.push_back(0);
            vertices.push_back(cos(phi));



            vertices.push_back(w*sin(phi2));
            vertices.push_back(-h);
            vertices.push_back(w*cos(phi2));
            vertices.push_back(sin(phi2));   /// normal
            vertices.push_back(0);
            vertices.push_back(cos(phi2));

            vertices.push_back(w*sin(phi2));
            vertices.push_back(h);
            vertices.push_back(w*cos(phi2));
            vertices.push_back(sin(phi2));   /// normal
            vertices.push_back(0);
            vertices.push_back(cos(phi2));

            vertices.push_back(w*sin(phi));
            vertices.push_back(h);
            vertices.push_back(w*cos(phi));
            vertices.push_back(sin(phi));   /// normal
            vertices.push_back(0);
            vertices.push_back(cos(phi));

            /// bottom
            vertices.push_back(0);
            vertices.push_back(-h);
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(-1);
            vertices.push_back(0);

            vertices.push_back(w*sin(phi2));
            vertices.push_back(-h);
            vertices.push_back(w*cos(phi2));
            vertices.push_back(0);
            vertices.push_back(-1);
            vertices.push_back(0);

            vertices.push_back(w*sin(phi));
            vertices.push_back(-h);
            vertices.push_back(w*cos(phi));
            vertices.push_back(0);
            vertices.push_back(-1);
            vertices.push_back(0);
        }
    }

    void create_pyramid(std::vector<float>& vertices, float height, float width, int segments) {
        float angle = 2*PI/segments;
        float w = width/2;
        glm::vec3 old_norm = glm::vec3();
        glm::vec3 norm = glm::vec3();
        glm::vec3 new_norm = glm::vec3();
        {
            glm::vec3 a = glm::vec3 (w*(sin(-PI)-sin(-PI-angle)), 0,w*(cos(-PI)-cos(-PI-angle)));
            glm::vec3 b = glm::vec3 (-a.x,height,-a.z);
            old_norm = glm::cross(a,b);

            a = glm::vec3 (w*(sin(-PI+angle)-sin(-PI)), 0,w*(cos(-PI+angle)-cos(-PI)));
            b = glm::vec3 (-a.x,height,-a.z);
            norm = glm::cross(a,b);

        }
        for (float phi = - PI; phi < PI; phi += angle) {
            float phi2 = phi + angle;
            float phi3 = phi2 + angle;
            /// Bottom
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(-1);
            vertices.push_back(0);

            vertices.push_back(w*sin(phi2));
            vertices.push_back(0);
            vertices.push_back(w*cos(phi2));
            vertices.push_back(0);
            vertices.push_back(-1);
            vertices.push_back(0);

            vertices.push_back(w*sin(phi));
            vertices.push_back(0);
            vertices.push_back(w*cos(phi));
            vertices.push_back(0);
            vertices.push_back(-1);
            vertices.push_back(0);


            /// Top
            glm::vec3 a = glm::vec3 (w*(sin(phi3)-sin(phi2)), 0,w*(cos(phi3)-cos(phi2)));
            glm::vec3 b = glm::vec3 (-a.x,height,-a.z);
            new_norm = glm::cross(a,b);

            vertices.push_back(w*sin(phi));
            vertices.push_back(0);
            vertices.push_back(w*cos(phi));
            glm::vec3 norm_left = (old_norm+norm)/2.0f;
            vertices.push_back(norm_left.x);
            vertices.push_back(norm_left.y);
            vertices.push_back(norm_left.z);

            vertices.push_back(w*sin(phi2));
            vertices.push_back(0);
            vertices.push_back(w*cos(phi2));
            glm::vec3 norm_right = (norm+new_norm)/2.0f;
            vertices.push_back(norm_right.x);
            vertices.push_back(norm_right.y);
            vertices.push_back(norm_right.z);

            vertices.push_back(0);
            vertices.push_back(height);
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(1);
            vertices.push_back(0);

            old_norm = norm;
            norm = new_norm;
        }
    }

}