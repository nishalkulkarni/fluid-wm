#include "window_manager.hpp"
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

int PriorityQueue::PARENT(int i)
{
    return (i - 1) / 2;
}

int PriorityQueue::LEFT(int i)
{
    return (2 * i + 1);
}

int PriorityQueue::RIGHT(int i)
{
    return (2 * i + 2);
}

void PriorityQueue::heapify_down(int i)
{
    int left = LEFT(i);
    int right = RIGHT(i);

    int smallest = i;

    if (left < queueSize() && window[left].first < window[i].first)
        smallest = left;

    if (right < queueSize() && window[right].first < window[smallest].first)
        smallest = right;

    if (smallest != i) {
        std::swap(window[i], window[smallest]);
        heapify_down(smallest);
    }
}

void PriorityQueue::heapify_up(int i)
{
    if (i && window[PARENT(i)].first > window[i].first) {
        std::swap(window[i], window[PARENT(i)]);
        heapify_up(PARENT(i));
    }
}

PriorityQueue::PriorityQueue()
{
    numWindows = 0;
}

int PriorityQueue::queueSize()
{
    return (int)window.size();
}

bool PriorityQueue::empty()
{
    return queueSize() == 0;
}

void PriorityQueue::insert(Window w)
{
    numWindows += 1;
    priority[w] = numWindows;
    window.push_back({ numWindows, w });

    int index = queueSize() - 1;
    heapify_up(index);
}

void PriorityQueue::pop()
{
    if (empty()) {
        std::cout << "No windows open (Heap underflow)" << std::endl;
        return;
    }

    window[0] = window.back();
    window.pop_back();

    heapify_down(0);
}

Window PriorityQueue::getMaster()
{
    if (empty()) {
        std::cout << "No windows open (Heap underflow)" << std::endl;
        return -1;
    }

    return window.at(0).second;
}

void PriorityQueue::swapPriority(Window a,Window b){
    if(!priority.count(a) || !priority.count(b)){
        std::cout << "Swap window not found" << std::endl;
        return;
    }

    int temp = priority[a];
    priority[a] = priority[b];
    priority[b] = temp;

}

void PriorityQueue::printQueue()
{
    for (auto i : window) {
        std::cout << i.first << " " << i.second << std::endl;
    }
}
