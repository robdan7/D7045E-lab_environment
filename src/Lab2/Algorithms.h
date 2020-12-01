#pragma once
#include <vector>
#include "Vertex.h"
#include "Triangle.h"
#include <iterator>
#include "Search_tree.h"
namespace Lab2 {
    void sort(std::vector<Vertex>& list);
    std::vector<uint32_t> calc_convex_hull(std::vector<Vertex>& sorted_list);
    std::shared_ptr<Search_tree> build(std::vector<uint32_t>& convex_hull, std::vector<Vertex>& vertices, std::vector<std::shared_ptr<Triangle>>& triangle_dest);

    void split(std::shared_ptr<Search_tree> tree, std::vector<uint32_t>& convex_hull, std::vector<Vertex>& vertices, std::vector<std::shared_ptr<Triangle>>& triangles);

    struct Color {
        float r,g,b;
        bool operator ==(const Color* other) {
            return this->r == other->r && this->g == other->g && this->b == other->b;
        }
        bool operator == (const Color& other) {
            return this->r == other.r && this->g == other.g && this->b == other.b;
        }
    };

    bool color(std::shared_ptr<Triangle> node, std::vector<bool>& visited, std::vector<Color>& color_dest, Color* A, Color* B, Color* C, Color* self);

    void color_circle(std::shared_ptr<Triangle> node, std::vector<std::shared_ptr<Triangle>>& dest);
}