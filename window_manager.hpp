#ifndef WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_HPP

extern "C" {
#include <X11/Xlib.h>
}
#include "util.hpp"
#include <memory>

class WindowManager {
public:
    static std::unique_ptr<WindowManager> Create();
    ~WindowManager();
    void Run();

private:
    WindowManager(Display* display);
    Display* display_;
    const Window root_;

    static int OnXError(Display* display, XErrorEvent* e);
    static int OnWMDetected(Display* display, XErrorEvent* e);
    static bool wm_detected_;
};

#endif
