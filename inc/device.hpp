#pragma once

#include "glm/glm.hpp"
#include "image.hpp"
#include "util.hpp"
#include "vulkan/vulkan_core.h"
#include <memory>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

namespace vbr::app {
class App;
}

namespace vbr::swapchain {
class Swapchain;
}

namespace vbr::device {

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

class Device {
    friend class vbr::app::App;
    friend class vbr::swapchain::Swapchain;

  private:
    VkSurfaceKHR &m_vk_surface;
    bool m_debug;
    vbr::util::GPUInfo m_vk_phy_info;
    VkPhysicalDevice m_vk_phy_device;
    vbr::util::QueueFamilyIndices m_vk_queue_indices;
    VkDevice m_vk_device;
    Queues m_vk_queues;
    VkCommandPool m_vk_cmd_pool;
    VkCommandBuffer m_vk_cmd;
    SyncObjs m_vk_sync;

  private:
    [[nodiscard]] bool pickupPhyDevice(const VkInstance &instance);
    [[nodiscard]] bool initLogicDevice();
    [[nodiscard]] bool initCmds();
    [[nodiscard]] bool initSync();

    VkFence &inFlightFence() { return m_vk_sync.in_flight_fence; }
    VkSemaphore &imageAvailable() { return m_vk_sync.image_available; }
    VkSemaphore &renderDone() { return m_vk_sync.render_done; }
    VkCommandBuffer &cmd() { return m_vk_cmd; }
    void updateWindowSize();

  public:
    Device(VkSurfaceKHR &surface, bool debug = false);
    ~Device();

    bool init(const VkInstance &instance);
    VkDevice &operator*() { return m_vk_device; }
    VkQueue &graphicsQueue() { return m_vk_queues.graphics; }
    VkQueue &presentQueue() { return m_vk_queues.present; }
    VkQueue &transferQueue() { return m_vk_queues.transfer; }
    VkQueue &computeQueue() { return m_vk_queues.compute; }

    Device(Device &) = delete;
    Device(Device &&) = delete;
    Device &operator=(Device &) = delete;
    Device &operator=(Device &&) = delete;
};

} // namespace vbr::device
