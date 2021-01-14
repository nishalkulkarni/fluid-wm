#include "window_manager.hpp"
#include <cstdlib>
#include <iostream>
#include <memory>

int main(int argc, char** argv)
{

    /* std::unique_ptr<WindowManager> window_manager = std::make_unique<WindowManager>(WindowManager::Create()); */
    std::unique_ptr<WindowManager> window_manager(WindowManager::Create());
    if (!window_manager) {
        std::cout << "Failed to initialize window manager.";
        return EXIT_FAILURE;
    }
    std::cout<<"Window manager running"<<std::endl;
    window_manager->Run();

    return EXIT_SUCCESS;
}
