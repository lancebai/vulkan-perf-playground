# Vulkan Performance Sandbox

An interactive 3D C++ rendering workbench and performance analysis environment built on the **Vulkan API**. This repository is designed to explore Vulkan's core concepts—such as command buffer lifecycles, CPU-GPU synchronization, and memory mapping—while providing a built-in playground to profile GPU bottlenecks using **NVIDIA Nsight Systems**.

---

## 🚀 Key Features

*   **Interactive 3D Geometry**: Real-time rendering of parametric shapes (Torus, Trefoil Knot, and Sphere).
*   **Dynamic Visual Styles**: Real-time configurable Phong shading with four distinct color modes:
    1.  *Vertex Color*: Standard interpolated vertex color.
    2.  *Surface Normals*: Visualizes surface normal vectors as RGB.
    3.  *Rainbow Wave*: A dynamic shader-driven color wave using sine/cosine functions and runtime time inputs.
    4.  *Distance Depth*: Shaders calculate depth fog based on camera distance.
*   **Performance Monitoring & Simulation**:
    *   **Live FPS & Latency Counter** embedded in the UI.
    *   **Simulated GPU Load**: An adjustable ALU iteration slider that forces heavy fragment shader calculations, allowing you to easily simulate extreme GPU-bound states (e.g., throttling down to 1 FPS) for profiling validation.
*   **Vulkan Optimizations**:
    *   **Persistent UBO Mapping**: Maps Uniform Buffers once at initialization and updates them via simple memory copies, eliminating `vkMapMemory` / `vkUnmapMemory` overhead per frame.
    *   **Dual Static Pipelines**: Pre-compiles both solid and wireframe pipelines at startup. Toggling wireframe mode switches pipelines dynamically rather than recreating them at runtime.
*   **Interactive UI**: Side panel UI powered by **Dear ImGui** to tweak all rendering and workload configurations on the fly.

---

## 🛠️ Tech Stack & Dependencies

The project uses the following components:
*   **Graphics API**: Vulkan (Vulkan SDK)
*   **Windowing & Input**: GLFW (fetched automatically via CMake)
*   **Mathematics**: GLM (fetched automatically via CMake)
*   **UI Overlay**: Dear ImGui (fetched automatically via CMake)
*   **Build System**: CMake (version 3.22+)

---

## 📦 Prerequisites

### 1. Install Vulkan SDK
Ensure you have the Vulkan SDK installed on your system. For Ubuntu/Debian:
```bash
sudo apt install vulkan-sdk
```
Alternatively, download it from the [LunarG Vulkan SDK page](https://vulkan.lunarg.com/).

### 2. Install Development Libraries (X11)
To build GLFW and link system windowing dependencies, install X11 development headers:
```bash
sudo apt install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libxxf86vm-dev
```

---

## 🔨 How to Build and Run

1.  **Configure the build directory**:
    ```bash
    cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
    ```
    > [!IMPORTANT]
    > Always build in **`Release`** mode when profiling. In `Debug` mode, Vulkan Validation Layers are active, which increases CPU overhead by 5x to 10x and skews performance metrics.

2.  **Compile the project**:
    ```bash
    cmake --build build -j$(nproc)
    ```

3.  **Run the application**:
    ```bash
    ./build/vulkan_app
    ```

---

## 📊 Performance Profiling with NVIDIA Nsight Systems

This project is tailored to study the "fingerprints" of CPU-bound vs. GPU-bound states.

### 1. Generate a Profiling Report
Run the application through the `nsys` command-line tool. Start the application, play around with the GUI parameters (like the **GPU Load Iterations** slider), and close the window to complete the profiling:

```bash
nsys profile \
  -w true \
  -t vulkan,osrt,opengl \
  --sample=none \
  -o vulkan_perf_report \
  ./build/vulkan_app
```

*Note: If you run this in Debug mode, set `export VK_LAYER_PATH=/path/to/vulkan/sdk/explicit_layer.d` first so validation layers can load successfully.*

### 2. View CLI Summary Statistics
To inspect the overhead of Vulkan API calls in your terminal:
```bash
nsys stats --report vulkan_api_sum vulkan_perf_report.nsys-rep
```

### 3. Understanding Profiling Metrics
*   **VSync-Bound / Idle State**: If GPU Load Iterations is `0`, you will notice `vkAcquireNextImageKHR` taking up **~90%** of the Vulkan API time. This indicates that the CPU/GPU is waiting on the display server's VSync limit.
*   **GPU-Bound State**: If you slide GPU Load Iterations to `300,000+`, the frame rate drops significantly. In this state, `vkQueuePresentKHR` takes the majority of the execution time, representing the driver blocking the CPU submission thread to protect the overloaded GPU queue.

---

## 📂 Project Structure

*   `src/`
    *   [main.cpp](src/main.cpp) - Entry point.
    *   [vulkan_app.hpp](src/vulkan_app.hpp) - Application layout, Vulkan handles, and geometry structures.
    *   [vulkan_app.cpp](src/vulkan_app.cpp) - Core Vulkan setup, swapchain management, rendering loop, and ImGui integration.
*   `shaders/`
    *   [shader.vert](shaders/shader.vert) - Vertex shader (MVP transformation, normals, and light direction pass).
    *   [shader.frag](shaders/shader.frag) - Fragment shader (Phong shading, color modes, and GPU load loop).
*   [vulkan_project_guide.md](vulkan_project_guide.md) - In-depth Chinese architectural guide of the application's synchronization, pipeline, and profiling details.
