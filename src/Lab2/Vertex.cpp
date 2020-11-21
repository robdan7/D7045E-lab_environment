#include "Vertex.h"

namespace Lab2 {

    Vertex::Vertex(float x, float y): x(x), y(y){

    }

    bool Vertex::left(Vertex &a, Vertex &b, Vertex &c) {
        Vertex first = b-a;
        Vertex second = c-a;
        return Vertex::z_cross_product(first,second) > 0;
    }

    bool Vertex::right(Vertex &a, Vertex &b, Vertex &c) {
        Vertex first = b-a;
        Vertex second = c-a;
        return Vertex::z_cross_product(first,second) < 0;
    }

    bool Vertex::on(Vertex &a, Vertex &b, Vertex &c) {
        Vertex first = b-a;
        Vertex second = c-a;
        return Vertex::z_cross_product(first,second) == 0;
    }

    double Vertex::z_cross_product(Vertex &a, Vertex &b) {
        /// Standard equation for the z value of a cross product in right-handed euclidean space
        /// Source: OpenGL SuperBible 7th edition p. 62
        return a.x*b.y-a.y*b.x;
    }

    Vertex Vertex::operator+(Vertex &other) const {
        return Vertex(this->x+other.x, this->y+other.y);
    }

    Vertex Vertex::operator-(Vertex &other) const {
        return Vertex(this->x-other.x,this->y-other.y);
    }
}