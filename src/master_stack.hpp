#ifndef MASTER_STACK_HPP
#define MASTER_STACK_HPP

extern "C" {
#include <X11/Xlib.h>
}
#include <vector>

class MasterStack {
private:
    std::vector<Window> stack;

public:
    MasterStack();

    int stackSize();
    bool empty();

    void insert(Window w);
    void erase(Window w);
    void swapMaster(Window stack_window);

    Window getMaster();
    void printStack();
};

#endif
