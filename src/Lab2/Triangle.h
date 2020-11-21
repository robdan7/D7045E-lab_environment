#pragma once
#include "Vertex.h"
#include "Search_tree.h"

namespace Lab2 {

    struct Triangle {
        Triangle(uint32_t& a, uint32_t& b, uint32_t& c);
        void set_leaf(std::shared_ptr<Leaf<Triangle>> leaf);
        //Lab2::Vertex* v[3];
        uint32_t& a;
        uint32_t& b;
        uint32_t& c;
        Triangle* ab;
        Triangle* cb;
        Triangle* ca;
        std::shared_ptr<Leaf<Triangle>> leaf;
        
        //Leaf leaf;
    };
}