#include "master_stack.hpp"
#include "window_manager.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

MasterStack::MasterStack()
{
}

int MasterStack::stackSize()
{
    return stack.size();
}

bool MasterStack::empty()
{
    return stack.size() == 0;
}

void MasterStack::insert(Window w)
{
    stack.push_back(w);
}

void MasterStack::erase(Window w)
{
    std::vector<Window>::iterator pos = find(stack.begin(), stack.end(), w);
    if (pos == stack.end()) {
        std::cout << "Error: Window not found\n";
        return;
    }
    stack.erase(pos);
}

void MasterStack::swapMaster(Window stack_window)
{
    std::vector<Window>::iterator posCurrent = find(stack.begin(), stack.end(), stack[0]);
    std::vector<Window>::iterator posStack = find(stack.begin(), stack.end(), stack_window);
    if (posCurrent == stack.end() || posStack == stack.end()) {
        std::cout << "Error: Window not found\n";
        return;
    }
    std::swap(*posCurrent, *posStack);
}

Window MasterStack::getMaster()
{
    return stack[0];
}

void MasterStack::printStack()
{
    for (int i = 0; i < stackSize(); i++) {
        std::cout << i + 1 << " " << stack[i] << std::endl;
    }
}
