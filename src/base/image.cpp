#include "../../inc/image.hpp"
#include "../../inc/device.hpp"
#include "vulkan/vulkan_core.h"

namespace vbr::image {

Image::Image(const VkDevice &device, VkImage from, bool is_swapchain)
    : image(from), main_device(device), is_swapchain_image(is_swapchain) {}

Image::~Image() { destroy(); }

bool Image::init(VkFormat format) {
    if (main_device != VK_NULL_HANDLE && image != VK_NULL_HANDLE) {
        VkImageViewCreateInfo info{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = format,
            .components =
                {
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                },
            .subresourceRange =
                {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
        };
        if (VK_SUCCESS ==
            vkCreateImageView(main_device, &info, nullptr, &view)) {
            return true;
        }
        return false;
    }
    return false;
}

void Image::destroy() {
    if (view != VK_NULL_HANDLE && main_device != VK_NULL_HANDLE) {
        vkDestroyImageView(main_device, view, nullptr);
    }
    if (image != VK_NULL_HANDLE && main_device != VK_NULL_HANDLE &&
        !is_swapchain_image) {
        vkDestroyImage(main_device, image, nullptr);
    }
}

Texture::Texture(vbr::device::Device &device) : main_device(device) {}
Texture::~Texture() {
    if (*main_device != VK_NULL_HANDLE) {
        if (memory != VK_NULL_HANDLE) {
            vkFreeMemory(*main_device, memory, nullptr);
            memory = VK_NULL_HANDLE;
        }
        if (sampler != VK_NULL_HANDLE) {
            vkDestroySampler(*main_device, sampler, nullptr);
            sampler = VK_NULL_HANDLE;
        }
        if (image != VK_NULL_HANDLE) {
            vkDestroyImage(*main_device, image, nullptr);
            image = VK_NULL_HANDLE;
        }
        if (view != VK_NULL_HANDLE) {
            vkDestroyImageView(*main_device, view, nullptr);
            view = VK_NULL_HANDLE;
        }
    }
}

bool Texture::init(VkFormat format) {
    if (*main_device != VK_NULL_HANDLE && image != VK_NULL_HANDLE) {
        VkImageViewCreateInfo info{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = format,
            .components =
                {
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                },
            .subresourceRange =
                {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
        };
        if (VK_SUCCESS ==
            vkCreateImageView(*main_device, &info, nullptr, &view)) {
            return true;
        }
        return false;
    }
    return false;
}

void Texture::copyFrom(VkBuffer &buffer, glm::ivec2 size) {
    auto cmd = main_device.beginTemporaryCommand();
    VkBufferImageCopy copy_info{
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1,

            },
        .imageOffset =
            {
                .x = 0,
                .y = 0,
                .z = 0,

            },
        .imageExtent =
            {
                .width = static_cast<uint32_t>(size.x),
                .height = static_cast<uint32_t>(size.y),
                .depth = 1,
            },
    };
    vkCmdCopyBufferToImage(cmd, buffer, image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_info);
    main_device.endTemporaryCommand(cmd);
}
} // namespace vbr::image
