#pragma once

#include "glm/glm.hpp"
#include "image.hpp"
#include "util.hpp"
#include "vulkan/vulkan_core.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

namespace vbr::device {
class Device;
}

namespace vbr::swapchain {

class Swapchain {
  private:
    vbr::device::Device &m_vk_device;
    VkSwapchainKHR m_vk_swapchain = VK_NULL_HANDLE;
    std::vector<std::unique_ptr<vbr::image::Image>> m_vk_swapchain_images;
    uint32_t m_current_index;

  public:
    Swapchain(vbr::device::Device &device);
    ~Swapchain();

    VkSwapchainKHR &operator*() { return m_vk_swapchain; }
    bool init(const glm::ivec2 &window_size);

    VkResult acquireNext();
    VkImage &currentImage() const {
        return m_vk_swapchain_images[m_current_index]->image;
    }
    VkImageView &currentView() const {
        return m_vk_swapchain_images[m_current_index]->view;
    }
    uint32_t currentIndex() const { return m_current_index; }

    Swapchain(Swapchain &) = delete;
    Swapchain(Swapchain &&) = delete;
    Swapchain &operator=(Swapchain &) = delete;
    Swapchain &operator=(Swapchain &&) = delete;
};

} // namespace vbr::swapchain
