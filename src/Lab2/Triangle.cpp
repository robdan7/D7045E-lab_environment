#include "Triangle.h"
#include "Search_tree.h"
namespace Lab2 {
    uint32_t created_IDs = 0;
    struct NULL_TRIANGLE : public Triangle {
        NULL_TRIANGLE() : Triangle(){}

        bool is_inside(const Vertex &v) override {
            return false;
        }
    };


    Triangle::Triangle():
        a(nullptr), b(nullptr), c(nullptr), ab(nullptr), bc(nullptr), ca(nullptr), triangle_ID(0) {}


    Triangle::Triangle(Vertex *a, Vertex *b, Vertex *c): a(a),b(b),c(c), triangle_ID(created_IDs++) {
        this->ab = std::make_shared<NULL_TRIANGLE>();
        this->bc = std::make_shared<NULL_TRIANGLE>();
        this->ca = std::make_shared<NULL_TRIANGLE>();
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
