#include "window_manager.hpp"
#include "util.hpp"
#include <assert.h>
#include <iostream>

bool WindowManager::wm_detected_;

std::unique_ptr<WindowManager> WindowManager::Create()
{
    const char* display_c_str = nullptr;

    Display* display = XOpenDisplay(display_c_str);
    if (display == nullptr) {
        std::cout << "Failed to open X display " << XDisplayName(display_c_str);
        return nullptr;
    }
    std::unique_ptr<WindowManager> wmInstance = std::make_unique<WindowManager>(WindowManager(display));
    return wmInstance;
}

WindowManager::WindowManager(Display* display)
    : display_(display)
    , root_(DefaultRootWindow(display_))
{
}

WindowManager::~WindowManager()
{
    XCloseDisplay(display_);
}

void WindowManager::Run()
{
    wm_detected_ = false;
    XSetErrorHandler(&WindowManager::OnWMDetected);

    XSelectInput(
        display_,
        root_,
        SubstructureRedirectMask | SubstructureNotifyMask);
    XSync(display_, false);
    if (wm_detected_) {
        std::cout << "Detected another window manager on display "
                  << XDisplayString(display_);
        return;
    }
    XSetErrorHandler(&WindowManager::OnXError);

    for(;;){
        XEvent e;
        XNextEvent(display_, &e);
        /* std::cout<< "Received event: " << e.toString(); */

    }
}

int WindowManager::OnWMDetected(Display* display, XErrorEvent* e)
{
    assert(static_cast<int>(e->error_code) == BadAccess);
    wm_detected_ = true;
    return 0;
}

int WindowManager::OnXError(Display* display, XErrorEvent* e)
{
    const int MAX_ERROR_TEXT_LENGTH = 1024;
    char error_text[MAX_ERROR_TEXT_LENGTH];
    XGetErrorText(display, e->error_code, error_text, sizeof(error_text));

    std::cout << "Received X error:\n"
              << "    Request: " << int(e->request_code)
              << " - " << XRequestCodeToString(e->request_code) << "\n"
              << "    Error code: " << int(e->error_code)
              << " - " << error_text << "\n"
              << "    Resource ID: " << e->resourceid;
    return 0;
}
