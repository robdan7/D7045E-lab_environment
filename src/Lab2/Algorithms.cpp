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

    void calc_convex_hull(std::vector<Vertex>& sorted_list, std::vector<uint32_t>& line_strip) {
        for (int i = 0; i < sorted_list.size(); ++i) {
            while (line_strip.size() >= 2 && (Vertex::right(sorted_list[line_strip[line_strip.size()-2]], sorted_list[line_strip.back()], sorted_list[i]) || Vertex::on(sorted_list[line_strip[line_strip.size()-2]], sorted_list[line_strip.back()], sorted_list[i]))) {
                line_strip.pop_back();
            }
            line_strip.push_back(i);
        }

        uint32_t start_index = line_strip.size()-1;
        for (int i = sorted_list.size()-2; i >= 0; --i) {
            while (line_strip.size()-start_index >= 2 && (Vertex::right(sorted_list[line_strip[line_strip.size()-2]], sorted_list[line_strip.back()], sorted_list[i])|| Vertex::on(sorted_list[line_strip[line_strip.size()-2]], sorted_list[line_strip.back()], sorted_list[i]))) {
                line_strip.pop_back();
            }
            line_strip.push_back(i);
        }
    }

    std::shared_ptr<Search_tree> build_helper(std::vector<uint32_t>& hull,std::vector<Vertex>& vertices, std::vector<std::shared_ptr<Triangle>>& triangles, uint32_t start_index, uint32_t end_index) {
        if (end_index - start_index == 1) {
            /// Base case
            triangles.push_back(std::make_shared<Triangle>(&vertices[hull[0]], &vertices[hull[start_index]],&vertices[hull[end_index]],triangles.size()));

            auto leaf = std::make_shared<Leaf>(nullptr, triangles.back());
            triangles.back()->set_leaf(leaf);

            /// Bind all fake triangle neighbours to the real triangle.
            triangles.back()->ab->ab = triangles.back();
            triangles.back()->bc->ab = triangles.back();
            triangles.back()->ca->ab = triangles.back();
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
        auto lower_size = triangles.size();
        auto left = build_helper(hull,vertices,triangles,split,end_index);
        //tree->set_right_tree(build_helper(hull,vertices,triangles,split,end_index));

        auto tree = std::make_shared<Search_tree>(left, right, Line{vertices[hull[0]],vertices[hull[split]]});


        triangles[lower_size-1]->ca = triangles[lower_size];
        triangles[lower_size]->ab = triangles[lower_size-1];

        /// These lines bind together the newly created fake triangles.
        triangles[lower_size-1]->bc->ca = triangles[lower_size]->bc;
        triangles[lower_size]->bc->bc = triangles[lower_size-1]->bc;

        return tree;
    }

    std::shared_ptr<Search_tree> build(std::vector<uint32_t>& convex_hull, std::vector<Vertex>& vertices, std::vector<std::shared_ptr<Triangle>>& triangle_dest) {
        auto result =  build_helper(convex_hull,vertices,  triangle_dest, 1, convex_hull.size()-2);

        /// The first and last added triangles have unconnected fake triangle neighbours.
        /// This will enclose the loop of fake triangles around the triangle fan.
        triangle_dest[0]->ab->ca = triangle_dest[0]->bc;
        triangle_dest[0]->bc->bc = triangle_dest[0]->ab;

        triangle_dest.back()->ca->bc = triangle_dest.back()->bc;
        triangle_dest.back()->bc->ca = triangle_dest.back()->ca;

        triangle_dest[0]->ab->bc = triangle_dest.back()->ca;
        triangle_dest.back()->ca->ca = triangle_dest[0]->ab;
        return result;
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

    void shift_color(Color*& A, Color*& B, Color*& C, Color*& D) {
        Color* A_temp = A;
        A=D;
        D=C;
        C=B;
        B=A_temp;
    }

    bool color(std::shared_ptr<Triangle> node, std::vector<bool>& visited, std::vector<Color> &color_dest, Color* A, Color* B, Color* C, Color* self) {
        if (!node->a && !node->b && !node->c) {
            return true;
        } else if (visited[node->triangle_ID]) {
            if (color_dest[node->triangle_ID] == A) {
                return false;
            }
            return true;
        }
        visited[node->triangle_ID] = !visited[node->triangle_ID];
        if (!color(node->ab,visited, color_dest,self,C,B,A)) {
            shift_color(A,B,C,self);
        }
        if (!color(node->bc,visited,color_dest,self,C,A,B)) {
            shift_color(A,B,C,self);
        }
        if (!color(node->ca,visited,color_dest,self,A,B,C)) {
            shift_color(A,B,C,self);
        }

        color_dest[node->triangle_ID] = *self;

        return true;
    }

    void color_circle(std::shared_ptr<Triangle> node, std::vector<std::shared_ptr<Triangle>>& dest) {
        dest.push_back(node);
        auto current_node = node->ab;
        Vertex* current_point = node->b;
        do {
            if (current_node->a && current_node->b && current_node->c) {
                dest.push_back(current_node);
            }

            if (current_node->a == current_point) {
                /// next side is ca or bc
                if (current_node->ca == node) {
                    current_point = current_node->c;
                    current_node = current_node->bc;
                } else {
                    current_node = current_node->ca;
                }
            } else if (current_node->b == current_point) {
                /// next side is ab or ca
                if (current_node->ab == node) {
                    current_point = current_node->a;
                    current_node = current_node->ca;
                } else {
                    current_node = current_node->ab;
                }
            } else {
                /// next side is bc or ab
                if (current_node->bc == node) {
                    current_point = current_node->b;
                    current_node = current_node->ab;
                } else {
                    current_node = current_node->bc;
                }
            }
        } while (current_node != node->ab);

    }

}
