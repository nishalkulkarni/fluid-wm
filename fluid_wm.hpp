#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

extern "C" {
#include <X11/Xlib.h>
}
#include "bsp.hpp"
#include "master_stack.hpp"
#include "priority_queue.hpp"
#include "util.hpp"
#include <memory>
#include <unordered_map>

class WindowManager {
public:
    static std::unique_ptr<WindowManager> Create();
    ~WindowManager();
    void Run();

private:
    WindowManager(Display* display);
    void Frame(Window w, bool was_created_before_window_manager);
    void Unframe(Window w);

    Display* display_;
    const Window root_;
    ::std::unordered_map<Window, Window> clients_;
    PriorityQueue* pq;
    MasterStack* ms;
    BSP* bsp;

    Position<int> drag_start_pos_;
    Position<int> drag_start_frame_pos_;
    Size<int> drag_start_frame_size_;

    void OnCreateNotify(const XCreateWindowEvent& e);
    void OnDestroyNotify(const XDestroyWindowEvent& e);
    void OnReparentNotify(const XReparentEvent& e);
    void OnMapNotify(const XMapEvent& e);
    void OnUnmapNotify(const XUnmapEvent& e);
    void OnConfigureNotify(const XConfigureEvent& e);
    void OnMapRequest(const XMapRequestEvent& e);
    void OnConfigureRequest(const XConfigureRequestEvent& e);
    void OnButtonPress(const XButtonEvent& e);
    void OnButtonRelease(const XButtonEvent& e);
    void OnMotionNotify(const XMotionEvent& e);
    void OnKeyPress(const XKeyEvent& e);
    void OnKeyRelease(const XKeyEvent& e);

    static int OnXError(Display* display, XErrorEvent* e);
    static int OnWMDetected(Display* display, XErrorEvent* e);
    static bool wm_detected_;

    const Atom WM_PROTOCOLS;
    const Atom WM_DELETE_WINDOW;
};

#endif
