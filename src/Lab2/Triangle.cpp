#include "Triangle.h"
namespace Lab2 {

    class NULL_TRIANGLE: public Triangle {

    };

    Triangle::Triangle(uint32_t& a, uint32_t& b, uint32_t& c): a(a), b(b), c(c) {
    }

    void Triangle::set_leaf(std::shared_ptr<Leaf<Triangle>> leaf) {
        this->leaf = leaf;
    }
}
