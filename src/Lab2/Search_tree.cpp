#include "Search_tree.h"
namespace Lab2 {

    uint32_t Node::insert(Lab2::Vertex *v, std::vector<std::shared_ptr<Triangle>> &dest, std::vector<Vertex> &vertices) {
        if (Vertex::left(this->cut.a, this->cut.b, *v)) {
            auto n_triangles = this->child_left->insert(v, dest, vertices);

            if (n_triangles == 2) {
                /// The child is a leaf. Reorganize the tree structure.
                auto original = std::static_pointer_cast<Leaf>(this->child_left);
                this->child_left = std::make_shared<Tri_node>(std::shared_ptr<I_node>(this), v, original->get_data()->a,
                                                              dest.at(dest.size() - 2)->a, dest.back()->a);

                auto child_pointer = std::static_pointer_cast<Tri_node>(this->child_left);
                /// The same
                child_pointer->child_A = original;
                child_pointer->child_A->parent = child_pointer;
                child_pointer->child_B = std::make_shared<Leaf>(child_pointer, dest.at(dest.size() - 2));
                child_pointer->child_C = std::make_shared<Leaf>(child_pointer, dest.back());
            } else if (n_triangles == 1) {
                auto original = std::static_pointer_cast<Leaf>(this->child_left);
                this->child_left = std::make_shared<Node>(std::shared_ptr<I_node>(this),
                                                          Line{*original->get_data()->a, *original->get_data()->b});
                auto child_pointer = std::static_pointer_cast<Node>(this->child_left);
                this->node_insert_helper(child_pointer, original, dest);
            }

            return 0;
        } else if (Vertex::right(this->cut.a, this->cut.b, *v)) {
            auto n_triangles = this->child_right->insert(v, dest, vertices);
            if (n_triangles == 2) {
                auto original = std::static_pointer_cast<Leaf>(this->child_right);
                this->child_right = std::make_shared<Tri_node>(std::shared_ptr<I_node>(this), v,
                                                               original->get_data()->a, dest.at(dest.size() - 2)->a,
                                                               dest.back()->a);

                auto child_pointer = std::static_pointer_cast<Tri_node>(this->child_right);
                /// The same
                child_pointer->child_A = original;
                child_pointer->child_A->parent = child_pointer;
                child_pointer->child_B = std::make_shared<Leaf>(child_pointer, dest.at(dest.size() - 2));
                child_pointer->child_C = std::make_shared<Leaf>(child_pointer, dest.back());
            } else if (n_triangles == 1) {
                auto original = std::static_pointer_cast<Leaf>(this->child_right);
                this->child_right = std::make_shared<Node>(std::shared_ptr<I_node>(this),
                                                           Line{*original->get_data()->a, *original->get_data()->b});
                auto child_pointer = std::static_pointer_cast<Node>(this->child_right);
                this->node_insert_helper(child_pointer, original, dest);
            }
            return 0;
        } else {
            auto left_triangles = this->child_left->insert(v, dest, vertices);
            auto right_triangles = this->child_right->insert(v, dest, vertices);

            if (left_triangles) {
                auto original = std::static_pointer_cast<Leaf>(this->child_left);
                this->child_left = std::make_shared<Node>(std::shared_ptr<I_node>(this),
                                                          Line{*original->get_data()->a, *original->get_data()->b});
                auto child_pointer = std::static_pointer_cast<Node>(this->child_left);
                this->node_insert_helper(child_pointer, original, dest);
            }

            if (right_triangles) {
                auto original = std::static_pointer_cast<Leaf>(this->child_right);
                this->child_right = std::make_shared<Node>(std::shared_ptr<I_node>(this),
                                                           Line{*original->get_data()->a, *original->get_data()->b});
                auto child_pointer = std::static_pointer_cast<Node>(this->child_right);
                this->node_insert_helper(child_pointer, original, dest);
            }
        }
        return 0;
    }

    void Node::node_insert_helper(std::shared_ptr<Node> child_pointer, std::shared_ptr<Leaf> original, std::vector<std::shared_ptr<Triangle>> &dest) {
        child_pointer->child_left = original;
        child_pointer->child_left->parent = child_pointer;
        child_pointer->child_right = std::make_shared<Leaf>(child_pointer, dest.back());
    }

    uint32_t Tri_node::insert(Vertex *v, std::vector<std::shared_ptr<Triangle>> &dest, std::vector<Vertex> &vertices) {

        /// For when the point is on a line.
        if (Vertex::on(*this->center, *this->point_AB, *v) || Vertex::on(*this->center, *this->point_CA, *v)) {
            /// On a line in A.
            auto n_triangles = this->child_A->insert(v, dest, vertices);
            if (n_triangles) {
                auto original = std::static_pointer_cast<Leaf>(this->child_A);
                this->child_A = std::make_shared<Node>(std::shared_ptr<I_node>(this),
                                                       Line{*original->get_data()->a, *original->get_data()->b});
                auto child_pointer = std::static_pointer_cast<Node>(this->child_A);
                /// The same
                child_pointer->child_left = original;
                child_pointer->child_left->parent = child_pointer;
                child_pointer->child_right = std::make_shared<Leaf>(child_pointer, dest.back());
            }
        }
        if (Vertex::on(*this->center, *this->point_BC, *v) || Vertex::on(*this->center, *this->point_CA, *v)) {
            /// On a line in B
            auto n_triangles = this->child_B->insert(v, dest, vertices);
            if (n_triangles) {
                auto original = std::static_pointer_cast<Leaf>(this->child_B);
                this->child_B = std::make_shared<Node>(std::shared_ptr<I_node>(this),
                                                       Line{*original->get_data()->a, *original->get_data()->b});
                auto child_pointer = std::static_pointer_cast<Node>(this->child_B);
                /// The same
                child_pointer->child_left = original;
                child_pointer->child_left->parent = child_pointer;
                child_pointer->child_right = std::make_shared<Leaf>(child_pointer, dest.back());
            }
        }
        if (Vertex::on(*this->center, *this->point_CA, *v) || Vertex::on(*this->center, *this->point_BC, *v)) {
            /// On a line in C
            auto n_triangles = this->child_C->insert(v, dest, vertices);
            if (n_triangles) {
                auto original = std::static_pointer_cast<Leaf>(this->child_C);
                this->child_C = std::make_shared<Node>(std::shared_ptr<I_node>(this),
                                                       Line{*original->get_data()->a, *original->get_data()->b});
                auto child_pointer = std::static_pointer_cast<Node>(this->child_C);
                /// The same
                child_pointer->child_left = original;
                child_pointer->child_left->parent = child_pointer;
                child_pointer->child_right = std::make_shared<Leaf>(child_pointer, dest.back());
            }
        }

        /// For when the point is inside a triangle.
        if (Vertex::left(*this->center, *this->point_AB, *v) && Vertex::right(*this->center, *this->point_BC, *v)) {
            /// Inside child B
            auto n_triangles = this->child_B->insert(v, dest, vertices);
            if (n_triangles) {
                auto original = std::static_pointer_cast<Leaf>(this->child_B);
                this->child_B = std::make_shared<Tri_node>(std::shared_ptr<I_node>(this), v, original->get_data()->a,
                                                           dest.at(dest.size() - 2)->a, dest.back()->a);
                auto child_pointer = std::static_pointer_cast<Tri_node>(this->child_B);
                /// The same
                child_pointer->child_A = original;
                child_pointer->child_A->parent = child_pointer;
                child_pointer->child_B = std::make_shared<Leaf>(child_pointer, dest.at(dest.size() - 2));
                child_pointer->child_C = std::make_shared<Leaf>(child_pointer, dest.back());
            }
        } else if (Vertex::left(*this->center, *this->point_BC, *v) &&
                   Vertex::right(*this->center, *this->point_CA, *v)) {
            /// Inside child C
            auto n_triangles = this->child_C->insert(v, dest, vertices);
            if (n_triangles) {
                auto original = std::static_pointer_cast<Leaf>(this->child_C);
                this->child_C = std::make_shared<Tri_node>(std::shared_ptr<I_node>(this), v, original->get_data()->a,
                                                           dest.at(dest.size() - 2)->a, dest.back()->a);
                auto child_pointer = std::static_pointer_cast<Tri_node>(this->child_C);
                /// The same
                child_pointer->child_A = original;
                child_pointer->child_A->parent = child_pointer;
                child_pointer->child_B = std::make_shared<Leaf>(child_pointer, dest.at(dest.size() - 2));
                child_pointer->child_C = std::make_shared<Leaf>(child_pointer, dest.back());
            }
        } else {
            /// Inside child A
            auto n_triangles = this->child_A->insert(v, dest, vertices);
            if (n_triangles) {
                auto original = std::static_pointer_cast<Leaf>(this->child_A);
                this->child_A = std::make_shared<Tri_node>(std::shared_ptr<I_node>(this), v, original->get_data()->a,
                                                           dest.at(dest.size() - 2)->a, dest.back()->a);
                auto child_pointer = std::static_pointer_cast<Tri_node>(this->child_A);
                /// The same
                child_pointer->child_A = original;
                child_pointer->child_A->parent = child_pointer;
                child_pointer->child_B = std::make_shared<Leaf>(child_pointer, dest.at(dest.size() - 2));
                child_pointer->child_C = std::make_shared<Leaf>(child_pointer, dest.back());
            }
        }


        return 0;
    }

    uint32_t Leaf::insert(Vertex *v, std::vector<std::shared_ptr<Triangle>> &dest, std::vector<Vertex> &vertices) {
        /// split data and return new triangles.

        Vertex *a = this->data->a;
        Vertex *b = this->data->b;
        Vertex *c = this->data->c;

        /// The following if statements will move the current triangle and append a new one to the list.
        /// The old triangle will always be on the left side of point a to v
        if (Vertex::on(*this->data->a, *this->data->b, *v)) {
            /// The new point is between a and b.
            /// New triangle: c a v
            /// Old triangle: c v b

            //dest.emplace_back( c,a,v);
            dest.push_back(std::make_shared<Triangle>(c, a, v));
            dest.back()->ab = this->data->ca;
            dest.back()->bc = this->data->ab;
            dest.back()->ca = this->data;

            this->data->a = c;
            this->data->c = b;
            this->data->b = v;

            auto bc = this->data->bc;
            this->data->bc = this->data->ab;
            this->data->ab = dest.back();
            this->data->ca = bc;
            return 1;
        } else if (Vertex::on(*this->data->b, *this->data->c, *v)) {
            /// The new point is between b and c.
            /// New triangle: a b c
            /// Old triangle: a v c

            //dest.emplace_back(a,b,c);
            dest.push_back(std::make_shared<Triangle>(a, b, v));
            dest.back()->ab = this->data->ab;
            dest.back()->bc = this->data->bc;
            dest.back()->ca = this->data;

            //this->data-> a = a;
            this->data->b = v;
            //this->data->c = c;

            this->data->ab = dest.back();
            //this->data->bc = this->data->bc;
            //this->data->ca = this->data->ca;

            return 1;
        } else if (Vertex::on(*this->data->c, *this->data->a, *v)) {
            /// The new point is between c and a.
            /// New triangle: b c v
            /// Old triangle: b v a

            //dest.emplace_back(b,c,v);
            dest.push_back(std::make_shared<Triangle>(b, c, v));
            dest.back()->ab = this->data->bc;
            dest.back()->bc = this->data->ca;
            dest.back()->ca = this->data;

            this->data->a = b;
            this->data->b = v;
            this->data->c = a;

            this->data->bc = this->data->ca;
            this->data->ca = this->data->ab;
            this->data->ab = dest.back();

            return 1;
        }

        /// Split into three triangles.
        /// New triangle 1: b v a
        /// New triangle 2: c v b
        /// Old triangle: a v c

        //dest.emplace_back(b,v,a);
        dest.push_back(std::make_shared<Triangle>(b, v, a));
        //dest.back().ab = 2;
        dest.back()->bc = this->data;
        dest.back()->ca = this->data->ab;

        //dest.emplace_back(c,v,b);
        dest.push_back(std::make_shared<Triangle>(c, v, b));
        dest.back()->ab = this->data;
        dest.back()->bc = dest.at(dest.size() - 2);
        dest.back()->ca = this->data->bc;

        dest.at(dest.size() - 2)->ab = dest.back();

        this->data->ab = dest.at(dest.size() - 2);
        this->data->bc = dest.back();

        this->data->b = v;

        return 2;
    }
}