#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <array>
#include <string>
#include <memory>
#include <optional>

// Structures for Vulkan geometry and uniforms

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec3 normal;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, normal);

        return attributeDescriptions;
    }
};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec4 lightPos;
    glm::vec4 viewPos;
};

struct PushConsts {
    int colorMode = 0; // 0: Vertex, 1: Normal, 2: Rainbow, 3: Depth
    float shininess = 32.0f;
    float time = 0.0f;
    float ambientIntensity = 0.15f;
    int gpuLoadIterations = 0;
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanApp {
public:
    VulkanApp();
    ~VulkanApp();

    void run();

private:
    // Window dimensions
    uint32_t width = 1280;
    uint32_t height = 720;
    GLFWwindow* window = nullptr;

    // Vulkan Core
    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkQueue presentQueue = VK_NULL_HANDLE;

    // Swap Chain
    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    // Render Pass & Pipeline
    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline graphicsPipeline = VK_NULL_HANDLE;

    // Command Pools & Buffers
    VkCommandPool commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers;

    // Depth Buffering
    VkImage depthImage = VK_NULL_HANDLE;
    VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;
    VkImageView depthImageView = VK_NULL_HANDLE;

    // Geometry buffers
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
    VkBuffer indexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

    // Uniform buffers (one per swapchain image in flight)
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    // Descriptor sets
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;

    // ImGui-specific Vulkan resources
    VkDescriptorPool imguiDescriptorPool = VK_NULL_HANDLE;

    // Synchronization Objects
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;
    bool framebufferResized = false;

    // Application state & settings (ImGui controllable)
    PushConsts pushConsts;
    float rotationSpeed = 0.5f;
    bool rotateAuto = true;
    float manualRotationX = 0.0f;
    float manualRotationY = 0.0f;
    glm::vec3 shapeColor = glm::vec3(0.1f, 0.6f, 0.9f);
    glm::vec3 clearColor = glm::vec3(0.08f, 0.08f, 0.12f); // Dark slate
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.95f);
    glm::vec3 lightPos = glm::vec3(5.0f, 5.0f, 3.0f);
    bool drawWireframe = false;
    int selectedShape = 0; // 0: Torus, 1: Trefoil Knot, 2: Sphere
    bool rebuildGeometry = false;

    // Camera settings & controls
    glm::vec3 cameraTarget = glm::vec3(0.0f);
    float cameraDistance = 3.5f;
    float cameraPitch = 0.2f;  // radians
    float cameraYaw = 0.8f;    // radians
    bool isDragging = false;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    int dragButton = -1; // 0: Left, 1: Right

    // Performance Stats
    float fps = 0.0f;
    float frameTimeMs = 0.0f;
    std::string gpuName = "";
    std::string apiVersion = "";

    // Initialization helpers
    void initWindow();
    void initVulkan();
    void initImGui();
    void mainLoop();
    void cleanup();

    // Vulkan creation steps
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createCommandPool();
    void createDepthResources();
    void createFramebuffers();
    void createVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffers();
    void createDescriptorPool();
    void createDescriptorSets();
    void createCommandBuffers();
    void createSyncObjects();

    // Utility methods
    void recreateSwapChain();
    void cleanupSwapChain();
    void updateUniformBuffer(uint32_t currentImage);
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void drawFrame();
    void renderImGui();
    void generateGeometry();
    void generateTorus(float R, float r, int radialSeg, int tubularSeg);
    void generateTrefoilKnot(int segments, int tubeSegments, float tubeRadius);
    void generateSphere(float radius, int rings, int sectors);

    // Helper functions for raw buffer creation
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkShaderModule createShaderModule(const std::vector<char>& code);
    VkFormat findDepthFormat();
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

    // Callbacks
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};
