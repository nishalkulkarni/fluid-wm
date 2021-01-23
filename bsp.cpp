#include "bsp.hpp"
#include "fluid_wm.hpp"
#include <iostream>
#include <tuple>

Node::Node()
{
    left = nullptr;
    right = nullptr;
}

void Node::set_dimensions(int w, int h, int x, int y)
{
    this->width = w;
    this->height = h;
    this->x = x;
    this->y = y;
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

std::tuple<int, int, int, int> Node::get_dimensions()
{
    return std::make_tuple(this->width, this->height, this->x, this->y);
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
    std::cout << window << " -> " << width << " -> " << height << " -> " << x << " -> " << y << std::endl;
}

std::tuple<Node*, Node*> Node::split_node(Window new_window)
{
    Node* left_child = new Node();
    Node* right_child = new Node();

    left_child->set_window(this->window);
    right_child->set_window(new_window);

    int split_width = this->width, split_height = this->height;
    int split_x = this->x, split_y = this->y;
    if (this->width >= this->height) {
        split_width /= 2;
        split_x += split_width;
        left_child->set_dimensions(split_width, this->height, this->x, this->y);
        right_child->set_dimensions(this->width - split_width, this->height, split_x, this->y);
    } else if (this->width < this->height) {
        split_height /= 2;
        split_y += split_height;
        left_child->set_dimensions(this->width, split_height, this->x, this->y);
        right_child->set_dimensions(this->width, this->height - split_height, this->x, split_y);
    }

    set_children(left_child, right_child);

    return std::make_tuple(left_child, right_child);
}

BSP::BSP()
{
    root = nullptr;
}

std::tuple<Node*, Node*> BSP::init_root(Window w, int width, int height, int x, int y)
{
    root = new Node();
    root->set_window(w);
    root->set_dimensions(width, height, x, y);
    return std::make_tuple(nullptr, root);
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

std::tuple<Node*, Node*> BSP::create_node(Window w)
{
    Node* current = find_node(w);
    if (current == nullptr) {
        std::cout << "Cannot create new window\n";
        return std::make_tuple(nullptr, nullptr);
    } else {
        return current->split_node(w);
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
