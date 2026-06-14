#include "vulkan_app.hpp"
#include <iostream>
#include <exception>
#include <csignal>

volatile sig_atomic_t g_SignalInterrupt = 0;

void signalHandler(int signum) {
    g_SignalInterrupt = 1;
}

int main() {
    VulkanApp app;
    
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Application error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
