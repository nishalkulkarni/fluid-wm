#ifndef BSP_HPP
#define BSP_HPP

extern "C" {
#include <X11/Xlib.h>
}
#include <utility>

class Node {
private:
  int width, height, x, y;
  Window window;
  Node *left;
  Node *right;

public:
  Node();
  void set_dimensions(int w, int h, int x, int y);
  void set_window(Window w);
  void set_children(Node *l, Node *r);

  ::std::tuple<int, int, int, int> get_dimensions();
  Window get_window();
  Node *get_left_child();
  Node *get_right_child();

  void print();
  bool is_leaf();
  ::std::tuple<Node *, Node *> split_node(Window new_window);
};

class BSP {
private:
  Node *root;

public:
  BSP();
  ::std::tuple<Node *, Node *> init_root(Window w, int width, int height, int x,
                                         int y);
  Node *find_node_util(Window w, Node *current);
  Node *find_node(Window w);
  ::std::tuple<Node *, Node *> create_node(Window w);
  ::std::tuple<Node *, Node *> find_to_delete_node(Window w, Node *current);
  ::std::tuple<Node *, Node *> delete_node(Window w);
  void display_tree_util(Node *current);
  void display_tree();
};

#endif
