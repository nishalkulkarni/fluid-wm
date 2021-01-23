#include "bsp.hpp"
#include "fluid_wm.hpp"
#include <iostream>

Node::Node()
{
    left = nullptr;
    right = nullptr;
}

void Node::set_dimensions(int w, int h)
{
    this->width = w;
    this->height = h;
}

void Node::set_window(Window w)
{
    this->window = w;
}

void Node::set_children(Node* l, Node* r)
{
    this->left = l;
    this->right = r;
}

std::pair<int, int> Node::get_dimensions()
{
    return { this->width, this->height };
}

Window Node::get_window()
{
    return this->window;
}

Node* Node::get_left_child()
{
    return this->left;
}

Node* Node::get_right_child()
{
    return this->right;
}

bool Node::isLeaf()
{
    return (left == nullptr && right == nullptr);
}

void Node::print()
{
    std::cout << window << " -> " << width << " -> " << height << std::endl;
}

void Node::split_node(Window new_window)
{
    Node* left_child = new Node();
    Node* right_child = new Node();

    left_child->set_window(this->window);
    right_child->set_window(new_window);
    /* this->window = NULL; */

    int split_width = this->width / 2;
    int split_height = this->height / 2;
    left_child->set_dimensions(split_width, split_height);
    right_child->set_dimensions(this->width - split_width, this->height - split_height);

    set_children(left_child, right_child);
}

BSP::BSP()
{
    root = nullptr;
}

void BSP::init_root(Window w, int width, int height)
{
    root = new Node();
    root->set_window(w);
    root->set_dimensions(width, height);
}

Node* BSP::find_node_util(Window w, Node* current)
{
    /* if (current->get_window() == w && current->isLeaf()) { */
    /*     return current; */
    /* } */
    if (current->isLeaf()) {
        return current;
    }

    if (current->get_right_child()->isLeaf() == false) {
        return find_node_util(w, current->get_right_child());
    } else {
        return current->get_right_child();
    }

    if (current->get_left_child()->isLeaf() == false) {
        return find_node_util(w, current->get_left_child());
    } else {
        return current->get_left_child();
    }

    return nullptr;
}

Node* BSP::find_node(Window w)
{
    if (root == nullptr) {
        std::cout << "No root window present\n";
    }
    Node* res = find_node_util(w, root);
    if (res == nullptr) {
        std::cout << "Window not found\n";
    }
    return res;
}

void BSP::create_node(Window w)
{
    Node* current = find_node(w);
    if (current == nullptr) {
        std::cout << "Cannot create new window\n";
    } else {
        current->split_node(w);
    }
}

void BSP::display_tree_util(Node* current)
{
    if (current->get_left_child() != nullptr) {
        display_tree_util(current->get_left_child());
    }
    if (current->get_right_child() != nullptr) {
        display_tree_util(current->get_right_child());
    }
    current->print();
}

void BSP::display_tree()
{
    display_tree_util(root);
}
