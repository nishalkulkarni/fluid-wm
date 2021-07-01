#ifndef PRIORITY_QUEUE_HPP
#define PRIORITY_QUEUE_HPP

extern "C" {
#include <X11/Xlib.h>
}
#include <unordered_map>
#include <vector>

class PriorityQueue {
private:
  ::std::vector<std::pair<int, Window>> window;
  ::std::unordered_map<Window, int> priority;
  int numWindows;

  int PARENT(int i);
  int LEFT(int i);
  int RIGHT(int i);

  void heapify_down(int i);
  void heapify_up(int i);

public:
  PriorityQueue();

  int queueSize();
  bool empty();

  void insert(Window w);
  void pop();
  void swapPriority(Window a, Window b);

  Window getMaster();
  void printQueue();
};

#endif
