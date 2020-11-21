#pragma once
#include "Vertex.h"
#include "Search_tree.h"

namespace Lab2 {

    struct Triangle {
        Triangle(Vertex& a, Vertex& b, Vertex& c);
        void set_leaf(std::shared_ptr<Leaf<Triangle>> leaf);
        bool is_inside(Vertex& v);
        bool on_edge(Vertex& v);
        //Lab2::Vertex* v[3];
        Vertex& a;
        Vertex& b;
        Vertex& c;
        Triangle* ab = nullptr;
        Triangle* cb = nullptr;
        Triangle* ca = nullptr;
        std::shared_ptr<Leaf<Triangle>> leaf;

        //Leaf leaf;
    };
}