#pragma once
#include <memory>
#include <functional>
#include "Triangle.h"
namespace Lab2 {
    template<class LEAF_TYPE>
    class I_comparator {
    public:
        template<typename FUNC>
        I_comparator(FUNC f) : f(f) {

        }

    private:
        std::function<void(LEAF_TYPE* a, LEAF_TYPE* b)> f;
    };
    template<class LEAF_TYPE>
    class Search_tree;

    template<class SEARCH_QUALIFIER>
    class Node;
    template<class LEAF_TYPE>
    class Leaf : public Node<LEAF_TYPE> {
    public:
        Leaf(std::shared_ptr<Node<LEAF_TYPE>> node,LEAF_TYPE* data) : data(data), Node<LEAF_TYPE>(node) {}
        Leaf() = default;

        const LEAF_TYPE* get_data() {
            return this->data;
        }

    private:
        LEAF_TYPE* data;
    };

    template<class LEAF_TYPE>
    class Node {
        friend class Search_tree<LEAF_TYPE>;
    public:
        explicit Node(std::shared_ptr<Node<LEAF_TYPE>> parent) : parent(parent) {}
        Node() = default;
    private:
        std::shared_ptr<Node> parent;
        std::vector<std::shared_ptr<Node>> children;
    private:
        std::vector<std::unique_ptr<I_comparator<LEAF_TYPE>>> m_comparator;
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
            root->children.push_back(tree->root);
            root->children.back()->parent = this->root;
        }

        void set_right_tree(std::shared_ptr<Search_tree<LEAF_TYPE>> tree) {
            root->children.push_back(tree->root);
            root->children.back()->parent = this->root;
        }

        //virtual LEAF_TYPE* find(const LEAF_TYPE& searchword) = 0;

    private:
        std::shared_ptr<Node<LEAF_TYPE>> root;
    };
}