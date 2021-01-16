extern "C" {
#include <X11/Xutil.h>
}
#include "master_stack.hpp"
#include "priority_queue.hpp"
#include "util.hpp"
#include "window_manager.hpp"
#include <assert.h>
#include <cstring>
#include <iostream>

bool WindowManager::wm_detected_;

std::unique_ptr<WindowManager> WindowManager::Create()
{
    Display* display = XOpenDisplay(nullptr);
    if (display == nullptr) {
        std::cout << "Failed to open X display " << XDisplayName(nullptr) << std::endl;
        return nullptr;
    }
    std::unique_ptr<WindowManager> wmInstance(new WindowManager(display));
    return wmInstance;
}

WindowManager::WindowManager(Display* display)
    : display_(display)
    , root_(DefaultRootWindow(display_))
    , pq(new PriorityQueue())
    , ms(new MasterStack())
    , WM_PROTOCOLS(XInternAtom(display_, "WM_PROTOCOLS", false))
    , WM_DELETE_WINDOW(XInternAtom(display_, "WM_DELETE_WINDOW", false))
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
                  << XDisplayString(display_) << std::endl;
        return;
    }
    XSetErrorHandler(&WindowManager::OnXError);

    XGrabServer(display_);
    Window returned_root, returned_parent;
    Window* top_level_windows;
    unsigned int num_top_level_windows;
    assert(XQueryTree(
        display_,
        root_,
        &returned_root,
        &returned_parent,
        &top_level_windows,
        &num_top_level_windows));
    assert(returned_root == root_);
    for (unsigned int i = 0; i < num_top_level_windows; ++i) {
        Frame(top_level_windows[i], true);
    }
    XFree(top_level_windows);
    XUngrabServer(display_);

    for (;;) {
        XEvent e;
        XNextEvent(display_, &e);
        std::cout << "Received event: " << ToString(e) << std::endl;

        switch (e.type) {
        case CreateNotify:
            OnCreateNotify(e.xcreatewindow);
            break;
        case DestroyNotify:
            OnDestroyNotify(e.xdestroywindow);
            break;
        case ReparentNotify:
            OnReparentNotify(e.xreparent);
            break;
        case MapNotify:
            OnMapNotify(e.xmap);
            break;
        case UnmapNotify:
            OnUnmapNotify(e.xunmap);
            break;
        case ConfigureNotify:
            OnConfigureNotify(e.xconfigure);
            break;
        case MapRequest:
            OnMapRequest(e.xmaprequest);
            break;
        case ConfigureRequest:
            OnConfigureRequest(e.xconfigurerequest);
            break;
        case ButtonPress:
            OnButtonPress(e.xbutton);
            break;
        case ButtonRelease:
            OnButtonRelease(e.xbutton);
            break;
        case MotionNotify:
            // Skip any already pending motion events.
            while (XCheckTypedWindowEvent(
                display_, e.xmotion.window, MotionNotify, &e)) { }
            OnMotionNotify(e.xmotion);
            break;
        case KeyPress:
            OnKeyPress(e.xkey);
            break;
        case KeyRelease:
            OnKeyRelease(e.xkey);
            break;
        default:
            std::cout << "Ignored event" << std::endl;
        }
    }
}

void WindowManager::Frame(Window w, bool was_created_before_window_manager)
{
    const unsigned int BORDER_WIDTH = 3;
    const unsigned long BORDER_COLOR = 0xff1234;
    const unsigned long BG_COLOR = 0x0000ff;

    assert(!clients_.count(w));

    XWindowAttributes x_window_attrs;
    assert(XGetWindowAttributes(display_, w, &x_window_attrs));

    if (was_created_before_window_manager) {
        if (x_window_attrs.override_redirect || x_window_attrs.map_state != IsViewable) {
            return;
        }
    }

    std::cout << " \n\n " << x_window_attrs.width << " -- " << x_window_attrs.height << "\n\n";
    std::cout << " \n\n " << w << "\n\n";

    const Window frame = XCreateSimpleWindow(
        display_,
        root_,
        x_window_attrs.x,
        x_window_attrs.y,
        x_window_attrs.width,
        x_window_attrs.height,
        BORDER_WIDTH,
        BORDER_COLOR,
        BG_COLOR);

    XSelectInput(
        display_,
        frame,
        SubstructureRedirectMask | SubstructureNotifyMask);

    XAddToSaveSet(display_, w);

    XReparentWindow(
        display_,
        w,
        frame,
        0, 0);

    XMapWindow(display_, frame);

    clients_[w] = frame;

    ms->insert(w);
    ms->printStack();

    XGrabButton(
        display_,
        Button1,
        Mod1Mask,
        w,
        false,
        ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
        GrabModeAsync,
        GrabModeAsync,
        None,
        None);
    //   b. Resize windows with alt + right button.
    XGrabButton(
        display_,
        Button3,
        Mod1Mask,
        w,
        false,
        ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
        GrabModeAsync,
        GrabModeAsync,
        None,
        None);
    //   c. Kill windows with alt + f4.
    XGrabKey(
        display_,
        XKeysymToKeycode(display_, XK_F4),
        Mod1Mask,
        w,
        false,
        GrabModeAsync,
        GrabModeAsync);
    //   d. Switch windows with alt + tab.
    XGrabKey(
        display_,
        XKeysymToKeycode(display_, XK_Tab),
        Mod1Mask,
        w,
        false,
        GrabModeAsync,
        GrabModeAsync);

    std::cout << "Framed window " << w << " [" << frame << "]";
}

void WindowManager::Unframe(Window w)
{
    assert(clients_.count(w));

    const Window frame = clients_[w];
    XUnmapWindow(display_, frame);
    XReparentWindow(
        display_,
        w,
        root_,
        0, 0);
    XRemoveFromSaveSet(display_, w);
    XDestroyWindow(display_, frame);

    clients_.erase(w);
    ms->erase(w);

    std::cout << "Unframed window " << w << " [" << frame << "]";
}

void WindowManager::OnCreateNotify(const XCreateWindowEvent& e) { }

void WindowManager::OnDestroyNotify(const XDestroyWindowEvent& e) { }

void WindowManager::OnReparentNotify(const XReparentEvent& e) { }

void WindowManager::OnMapNotify(const XMapEvent& e) { }

void WindowManager::OnUnmapNotify(const XUnmapEvent& e)
{
    if (!clients_.count(e.window)) {
        std::cout << "Ignore UnmapNotify for non-client window " << e.window;
        return;
    }

    if (e.event == root_) {
        std::cout << "Ignore UnmapNotify for reparented pre-existing window "
                  << e.window;
        return;
    }

    Unframe(e.window);
}

void WindowManager::OnConfigureNotify(const XConfigureEvent& e) { }

void WindowManager::OnConfigureRequest(const XConfigureRequestEvent& e)
{
    Window returned_root;
    int x, y;
    unsigned width, height, border_width, depth;

    assert(XGetGeometry(
        display_,
        root_,
        &returned_root,
        &x, &y,
        &width, &height,
        &border_width,
        &depth));

    std::cout << width << " @@ " << height << std::endl;
    std::cout << e.window << std::endl;
    std::cout << returned_root << std::endl;

    XWindowChanges changes;
    // Copy fields from e to changes.
    changes.x = e.x;
    changes.y = e.y;
    changes.width = e.width;
    changes.height = e.height;
    changes.border_width = e.border_width;
    changes.sibling = e.above;
    changes.stack_mode = e.detail;
    if (clients_.count(e.window)) {
        const Window frame = clients_[e.window];
        XConfigureWindow(display_, frame, e.value_mask, &changes);
        std::cout << "Resize [" << frame << "] to " << Size<int>(e.width, e.height);
    }
    XConfigureWindow(display_, e.window, e.value_mask, &changes);
    std::cout << "Resize " << e.window << " to " << Size<int>(e.width, e.height);
}

void WindowManager::OnMapRequest(const XMapRequestEvent& e)
{
    Frame(e.window, false);
    XMapWindow(display_, e.window);
}

void WindowManager::OnButtonPress(const XButtonEvent& e)
{
    assert(clients_.count(e.window));
    const Window frame = clients_[e.window];

    drag_start_pos_ = Position<int>(e.x_root, e.y_root);

    Window returned_root;
    int x, y;
    unsigned width, height, border_width, depth;
    assert(XGetGeometry(
        display_,
        frame,
        &returned_root,
        &x, &y,
        &width, &height,
        &border_width,
        &depth));
    drag_start_frame_pos_ = Position<int>(x, y);
    drag_start_frame_size_ = Size<int>(width, height);

    XRaiseWindow(display_, frame);
}

void WindowManager::OnButtonRelease(const XButtonEvent& e) { }

void WindowManager::OnMotionNotify(const XMotionEvent& e)
{
    assert(clients_.count(e.window));
    const Window frame = clients_[e.window];
    const Position<int> drag_pos(e.x_root, e.y_root);
    const Vector2D<int> delta = drag_pos - drag_start_pos_;

    if (e.state & Button1Mask) {
        // alt + left button: Move window.
        const Position<int> dest_frame_pos = drag_start_frame_pos_ + delta;
        XMoveWindow(
            display_,
            frame,
            dest_frame_pos.x, dest_frame_pos.y);
    } else if (e.state & Button3Mask) {
        // alt + right button: Resize window.
        // Window dimensions cannot be negative.
        const Vector2D<int> size_delta(
            std::max(delta.x, -drag_start_frame_size_.width),
            std::max(delta.y, -drag_start_frame_size_.height));
        const Size<int> dest_frame_size = drag_start_frame_size_ + size_delta;
        // 1. Resize frame.
        XResizeWindow(
            display_,
            frame,
            dest_frame_size.width, dest_frame_size.height);
        // 2. Resize client window.
        XResizeWindow(
            display_,
            e.window,
            dest_frame_size.width, dest_frame_size.height);
    }
}

void WindowManager::OnKeyPress(const XKeyEvent& e)
{
    if ((e.state & Mod1Mask) && (e.keycode == XKeysymToKeycode(display_, XK_F4))) {
        // alt + f4: Close window.
        Atom* supported_protocols;
        int num_supported_protocols;
        if (XGetWMProtocols(display_,
                e.window,
                &supported_protocols,
                &num_supported_protocols)
            && (::std::find(supported_protocols,
                    supported_protocols + num_supported_protocols,
                    WM_DELETE_WINDOW)
                != supported_protocols + num_supported_protocols)) {
            std::cout << "Gracefully deleting window " << e.window;

            XEvent msg;
            memset(&msg, 0, sizeof(msg));
            msg.xclient.type = ClientMessage;
            msg.xclient.message_type = WM_PROTOCOLS;
            msg.xclient.window = e.window;
            msg.xclient.format = 32;
            msg.xclient.data.l[0] = WM_DELETE_WINDOW;

            assert(XSendEvent(display_, e.window, false, 0, &msg));
        } else {
            std::cout << "Killing window " << e.window;
            XKillClient(display_, e.window);
        }
    } else if ((e.state & Mod1Mask) && (e.keycode == XKeysymToKeycode(display_, XK_Tab))) {
        // alt + tab: Switch window.
        auto i = clients_.find(e.window);
        assert(i != clients_.end());
        ++i;
        if (i == clients_.end()) {
            i = clients_.begin();
        }

        XRaiseWindow(display_, i->second);
        XSetInputFocus(display_, i->first, RevertToPointerRoot, CurrentTime);
    }
}

void WindowManager::OnKeyRelease(const XKeyEvent& e) { }

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
              << "    Resource ID: " << e->resourceid << "\n";
    return 0;
}
