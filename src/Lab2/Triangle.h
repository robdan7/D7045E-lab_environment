#pragma once
#include "Vertex.h"


namespace Lab2 {
    class Leaf;


    struct Triangle {
        Triangle(Vertex* a, Vertex* b, Vertex* c, uint32_t triangle_ID);
        void set_leaf(std::shared_ptr<Leaf> leaf);
        virtual bool is_inside(const Vertex& v);
        virtual bool on_edge(const Vertex& v);

        void update_reference(std::shared_ptr<Triangle> target, std::shared_ptr<Triangle> old) {
            if (this->ab == old) {
                this->ab = target;
            } else if(this->bc == old) {
                this->bc = target;
            } else if(this->ca == old) {
                this->ca = target;
            }
        }


        Vertex* a;
        Vertex* b;
        Vertex* c;
        std::shared_ptr<Triangle> ab = nullptr;
        std::shared_ptr<Triangle> bc = nullptr;
        std::shared_ptr<Triangle> ca = nullptr;
        std::shared_ptr<Leaf> leaf;
        const uint32_t triangle_ID;
    protected:
        Triangle(Vertex* a, Vertex* b) : a(a), b(b), c(nullptr), ab(nullptr), bc(nullptr), ca(nullptr), triangle_ID(0) {}
        //Triangle();
    };

}