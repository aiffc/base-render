#include "../../inc/swapchain.hpp"
#include "../../inc/device.hpp"
#include "spdlog/spdlog.h"

namespace vbr::swapchain {

Swapchain::Swapchain(vbr::device::Device &device) : m_vk_device(device) {}
Swapchain::~Swapchain() {
    if (!m_vk_swapchain_images.empty()) {
        m_vk_swapchain_images.clear();
    }

    if (m_vk_swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(*m_vk_device, m_vk_swapchain, nullptr);
        m_vk_swapchain = VK_NULL_HANDLE;
    }
}

bool Swapchain::init(const glm::ivec2 &window_size) {
    vkDeviceWaitIdle(*m_vk_device);

    uint32_t image_count =
        m_vk_device.m_vk_phy_info.capabilities.minImageCount + 1;
    if (image_count > m_vk_device.m_vk_phy_info.capabilities.maxImageCount &&
        m_vk_device.m_vk_phy_info.capabilities.maxImageCount > 0) {
        image_count = m_vk_device.m_vk_phy_info.capabilities.maxImageCount;
    }

    VkExtent2D extent{
        .width = static_cast<uint32_t>(window_size.x),
        .height = static_cast<uint32_t>(window_size.y),
    };

    extent.width =
        std::clamp(extent.width,
                   m_vk_device.m_vk_phy_info.capabilities.maxImageExtent.width,
                   m_vk_device.m_vk_phy_info.capabilities.minImageExtent.width);
    extent.height = std::clamp(
        extent.height,
        m_vk_device.m_vk_phy_info.capabilities.maxImageExtent.height,
        m_vk_device.m_vk_phy_info.capabilities.minImageExtent.height);

    uint32_t graphics_queue_indices =
        m_vk_device.m_vk_queue_indices.graphics.value();
    uint32_t present_queue_indices =
        m_vk_device.m_vk_queue_indices.present.value();
    std::vector<uint32_t> indices;
    VkSharingMode sharing_mode = VK_SHARING_MODE_EXCLUSIVE;
    if (graphics_queue_indices != present_queue_indices) {
        indices.push_back(graphics_queue_indices);
        indices.push_back(present_queue_indices);
        sharing_mode = VK_SHARING_MODE_CONCURRENT;
    }
    VkSwapchainKHR old_swapchain = VK_NULL_HANDLE;
    if (m_vk_swapchain != VK_NULL_HANDLE) {
        old_swapchain = m_vk_swapchain;
        m_vk_swapchain = VK_NULL_HANDLE;
    }

    VkSwapchainCreateInfoKHR info{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = m_vk_device.m_vk_surface,
        .minImageCount = image_count,
        .imageFormat = m_vk_device.m_vk_phy_info.surface_format.format,
        .imageColorSpace = m_vk_device.m_vk_phy_info.surface_format.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = sharing_mode,
        .queueFamilyIndexCount = static_cast<uint32_t>(indices.size()),
        .pQueueFamilyIndices = indices.data(),
        .preTransform = m_vk_device.m_vk_phy_info.capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = m_vk_device.m_vk_phy_info.present_mode,
        .clipped = VK_TRUE,
        .oldSwapchain = old_swapchain,
    };

    if (VK_SUCCESS !=
        vkCreateSwapchainKHR(*m_vk_device, &info, nullptr, &m_vk_swapchain)) {
        spdlog::error("failed to create swaochain khr");
        return false;
    }
    // destroy old swapchain
    if (old_swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(*m_vk_device, old_swapchain, nullptr);
        if (!m_vk_swapchain_images.empty()) {
            m_vk_swapchain_images.clear();
        }
    }

    uint32_t count = 0;
    if (VK_SUCCESS == vkGetSwapchainImagesKHR(*m_vk_device, m_vk_swapchain,
                                              &count, nullptr)) {
        std::vector<VkImage> swapchain_images{count};
        if (VK_SUCCESS == vkGetSwapchainImagesKHR(*m_vk_device, m_vk_swapchain,
                                                  &count,
                                                  swapchain_images.data())) {

            for (size_t i = 0; i < count; ++i) {
                auto siv = std::make_unique<vbr::image::Image>(
                    *m_vk_device, swapchain_images[i], true);
                siv->init(m_vk_device.m_vk_phy_info.surface_format.format);
                m_vk_swapchain_images.push_back(std::move(siv));
            }
        } else {
            spdlog::error("failed to get swapchain images");
            return false;
        }
    } else {
        spdlog::error("failed to get swapchain images");
        return false;
    }

    return true;
}

VkResult Swapchain::acquireNext() {
    return vkAcquireNextImageKHR(*m_vk_device, m_vk_swapchain, UINT64_MAX,
                                 m_vk_device.m_vk_sync.image_available,
                                 VK_NULL_HANDLE, &m_current_index);
}

} // namespace vbr::swapchain
