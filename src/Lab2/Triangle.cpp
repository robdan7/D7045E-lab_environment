#include "Triangle.h"
namespace Lab2 {

    class NULL_TRIANGLE: public Triangle {

    };

    Triangle::Triangle(Vertex& a, Vertex& b, Vertex& c): a(a), b(b), c(c) {
    }

    void Triangle::set_leaf(std::shared_ptr<Leaf<Triangle>> leaf) {
        this->leaf = leaf;
    }

    bool Triangle::is_inside(Vertex &v) {

        return Vertex::left(this->a,this->b,v) && Vertex::left(this->b,this->c,v) && Vertex::left(this->c,this->a,v);
    }

    bool Triangle::on_edge(Vertex &v) {
        return Vertex::on(this->a,this->b,v) || Vertex::on(this->b,this->c,v) || Vertex::on(this->c,this->a,v);
    }
}
