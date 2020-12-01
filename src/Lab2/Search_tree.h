#pragma once
#include <memory>
#include <functional>
#include <utility>
#include "Line.h"
#include "Triangle.h"
namespace Lab2 {
    class Search_tree;
    class Node;
    class Tri_node;

    class I_node {
        friend Search_tree;
        friend Tri_node;
        friend Node;
    public:
        explicit I_node(std::shared_ptr<I_node> parent) : parent(std::move(parent))  {}
        I_node() : parent(nullptr) {}
        virtual std::shared_ptr<Triangle>  find(const Vertex& v) = 0;
        /**
         * Insert a node into the tree
         * @param v
         * @param dest
         * @return How many triangles that were created, i.e. the leaf node was reached and the tre must be restructured.
         */
        virtual uint32_t insert(Vertex* v, std::vector<std::shared_ptr<Triangle>>& dest, std::vector<Vertex>& vertices) = 0;
    private:
        std::shared_ptr<I_node> parent;
    };

    class Leaf : public I_node {
    public:
        Leaf(const std::shared_ptr<I_node>& parent,std::shared_ptr<Triangle> data) : data(data), I_node(parent) {
            this->test = true;
        }

        uint32_t insert(Vertex* v, std::vector<std::shared_ptr<Triangle>>& dest, std::vector<Vertex>& vertices) override;

        std::shared_ptr<Triangle> get_data() {
            return this->data;
        }

        std::shared_ptr<Triangle> find(const Vertex &v) override;

    private:
        std::shared_ptr<Triangle> data;
        bool test = false;
    };

    class Tri_node : public I_node {
        friend class Search_tree;
        friend Node;
    public:
        std::shared_ptr<Triangle> find(const Vertex &v) override;

        explicit Tri_node(std::shared_ptr<I_node> parent,Vertex* center, Vertex* AB, Vertex* BC, Vertex* CA) : I_node(std::move(parent)),center(center),point_AB(AB),point_BC(BC),point_CA(CA) {}
        Tri_node(Vertex* center, Vertex* AB, Vertex* BC, Vertex* CA) : I_node(),center(center),point_AB(AB),point_BC(BC),point_CA(CA) {}
        uint32_t insert(Vertex* v, std::vector<std::shared_ptr<Triangle>>& dest, std::vector<Vertex>& vertices) override;

    private:
        friend void tri_node_helper(std::shared_ptr<Tri_node> child_pointer, std::shared_ptr<Leaf> original, std::vector<std::shared_ptr<Triangle>> &dest);
    private:
        std::shared_ptr<I_node> child_A,child_B,child_C;
        Vertex* center;
        Vertex* point_AB;
        Vertex* point_BC;
        Vertex* point_CA;
    };

    class Node : public I_node{
        friend Search_tree;
        friend Tri_node;
        //friend Node;
    public:
        std::shared_ptr<Triangle> find(const Vertex &v) override;

        explicit Node(std::shared_ptr<I_node> parent, Line cut) : I_node(std::move(parent)), cut(cut) {}
        Node( Line cut) : I_node(), cut(cut) {}

        uint32_t insert(Vertex* v, std::vector<std::shared_ptr<Triangle>>& dest, std::vector<Vertex>& vertices) override;

    private:
        friend void node_helper(std::shared_ptr<Node> child_pointer, std::shared_ptr<Leaf> original, std::vector<std::shared_ptr<Triangle>> &dest);
    private:
        std::shared_ptr<I_node> child_left,child_right;
        Line cut;
    };

    class Search_tree {
    public:
        explicit Search_tree(std::shared_ptr<I_node> root) {
            try {
                auto pointer = std::static_pointer_cast<Leaf>(root);
                std::cout << "" << std::endl;
            } catch (std::exception e) {

            }
            auto pointer = std::static_pointer_cast<Leaf>(root);
            this->root = root;
        }

        Search_tree(const std::shared_ptr<Search_tree> tree_left, const std::shared_ptr<Search_tree> tree_right, Line divider) {
            this->root = std::make_shared<Node>(divider);
            std::static_pointer_cast<Node>(this->root)->child_left = tree_left->root;
            tree_left->root->parent = this->root;
            std::static_pointer_cast<Node>(this->root)->child_right = tree_right->root;
            tree_right->root->parent = this->root;
        }

        void insert(Vertex* v, std::vector<std::shared_ptr<Triangle>>& dest, std::vector<Vertex>& vertices) {
            auto n_triangles = this->root->insert(v,dest,vertices);

            /// In case the tree only has one node, create a new root and put the triangles in it.
            if (n_triangles == 1){
                auto original = std::static_pointer_cast<Leaf>(this->root);

                this->root = std::make_shared<Node>(Line{*original->get_data()->a, *original->get_data()->b});
                auto root_pointer = std::static_pointer_cast<Node>(this->root);
                root_pointer->child_left = original;
                root_pointer->child_left->parent = root_pointer;
                root_pointer->child_right = std::make_shared<Leaf>(this->root, dest.back());
            } else if (n_triangles == 2) {
                auto original = std::static_pointer_cast<Leaf>(this->root);
                this->root = std::make_shared<Tri_node>(v, original->get_data()->a, dest.at(dest.size()-2)->a, dest.back()->a);
                auto root_pointer = std::static_pointer_cast<Tri_node>(this->root);
                root_pointer->child_A = original;
                root_pointer->child_A->parent = root_pointer;
                root_pointer->child_B = std::make_shared<Leaf>(root_pointer, dest.at(dest.size() - 2));
                root_pointer->child_C = std::make_shared<Leaf>(root_pointer, dest.back());
            }
            //this->root->insert(node);
        }

        std::shared_ptr<Triangle> search(Vertex& v) {
            return this->root->find(v);
        }


        //virtual LEAF_TYPE* find(const LEAF_TYPE& searchword) = 0;

    private:
        std::shared_ptr<I_node> root;
    };
}