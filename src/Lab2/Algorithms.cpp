#include <algorithm>
#include <numeric>
#include "Algorithms.h"
#include "Search_tree.h"
#include "Line.h"
#include "Triangle.h"
namespace Lab2 {
    void internal_merge(std::vector<Vertex>* list, std::vector<Vertex>* destination, std::vector<Vertex>::iterator left, std::vector<Vertex>::iterator right, std::vector<Vertex>::iterator end) {
        auto i = left;
        auto j = right;
        for (auto k = left; k < end; ++k) {
            if (i < right && ((j >= end || i->x < j->x) || (i->x == j->x && i->y < j->y))) {
                destination->at(k-list->begin()) = Lab2::Vertex(i->x,i->y);
                i++;
            } else {
                auto test = Lab2::Vertex(j->x,j->y);
                destination->at(k-list->begin()) = Lab2::Vertex(j->x,j->y);
                j++;
            }
        }
    }

    void sort(std::vector<Vertex>& list) {
        std::vector<Vertex> destination(list.size());
        std::vector<Vertex>* internal_list = &list;
        std::vector<Vertex>* internal_work = &destination;
        bool swap = true;
        for (uint32_t width = 1; width < list.size(); width *=2) {
            for (std::vector<Vertex>::iterator i = internal_list->begin();i < internal_list->end(); i+=std::min(2*width,(uint32_t)(internal_list->end()-i))) {
                auto right = internal_list->end();
                auto end = internal_list->end();
                if ((int)(i-internal_list->begin())+width < internal_list->size()) {
                    right = i + width;
                }
                if ((int)(i-internal_list->begin())+2*width < internal_list->size()) {
                    end = i + 2*width;
                }
                internal_merge(internal_list,internal_work, i, right, end);
            }
            swap |= swap;
            /// Swap work list and reference list
            auto work_temp = internal_work;
            internal_work = internal_list;
            internal_list = work_temp;
        }
        if (swap) { /// The internal list is not pointing to the original list. Do a copy.
            //list.clear();
            //std::copy(internal_work->begin(),internal_work->end(),std::back_inserter(list));
            list.assign(internal_list->begin(),internal_list->end());
        }
    }

    std::vector<uint32_t> calc_convex_hull(std::vector<Vertex> &sorted_list) {
        std::vector<uint32_t> line_strip;
        for (int i = 0; i < sorted_list.size(); ++i) {
            while (line_strip.size() >= 2 && (Vertex::right(sorted_list[line_strip[line_strip.size()-2]], sorted_list[line_strip.back()], sorted_list[i]) || Vertex::on(sorted_list[line_strip[line_strip.size()-2]], sorted_list[line_strip.back()], sorted_list[i]))) {
                line_strip.pop_back();
            }
            line_strip.push_back(i);
        }

        uint32_t start_index = line_strip.size()-1;
        for (int i = sorted_list.size()-2; i >= 0; --i) {
            while (line_strip.size()-start_index >= 2 && (Vertex::right(sorted_list[line_strip[line_strip.size()-2]], sorted_list[line_strip.back()], sorted_list[i]) || Vertex::on(sorted_list[line_strip[line_strip.size()-2]], sorted_list[line_strip.back()], sorted_list[i]))) {
                line_strip.pop_back();
            }
            line_strip.push_back(i);
        }


        return std::move(line_strip);
    }

    std::shared_ptr<Search_tree> build_helper(std::vector<uint32_t>& hull,std::vector<Vertex>& vertices, std::vector<std::shared_ptr<Triangle>>& triangles, uint32_t start_index, uint32_t end_index) {
        if (end_index - start_index == 1) {
            /// Base case
            triangles.push_back(std::make_shared<Triangle>(&vertices[hull[0]], &vertices[hull[start_index]],&vertices[hull[end_index]]));

            auto leaf = std::make_shared<Leaf>(nullptr, triangles.back());
            triangles.back()->set_leaf(leaf);
            auto tree = std::make_shared<Search_tree>(std::static_pointer_cast<I_node>(leaf));
            return tree;
        }

        uint64_t split = ((uint64_t)start_index+(uint64_t)end_index)/2;
        if (split > UINT32_MAX) {
            throw std::exception("Whoops, something went wrong");
            exit(1);
        }

        auto index = triangles.size();
        auto right = build_helper(hull,vertices,triangles,start_index, split);
        auto lower_size = triangles.size()-index;
        auto left = build_helper(hull,vertices,triangles,split,end_index);
        //tree->set_right_tree(build_helper(hull,vertices,triangles,split,end_index));

        auto tree = std::make_shared<Search_tree>(left, right, Line{vertices[hull[0]],vertices[hull[split]]});


        triangles[lower_size-1]->ca = triangles[lower_size];
        triangles[lower_size]->ab = triangles[lower_size-1];

        return tree;
    }

    std::shared_ptr<Search_tree> build(std::vector<uint32_t>& convex_hull, std::vector<Vertex>& vertices, std::vector<std::shared_ptr<Triangle>>& triangle_dest) {
        return build_helper(convex_hull,vertices,  triangle_dest, 1, convex_hull.size()-2);
    }

    void split(std::shared_ptr<Search_tree> tree, std::vector<uint32_t>& convex_hull, std::vector<Vertex>& vertices, std::vector<std::shared_ptr<Triangle>>& triangles) {
        std::vector<uint32_t> inside_vertices(vertices.size());
        std::iota(inside_vertices.rbegin(), inside_vertices.rend(), 0);  /// Fill with increasing numbers


        uint32_t j = convex_hull.size()-2;
        uint32_t i = 0;
        while (j >= i) {
            if (convex_hull[j] < convex_hull[i]) {
                /// delete upper hull vertex
                inside_vertices[vertices.size()-1-convex_hull[j]] = inside_vertices.back();
                inside_vertices.pop_back();
                j--;
            } else {
                inside_vertices[vertices.size()-1-convex_hull[i]] = inside_vertices.back();
                inside_vertices.pop_back();
                i++;
            }
        }

        for (const auto& index : inside_vertices) {
            tree->insert(&vertices[index],triangles,vertices);
        }

    }

}
