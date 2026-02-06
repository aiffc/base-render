#pragma once

#include "SDL3/SDL.h"
#include "glm/glm.hpp"
#include "image.hpp"
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_core.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <memory>
#include <optional>
#include <vector>

namespace vbr::app {
struct GPUInfo {
    VkPhysicalDeviceFeatures features;
    // format properties
    // image format properties
    VkPhysicalDeviceMemoryProperties memory_properties;
    VkPhysicalDeviceProperties properties;
    std::vector<VkQueueFamilyProperties> queue_family_properties;
    VkPresentModeKHR present_mode;
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR surface_format;
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphics;
    std::optional<uint32_t> transfer;
    std::optional<uint32_t> present;
    std::optional<uint32_t> compute;
};

struct Queues {
    VkQueue graphics;
    VkQueue present;
    VkQueue transfer;
    VkQueue compute;
};

struct SyncObjs {
    VkSemaphore image_available = VK_NULL_HANDLE;
    VkSemaphore render_done = VK_NULL_HANDLE;
    VkFence in_flight_fence = VK_NULL_HANDLE;

    void destroy(const VkDevice device);
};

class App {
  protected:
    bool m_debug = true;
    glm::ivec2 m_window_size;
    struct WindowDeleter {
        void operator()(SDL_Window *window) {
            if (window) {
                SDL_DestroyWindow(window);
                window = nullptr;
            }
        }
    };
    std::unique_ptr<SDL_Window, WindowDeleter> m_window;
    bool m_quit = false;

    // vulkan things
    VkInstance m_vk_instance;
    VkDebugUtilsMessengerEXT m_vk_dbg_messager;
    VkSurfaceKHR m_vk_surface;
    VkPhysicalDevice m_vk_phy_device;
    GPUInfo m_vk_phy_info;
    QueueFamilyIndices m_vk_queue_indices;
    VkDevice m_vk_device;
    Queues m_vk_queues;
    VkSwapchainKHR m_vk_swapchain;
    std::vector<std::unique_ptr<vbr::image::Image>> m_vk_swapchain_images;
    VkCommandPool m_vk_cmd_pool;
    // TODO for now just support one command buffer
    std::vector<VkCommandBuffer> m_vk_cmds;
    SyncObjs m_vk_sync;

  protected:
    bool begin(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f);
    bool end();

  private:
    uint32_t m_vk_current_frame = 0;

  private:
    // internal function for vulkan init
    [[nodiscard]] bool initInstance();
    [[nodiscard]] bool initSurface();
    [[nodiscard]] bool pickupPhyDevice();
    [[nodiscard]] bool initLogicDevice();
    [[nodiscard]] bool initSwapchain();
    [[nodiscard]] bool initCmds(uint32_t cmd_count = 1);
    [[nodiscard]] bool initSync();

  public:
    App(const glm::ivec2 &window_size = {1024, 980});
    virtual ~App();

    bool shouldQuit() const { return m_quit; }

    [[nodiscard]] virtual bool init(SDL_InitFlags flag = SDL_INIT_AUDIO);
    virtual void update();
    virtual void event(SDL_Event *event);
    virtual void render();
    virtual void quit();

    App(App &) = delete;
    App(App &&) = delete;
    App &operator=(App &) = delete;
    App &operator=(App &&) = delete;
};

} // namespace vbr::app
