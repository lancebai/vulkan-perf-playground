#include "vulkan_app.hpp"
#include <iostream>
#include <exception>

#ifdef __ANDROID__
#include <android_native_app_glue.h>
#include <android/log.h>

#include <mutex>

static std::mutex g_AndroidMainMutex;

void android_main(struct android_app* state) {
    std::lock_guard<std::mutex> lock(g_AndroidMainMutex);
    VulkanApp app(state);
    try {
        app.run();
    } catch (const std::exception& e) {
        __android_log_print(ANDROID_LOG_ERROR, "VulkanApp", "Application error: %s", e.what());
    }
}
#else
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
#endif
