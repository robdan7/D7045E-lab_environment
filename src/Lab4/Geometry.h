#pragma once
#include <vector>
namespace Lab4 {
    void create_sphere(std::vector<float>& vertices, int segments);
    void create_prism(std::vector<float>& vertices, float height, float width, int segments);
    void create_pyramid(std::vector<float>& vertices, float height, float width, int segments);
}