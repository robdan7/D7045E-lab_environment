#pragma once
#include <vector>
#include "Vertex.h"
#include "Triangle.h"
#include <iterator>
namespace Lab2 {
    void sort(std::vector<Vertex>& list);
    std::vector<uint32_t> calc_convex_hull(std::vector<Vertex>& sorted_list);
    std::shared_ptr<Search_tree<Triangle>> build(std::vector<uint32_t>& convex_hull, std::vector<Vertex>& vertices, std::vector<Triangle>& triangle_dest);

    void split(std::shared_ptr<Search_tree<Triangle>>& tree, std::vector<uint32_t>& vertex_indices, std::vector<Vertex>& vertices, std::vector<Triangle>& triangles);
}