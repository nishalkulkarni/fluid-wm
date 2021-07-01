# fluid-wm

![C/C++ CI](https://github.com/nishalkulkarni/fluid-wm/workflows/C/C++%20CI/badge.svg)

X11 tiling window manager written in C++

![Screenshot of FluidWM](screenshot.jpeg)

### Requirements
- libX11 (Xlib header files) 

### Build Instructions
```
meson setup builddir
ninja -C builddir
```
This creates an executable(fluid_wm) in builddir/src

### Testing
```
./tests/run_fluid_wm.sh
```