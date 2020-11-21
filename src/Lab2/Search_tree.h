#pragma once
#include <memory>
namespace Lab2 {
    template<class LEAF_TYPE>
    class Node;
    template<class LEAF_TYPE>
    class Leaf : public Node<LEAF_TYPE> {
    public:
        Leaf(std::shared_ptr<Node<LEAF_TYPE>> node,LEAF_TYPE* data) : data(data), Node<LEAF_TYPE>(node) {}
        Leaf() = default;

        const LEAF_TYPE* get_data() override {
            return this->data;
        }

    private:
        LEAF_TYPE* data;
    };

    template<class LEAF_TYPE>
    class Search_tree;

    template<class LEAF_TYPE>
    class Node {
        friend class Search_tree<LEAF_TYPE>;
    public:
        explicit Node(std::shared_ptr<Node<LEAF_TYPE>> parent) : parent(parent) {}
        Node() = default;
        virtual const LEAF_TYPE* get_data() {return nullptr;}
    private:
        std::shared_ptr<Node<LEAF_TYPE>> parent;
        std::shared_ptr<Node<LEAF_TYPE>> left, right;
    };

    template<class LEAF_TYPE>
    class Search_tree {
    public:
        template<typename ARGS>
        explicit Search_tree(ARGS data...) {
            this->root = std::make_shared<Leaf<LEAF_TYPE>>(data);
        }

        /**
         * Create a search tree with a single entry defined by the user.
         * @param root
         */
        explicit Search_tree(std::shared_ptr<Leaf<LEAF_TYPE>> root) {
            this->root = root;
        }

        Search_tree() {
            this->root = std::make_shared<Node<LEAF_TYPE>>();
        }
        void set_left_tree(std::shared_ptr<Search_tree<LEAF_TYPE>> tree) {
            this->root->left = tree->root;
            this->root->left->parent = this->root;
        }

        void set_right_tree(std::shared_ptr<Search_tree<LEAF_TYPE>> tree) {
            this->root->right = tree->root;
            this->root->right->parent = this->root;
        }

    private:
        std::shared_ptr<Node<LEAF_TYPE>> root;
    };
}