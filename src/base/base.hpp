#pragma once

#include "SDL3/SDL.h"
#include "glm/glm.hpp"
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_core.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <memory>
#include <optional>
#include <vector>

struct GPUInfo {
    VkPhysicalDeviceFeatures features;
    // format properties
    // image format properties
    VkPhysicalDeviceMemoryProperties memory_properties;
    VkPhysicalDeviceProperties properties;
    std::vector<VkQueueFamilyProperties> queue_family_properties;
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphics;
    std::optional<uint32_t> transfer;
    std::optional<uint32_t> present;
    std::optional<uint32_t> compute;
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

  private:
    // internal function for vulkan init
    bool initInstance();
    bool initSurface();
    bool pickupPhyDevice();

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
