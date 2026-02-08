#pragma once

#include "vulkan/vulkan_core.h"
#include <vulkan/vulkan.h>

namespace vbr::device {
class Device;

}

namespace vbr::buffer {

struct Buffer {
    friend class vbr::device::Device;

    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;

    Buffer(vbr::device::Device &d);
    ~Buffer();

  private:
    vbr::device::Device &device;

  private:
    void bind(VkDeviceSize offset = 0);
    void map(VkDeviceSize size, void **data);
    void unmap();
    void copyFrom(const Buffer &src, VkDeviceSize size);
    void cutFrom(Buffer &src, VkDeviceSize size);
};

} // namespace vbr::buffer
