#pragma once
#include "Vertex.h"


namespace Lab2 {
    class Leaf;
    struct Triangle {
        Triangle(Vertex* a, Vertex* b, Vertex* c);
        void set_leaf(std::shared_ptr<Leaf> leaf);
        bool is_inside(const Vertex& v);
        bool on_edge(const Vertex& v);

        Vertex* a;
        Vertex* b;
        Vertex* c;
        std::shared_ptr<Triangle> ab = nullptr;
        std::shared_ptr<Triangle> bc = nullptr;
        std::shared_ptr<Triangle> ca = nullptr;
        std::shared_ptr<Leaf> leaf;
    };
}