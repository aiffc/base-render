#pragma once

#include "glm/glm.hpp"
#include "vulkan/vulkan_core.h"
#include <string_view>
#include <vulkan/vulkan.h>

namespace vbr::device {
class Device;

}

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

struct Texture {
    VkImage image = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkSampler sampler = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;

    Texture(vbr::device::Device &device);
    ~Texture();

    bool init(VkFormat format);

    void copyFrom(VkBuffer &buffer, glm::ivec2 size);

  private:
    vbr::device::Device &main_device;
};

} // namespace vbr::image
