#pragma once

#include <vulkan/vulkan.h>

namespace vbr::image {

struct Image {
    VkImage image = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;

    Image(const VkDevice &device, VkImage from, bool is_swapchain = false);
    ~Image();

    bool init(VkFormat fomrat);
    void destroy();

  private:
    const VkDevice &main_device;
    bool is_swapchain_image;
};

} // namespace vbr::image
