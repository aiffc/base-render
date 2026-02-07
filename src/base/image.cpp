#include "../../inc/image.hpp"

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
} // namespace vbr::image
