#pragma once

namespace Lab2 {
    struct Vertex {
        float x,y;
        Vertex(float x =0, float y = 0);
        static bool left(Vertex& a, Vertex& b, Vertex& c);
        static bool right(Vertex& a, Vertex& b, Vertex& c);
        static bool on(Vertex& a, Vertex& b, Vertex& c);
        Vertex operator+(Vertex& other) const;
        Vertex operator-(Vertex& other) const;
    //private:
        static double z_cross_product(Vertex& a, Vertex& b);
    };
}

