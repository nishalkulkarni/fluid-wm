#ifndef BSP_HPP
#define BSP_HPP

extern "C" {
#include <X11/Xlib.h>
}
#include <utility>

class Node {
private:
    int width, height;
    Window window;
    Node* left;
    Node* right;

public:
    Node();
    void set_dimensions(int w, int h);
    void set_window(Window w);
    void set_children(Node* l, Node* r);

    ::std::pair<int, int> get_dimensions();
    Window get_window();
    Node* get_left_child();
    Node* get_right_child();

    void print();
    bool isLeaf();
    void split_node(Window new_window);
};

class BSP {
private:
    Node* root;

public:
    BSP();
    void init_root(Window w, int width, int height);
    Node* find_node_util(Window w, Node* current);
    Node* find_node(Window w);
    void create_node(Window w);
    void display_tree_util(Node* current);
    void display_tree();
};

#endif
