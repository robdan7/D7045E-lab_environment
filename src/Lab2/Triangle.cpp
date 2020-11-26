#include "Triangle.h"
#include "Search_tree.h"
namespace Lab2 {

    class NULL_TRIANGLE: public Triangle {

    };
    Triangle::Triangle(Vertex *a, Vertex *b, Vertex *c): a(a),b(b),c(c) {

    }

/*
    Triangle::Triangle(Vertex&& a, Vertex&& b, Vertex&& c){

    }
*/
    void Triangle::set_leaf(std::shared_ptr<Leaf> leaf) {
        this->leaf = leaf;
    }

    bool Triangle::is_inside(const Vertex &v) {

        return Vertex::left(*this->a,*this->b,v) && Vertex::left(*this->b,*this->c,v) && Vertex::left(*this->c,*this->a,v);
    }

    bool Triangle::on_edge(const Vertex &v) {
        return Vertex::on(*this->a,*this->b,v) || Vertex::on(*this->b,*this->c,v) || Vertex::on(*this->c,*this->a,v);
    }

}
