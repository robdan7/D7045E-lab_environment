#include "Search_tree.h"
namespace Lab2 {

    void tri_node_helper(std::shared_ptr<Tri_node> child_pointer, std::shared_ptr<Leaf> original, std::vector<std::shared_ptr<Triangle>> &dest) {
        child_pointer->child_A = original;
        //child_pointer->child_A->parent = child_pointer;
        child_pointer->child_B = std::make_shared<Leaf>(child_pointer, dest.at(dest.size() - 2));
        child_pointer->child_C = std::make_shared<Leaf>(child_pointer, dest.back());
    }

    void node_helper(std::shared_ptr<Node> child_pointer, std::shared_ptr<Leaf> original, std::shared_ptr<Triangle> data) {
        child_pointer->child_left = original;
        //child_pointer->child_left->parent = child_pointer;
        child_pointer->child_right = std::make_shared<Leaf>(child_pointer, data);
    }


    uint32_t Node::insert(Lab2::Vertex *v, std::vector<std::shared_ptr<Triangle>> &dest, std::vector<Vertex> &vertices) {
        if (Vertex::left(this->cut.a, this->cut.b, *v)) {
            auto n_triangles = this->child_left->insert(v, dest, vertices);

            if (n_triangles == 2) { /// The child is a leaf. Reorganize the tree structure.
                auto original = std::static_pointer_cast<Leaf>(this->child_left);
                this->child_left = std::make_shared<Tri_node>(std::shared_ptr<I_node>(this), v, original->get_data()->a,dest.at(dest.size() - 2)->a, dest.back()->a);

                auto child_pointer = std::static_pointer_cast<Tri_node>(this->child_left);

                tri_node_helper(child_pointer,original,dest);
            } else if (n_triangles == 1) {
                auto original = std::static_pointer_cast<Leaf>(this->child_left);
                this->child_left = std::make_shared<Node>(std::shared_ptr<I_node>(this),Line{*original->get_data()->a, *original->get_data()->b});
                auto child_pointer = std::static_pointer_cast<Node>(this->child_left);
                node_helper(child_pointer, original, dest.back());
            }

            return 0;
        } else if (Vertex::right(this->cut.a, this->cut.b, *v)) {
            auto n_triangles = this->child_right->insert(v, dest, vertices);

            if (n_triangles == 2) { /// The child is a leaf. Reorganize the tree structure.
                auto original = std::static_pointer_cast<Leaf>(this->child_right);
                this->child_right = std::make_shared<Tri_node>(std::shared_ptr<I_node>(this), v, original->get_data()->a, dest.at(dest.size() - 2)->a, dest.back()->a);
                auto child_pointer = std::static_pointer_cast<Tri_node>(this->child_right);
                tri_node_helper(child_pointer,original,dest);
            } else if (n_triangles == 1) {
                auto original = std::static_pointer_cast<Leaf>(this->child_right);
                this->child_right = std::make_shared<Node>(std::shared_ptr<I_node>(this),
                                                           Line{*original->get_data()->a, *original->get_data()->b});
                auto child_pointer = std::static_pointer_cast<Node>(this->child_right);
                node_helper(child_pointer, original, dest.back());
            }
            return 0;
        } else {
            /// TODO Four triangles need to be put together here. Page 19 in the lab specification.

            auto left_triangles = this->child_left->insert(v, dest, vertices);
            auto right_triangles = this->child_right->insert(v, dest, vertices);

            if (left_triangles) {
                auto original = std::static_pointer_cast<Leaf>(this->child_left);
                this->child_left = std::make_shared<Node>(std::shared_ptr<I_node>(this),
                                                          Line{*original->get_data()->a, *original->get_data()->b});
                auto child_pointer = std::static_pointer_cast<Node>(this->child_left);
                node_helper(child_pointer, original, dest.at(dest.size()-2));
            }

            if (right_triangles) {
                auto original = std::static_pointer_cast<Leaf>(this->child_right);
                this->child_right = std::make_shared<Node>(std::shared_ptr<I_node>(this),
                                                           Line{*original->get_data()->a, *original->get_data()->b});
                auto child_pointer = std::static_pointer_cast<Node>(this->child_right);
                node_helper(child_pointer, original, dest.back());
            }
        }
        return 0;
    }

    std::shared_ptr<Triangle> Node::find(const Vertex &v) {
        std::shared_ptr<Triangle>  result = nullptr;
        if ((result = this->child_left->find(v)) != nullptr) {
            return result;
        }
        return this->child_right->find(v);
    }


    void stitch_fake_triangles(std::shared_ptr<Triangle> old_triangle, std::shared_ptr<Triangle> new_triangle,Vertex *v) {
        old_triangle->b = v;
        old_triangle->bc->ca = new_triangle;
        new_triangle->bc = old_triangle->bc;
        new_triangle->ca = old_triangle;
        old_triangle->bc = new_triangle;
    }

    uint32_t Tri_node::insert(Vertex *v, std::vector<std::shared_ptr<Triangle>> &dest, std::vector<Vertex> &vertices) {

        /// TODO Check if the point was on a line or not.
        /// For when the point is inside a triangle.
        if (Vertex::left(*this->center, *this->point_AB, *v) && Vertex::right(*this->center, *this->point_BC, *v)) {
            /// Inside child B
            auto n_triangles = this->child_B->insert(v, dest, vertices);
            if (n_triangles == 2) {
                auto original = std::static_pointer_cast<Leaf>(this->child_B);
                this->child_B = std::make_shared<Tri_node>(std::shared_ptr<I_node>(this), v, original->get_data()->a,
                                                           dest.at(dest.size() - 2)->a, dest.back()->a);
                auto child_pointer = std::static_pointer_cast<Tri_node>(this->child_B);
                /// The same
                tri_node_helper(child_pointer,original,dest);
            } else if (n_triangles == 1) {
                auto original = std::static_pointer_cast<Leaf>(this->child_B);
                this->child_B = std::make_shared<Node>(std::shared_ptr<I_node>(this),Line{*this->center, *v});
                auto child_pointer = std::static_pointer_cast<Node>(this->child_B);
                node_helper(child_pointer,original,dest.back());
            }
            return 0;
        } else if (Vertex::left(*this->center, *this->point_BC, *v) &&
                   Vertex::right(*this->center, *this->point_CA, *v)) {
            /// Inside child C
            auto n_triangles = this->child_C->insert(v, dest, vertices);
            if (n_triangles == 2) {
                auto original = std::static_pointer_cast<Leaf>(this->child_C);
                this->child_C = std::make_shared<Tri_node>(std::shared_ptr<I_node>(this), v, original->get_data()->a,
                                                           dest.at(dest.size() - 2)->a, dest.back()->a);
                auto child_pointer = std::static_pointer_cast<Tri_node>(this->child_C);
                /// The same
                tri_node_helper(child_pointer,original,dest);
            } else if (n_triangles == 1) {
                auto original = std::static_pointer_cast<Leaf>(this->child_C);
                this->child_C = std::make_shared<Node>(std::shared_ptr<I_node>(this),Line{*this->center, *v});
                auto child_pointer = std::static_pointer_cast<Node>(this->child_C);
                node_helper(child_pointer,original,dest.back());
            }
            return 0;
        } else if (Vertex::left(*this->center, *this->point_CA,*v) &&
                   Vertex::right(*this->center,*this->point_AB,*v)){
            /// Inside child A
            auto n_triangles = this->child_A->insert(v, dest, vertices);
            if (n_triangles == 2) {
                auto original = std::static_pointer_cast<Leaf>(this->child_A);
                this->child_A = std::make_shared<Tri_node>(std::shared_ptr<I_node>(this), v, original->get_data()->a,
                                                           dest.at(dest.size() - 2)->a, dest.back()->a);
                auto child_pointer = std::static_pointer_cast<Tri_node>(this->child_A);
                /// The same
                tri_node_helper(child_pointer,original,dest);
            } else if (n_triangles = 1) {
                auto original = std::static_pointer_cast<Leaf>(this->child_A);
                this->child_A = std::make_shared<Node>(std::shared_ptr<I_node>(this),Line{*this->center, *v});
                auto child_pointer = std::static_pointer_cast<Node>(this->child_A);
                node_helper(child_pointer,original,dest.back());
            }
            return 0;
        }

        /// For when the point is on a line.
        if (Vertex::on(*this->center, *this->point_AB, *v) || Vertex::on(*this->center, *this->point_CA, *v)) {
            /// On a line in A.
            auto n_triangles_A = this->child_A->insert(v, dest, vertices);
            if (n_triangles_A) {
                auto original = std::static_pointer_cast<Leaf>(this->child_A);
                this->child_A = std::make_shared<Node>(std::shared_ptr<I_node>(this),
                                                       Line{*original->get_data()->a, *original->get_data()->b});
                auto child_pointer = std::static_pointer_cast<Node>(this->child_A);
                /// The same
                node_helper(child_pointer, original, dest.back());
            }
        }
        if (Vertex::on(*this->center, *this->point_BC, *v) || Vertex::on(*this->center, *this->point_AB, *v)) {
            /// On a line in B
            auto n_triangles = this->child_B->insert(v, dest, vertices);
            if (n_triangles) {
                auto original = std::static_pointer_cast<Leaf>(this->child_B);
                this->child_B = std::make_shared<Node>(std::shared_ptr<I_node>(this),
                                                       Line{*original->get_data()->a, *original->get_data()->b});
                auto child_pointer = std::static_pointer_cast<Node>(this->child_B);
                /// The same
                node_helper(child_pointer, original, dest.back());

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
                node_helper(child_pointer, original, dest.back());
            }
        }

        return 0;
    }

    std::shared_ptr<Triangle> Tri_node::find(const Vertex &v) {
        std::shared_ptr<Triangle>  result = nullptr;
        if ((result = this->child_A->find(v)) != nullptr) {
            return result;
        } else if ((result = this->child_B->find(v)) != nullptr) {
            return result;
        }
        return this->child_C->find(v);
    }


    uint32_t Leaf::insert(Vertex *v, std::vector<std::shared_ptr<Triangle>> &dest, std::vector<Vertex> &vertices) {
        /// split data and return new triangles.

        Vertex *a = this->data->a;
        Vertex *b = this->data->b;
        Vertex *c = this->data->c;

        /// The following if statements will move the current triangle and append a new one to the list.
        /// The old triangle will always be on the left side of line a to v
        if (Vertex::on(*this->data->a, *this->data->b, *v)) {
            /// The new point is between a and b.
            /// New triangle: c a v
            /// Old triangle: c v b

            dest.push_back(std::make_shared<Triangle>(c, a, v,dest.size()));
            dest.back()->bc->ab = dest.back();
            dest.back()->ab = this->data->ca;
            dest.back()->ca = this->data;

            /// Reset old triangle vertices so it can be reused.
            this->data->a = c;
            this->data->c = b;
            this->data->b = v;

            this->data->ca->update_reference(dest.back(), this->data);
            auto bc = this->data->bc;
            this->data->bc = this->data->ab;
            if (!this->data->ab->c) {
                /// The triangle at ab is fake. Update it.
                stitch_fake_triangles(this->data->ab,dest.back()->bc,v);
            } else {
                /// The triangle(s) at ab are real.
                dest.back()->bc = this->data->ab;
                if (this->data->ab->b == v) {
                    /// The triangle at ab has already been split. We need to stitch the triangles together
                    this->data->ab->bc = dest.back();
                    this->data->bc = dest.at(dest.size()-2);
                }
            }

            this->data->ab = dest.back();
            this->data->ca = bc;
            return 1;
        } else if (Vertex::on(*this->data->b, *this->data->c, *v)) {
            /// The new point is between b and c.
            /// New triangle: a b v
            /// Old triangle: a v c

            dest.push_back(std::make_shared<Triangle>(a, b, v, dest.size()));
            dest.back()->bc->ab = dest.back();
            dest.back()->ab = this->data->ab;
            dest.back()->ca = this->data;

            /// Reset old triangle vertices so it can be reused.
            this->data->b = v;

            this->data->ab->update_reference(dest.back(), this->data);
            if (!this->data->bc->c) {
                /// The triangle at bc is fake. Update it.
                stitch_fake_triangles(this->data->bc,dest.back()->bc,v);
            } else {
                /// The triangle(s) at ab are real.
                dest.back()->bc = this->data->bc;
                if (this->data->bc->b == v) {
                    /// The triangle has already been split. We need to stitch the triangles together
                    this->data->bc->bc = dest.back();
                    this->data->bc = dest.at(dest.size()-2);
                }
            }

            this->data->ab = dest.back();
            return 1;
        } else if (Vertex::on(*this->data->c, *this->data->a, *v)) {
            /// The new point is between c and a.
            /// New triangle: b c v
            /// Old triangle: b v a

            dest.push_back(std::make_shared<Triangle>(b, c, v, dest.size()));
            dest.back()->bc->ab = dest.back();
            dest.back()->ab = this->data->bc;
            dest.back()->ca = this->data;

            /// Reset old triangle vertices so it can be reused.
            this->data->a = b;
            this->data->b = v;
            this->data->c = a;

            this->data->bc->update_reference(dest.back(), this->data);
            this->data->bc = this->data->ca;
            if (!this->data->ca->c) {
                /// The triangle at ca is fake. Update it.
                stitch_fake_triangles(this->data->ca,dest.back()->bc,v);
            } else {
                /// The triangle(s) at ab are real.
                dest.back()->bc = this->data->ca;
                if (this->data->ca->b == v && this->data->ca->c) {
                    /// The triangle has already been split. We need to stitch the triangles together
                    this->data->ca->bc = dest.back();
                    this->data->bc = dest.at(dest.size()-2);
                }
            }

            this->data->ca = this->data->ab;
            this->data->ab = dest.back();

            return 1;
        }

        /// Split into three triangles.
        /// New triangle 1: b v a
        /// New triangle 2: c v b
        /// Old triangle: a v c
        dest.push_back(std::make_shared<Triangle>(b, v, a, dest.size()));
        dest.back()->bc = this->data;
        dest.back()->ca = this->data->ab;

        dest.push_back(std::make_shared<Triangle>(c, v, b,dest.size()));
        dest.back()->ab = this->data;
        dest.back()->bc = dest.at(dest.size() - 2);
        dest.back()->ca = this->data->bc;

        dest.at(dest.size() - 2)->ab = dest.back();


        /// TODO Create a better way to update references.
        this->data->ab->update_reference(dest.at(dest.size()-2), std::shared_ptr<Triangle>(this->data));
        this->data->bc->update_reference(dest.back(), std::shared_ptr<Triangle>(this->data));

        this->data->ab = dest.at(dest.size() - 2);
        this->data->bc = dest.back();

        this->data->b = v;

        return 2;
    }

    std::shared_ptr<Triangle> Leaf::find(const Vertex &v) {
        if (Vertex::left(*this->data->a, *this->data->b,v) && Vertex::left(*this->data->b, *this->data->c, v) && Vertex::left(*this->data->c, *this->data->a, v)) {
            return this->data;
        }
        return nullptr;
    }
}