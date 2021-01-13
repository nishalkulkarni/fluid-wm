#ifndef UTIL_HPP
#define UTIL_HPP

extern "C" {
#include <X11/Xlib.h>
}
#include <string>

extern ::std::string XRequestCodeToString(unsigned char request_code);

#endif
