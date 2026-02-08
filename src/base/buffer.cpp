#include "../../inc/buffer.hpp"
#include "../../inc/device.hpp"
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_core.h"
#include <spdlog/spdlog.h>

namespace vbr::buffer {

Buffer::Buffer(vbr::device::Device &d) : device(d) {}
Buffer::~Buffer() {
    if (*device != VK_NULL_HANDLE && memory != VK_NULL_HANDLE) {
        vkFreeMemory(*device, memory, nullptr);
        memory = VK_NULL_HANDLE;
    }
    if (*device != VK_NULL_HANDLE && buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(*device, buffer, nullptr);
        buffer = VK_NULL_HANDLE;
    }
}

void Buffer::bind(VkDeviceSize offset) {
    vkBindBufferMemory(*device, buffer, memory, offset);
}

void Buffer::map(VkDeviceSize size, void **data) {
    if (*device != VK_NULL_HANDLE) {
        vkMapMemory(*device, memory, 0, size, 0, data);
    } else {
        spdlog::error("invalid memory");
    }
}

void Buffer::unmap() {
    if (*device != VK_NULL_HANDLE) {
        vkUnmapMemory(*device, memory);
    } else {
        spdlog::error("invalid memory");
    }
}

void Buffer::copyFrom(const Buffer &src, VkDeviceSize size) {
    if (src.buffer == VK_NULL_HANDLE || src.memory == VK_NULL_HANDLE) {
        spdlog::warn("copy invalid buffer");
        return;
    }
    auto cmd = device.beginTemporaryCommand();
    if (cmd != VK_NULL_HANDLE) {
        VkBufferCopy info{
            .srcOffset = 0,
            .dstOffset = 0,
            .size = size,
        };

        vkCmdCopyBuffer(cmd, src.buffer, buffer, 1, &info);
        device.endTemporaryCommand(cmd);
    }
}

void Buffer::cutFrom(Buffer &src, VkDeviceSize size) {
    if (src.buffer == VK_NULL_HANDLE || src.memory == VK_NULL_HANDLE) {
        spdlog::warn("copy invalid buffer");
        return;
    }
    auto cmd = device.beginTemporaryCommand();
    if (cmd != VK_NULL_HANDLE) {
        VkBufferCopy info{
            .srcOffset = 0,
            .dstOffset = 0,
            .size = size,
        };
        vkCmdCopyBuffer(cmd, src.buffer, buffer, 1, &info);
        device.endTemporaryCommand(cmd);
        vkFreeMemory(*device, src.memory, nullptr);
        vkDestroyBuffer(*device, src.buffer, nullptr);
        src.memory = VK_NULL_HANDLE;
        src.buffer = VK_NULL_HANDLE;
    }
}

} // namespace vbr::buffer
