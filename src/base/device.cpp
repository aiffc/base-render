#include "../../inc/device.hpp"
#include "glm/glm.hpp"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <optional>
#include <set>

namespace vbr::device {
const std::vector<char const *> validation_layers = {
    "VK_LAYER_KHRONOS_validation",
};

void SyncObjs::destroy(const VkDevice device) {
    if (device == VK_NULL_HANDLE) {
        return;
    }
    if (image_available != VK_NULL_HANDLE) {
        vkDestroySemaphore(device, image_available, nullptr);
        image_available = VK_NULL_HANDLE;
    }
    if (render_done != VK_NULL_HANDLE) {
        vkDestroySemaphore(device, render_done, nullptr);
        render_done = VK_NULL_HANDLE;
    }
    if (in_flight_fence != VK_NULL_HANDLE) {
        vkDestroyFence(device, in_flight_fence, nullptr);
        in_flight_fence = VK_NULL_HANDLE;
    }
}

Device::Device(VkSurfaceKHR &surface, bool debug)
    : m_vk_surface(surface), m_debug(debug) {}
Device::~Device() {
    if (m_vk_device == VK_NULL_HANDLE) {
        return;
    }

    if (m_vk_device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(m_vk_device);
    }

    m_vk_sync.destroy(m_vk_device);

    if (m_vk_cmd != VK_NULL_HANDLE) {
        vkFreeCommandBuffers(m_vk_device, m_vk_cmd_pool, 1, &m_vk_cmd);
        m_vk_cmd = VK_NULL_HANDLE;
    }

    if (m_vk_cmd_pool) {
        vkDestroyCommandPool(m_vk_device, m_vk_cmd_pool, nullptr);
        m_vk_cmd_pool = VK_NULL_HANDLE;
    }

    if (m_vk_device != VK_NULL_HANDLE) {
        vkDestroyDevice(m_vk_device, nullptr);
        m_vk_device = VK_NULL_HANDLE;
    }
}

bool Device::pickupPhyDevice(const VkInstance &instance) {
    uint32_t count = 0;
    if (VK_SUCCESS != vkEnumeratePhysicalDevices(instance, &count, nullptr)) {
        spdlog::error("failed to pickup physical device");
        return false;
    }
    std::vector<VkPhysicalDevice> physical_devices{count};
    if (VK_SUCCESS !=
        vkEnumeratePhysicalDevices(instance, &count, physical_devices.data())) {
        spdlog::error("failed to pickup physical device");
        return false;
    }

    bool found = false;
    for (const auto &phy : physical_devices) {
        VkPhysicalDeviceProperties properties;
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceProperties(phy, &properties);
        vkGetPhysicalDeviceFeatures(phy, &features);

        spdlog::info("device name {}", properties.deviceName);

        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
            features.geometryShader) {
            m_vk_phy_device = phy;
            found = true;
            break;
        }
    }

    // select the bad one
    if (!found && count != 0) {
        spdlog::warn("no sutiable device, select the bad one");
        m_vk_phy_device = physical_devices[0];
        found = true;
    }

    if (found) {
        vkGetPhysicalDeviceProperties(m_vk_phy_device,
                                      &m_vk_phy_info.properties);
        vkGetPhysicalDeviceFeatures(m_vk_phy_device, &m_vk_phy_info.features);
        vkGetPhysicalDeviceMemoryProperties(m_vk_phy_device,
                                            &m_vk_phy_info.memory_properties);
        uint32_t pcount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_vk_phy_device, &pcount,
                                                 nullptr);
        m_vk_phy_info.queue_family_properties.resize(pcount);
        vkGetPhysicalDeviceQueueFamilyProperties(
            m_vk_phy_device, &pcount,
            m_vk_phy_info.queue_family_properties.data());
        // select present mode
        count = 0;
        if (VK_SUCCESS == vkGetPhysicalDeviceSurfacePresentModesKHR(
                              m_vk_phy_device, m_vk_surface, &count, nullptr)) {
            std::vector<VkPresentModeKHR> support_present_modes{count};
            if (VK_SUCCESS == vkGetPhysicalDeviceSurfacePresentModesKHR(
                                  m_vk_phy_device, m_vk_surface, &count,
                                  support_present_modes.data())) {
                if (std::ranges::any_of(
                        support_present_modes, [](const auto &present_mode) {
                            return present_mode == VK_PRESENT_MODE_MAILBOX_KHR;
                        })) {
                    spdlog::info(
                        "select present mode VK_PRESENT_MODE_MAILBOX_KHR");
                    m_vk_phy_info.present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
                } else {
                    spdlog::info("select present mode default");
                    m_vk_phy_info.present_mode = support_present_modes[0];
                }
            } else {
                spdlog::error("failed to get physical device present mode");
                return false;
            }
        } else {
            spdlog::error("failed to get physical device present mode");
            return false;
        }
        // get surface capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_vk_phy_device, m_vk_surface,
                                                  &m_vk_phy_info.capabilities);
        // get formats info
        count = 0;
        if (VK_SUCCESS == vkGetPhysicalDeviceSurfaceFormatsKHR(
                              m_vk_phy_device, m_vk_surface, &count, nullptr)) {
            std::vector<VkSurfaceFormatKHR> surface_formats{count};
            surface_formats.resize(count);
            if (VK_SUCCESS == vkGetPhysicalDeviceSurfaceFormatsKHR(
                                  m_vk_phy_device, m_vk_surface, &count,
                                  surface_formats.data())) {
                if (std::ranges::any_of(
                        surface_formats, [](const auto &surface_format) {
                            return surface_format.colorSpace ==
                                       VK_COLOR_SPACE_SRGB_NONLINEAR_KHR &&
                                   surface_format.format ==
                                       VK_FORMAT_B8G8R8A8_SRGB;
                        })) {
                    m_vk_phy_info.surface_format.format =
                        VK_FORMAT_B8G8R8A8_SRGB;
                    m_vk_phy_info.surface_format.colorSpace =
                        VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
                } else {
                    m_vk_phy_info.surface_format = surface_formats[0];
                }
            } else {
                spdlog::error("failed to get physical device surface formats");
                return false;
            }
        } else {
            spdlog::error("failed to get physical device surface formats");
            return false;
        }

        m_vk_queue_indices.graphics = 0;
        m_vk_queue_indices.present = 0;
        m_vk_queue_indices.transfer = 0;
        m_vk_queue_indices.compute = 0;

        uint32_t i = 0;
        for (const auto &family : m_vk_phy_info.queue_family_properties) {
            if ((family.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
                (!m_vk_queue_indices.graphics.has_value())) {
                spdlog::info("graphics queue index {}", i);
                m_vk_queue_indices.graphics = i;
            }
            if ((family.queueFlags & VK_QUEUE_COMPUTE_BIT) &&
                (!m_vk_queue_indices.compute.has_value())) {
                spdlog::info("compute queue index {}", i);
                m_vk_queue_indices.compute = i;
            }
            if ((family.queueFlags & VK_QUEUE_TRANSFER_BIT) &&
                (!m_vk_queue_indices.transfer.has_value())) {
                spdlog::info("transfer queue index {}", i);
                m_vk_queue_indices.transfer = i;
            }
            VkBool32 present_support = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(
                m_vk_phy_device, i, m_vk_surface, &present_support);
            if (present_support == VK_TRUE) {
                spdlog::info("present index {}", i);
                m_vk_queue_indices.present = i;
            }
            i++;
        }
        if (!m_vk_queue_indices.graphics.has_value() ||
            !m_vk_queue_indices.present.has_value()) {
            return false;
        }
    }
    return found;
}

bool Device::initLogicDevice() {
    std::vector<VkDeviceQueueCreateInfo> queue_infos;
    std::set<uint32_t> queue_index;
    if (m_vk_queue_indices.graphics.has_value()) {
        queue_index.insert(m_vk_queue_indices.graphics.value());
    }
    if (m_vk_queue_indices.transfer.has_value()) {
        queue_index.insert(m_vk_queue_indices.transfer.value());
    }
    if (m_vk_queue_indices.present.has_value()) {
        queue_index.insert(m_vk_queue_indices.present.value());
    }
    if (m_vk_queue_indices.compute.has_value()) {
        queue_index.insert(m_vk_queue_indices.compute.value());
    }
    float p = 1.0f;
    for (const auto &index : queue_index) {
        VkDeviceQueueCreateInfo queue_info{
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = index,
            .queueCount = 1,
            .pQueuePriorities = &p,
        };

        queue_infos.push_back(queue_info);
    }

    uint32_t support_layer_count = 0;
    std::vector<VkLayerProperties> support_layeries;
    if (VK_SUCCESS == vkEnumerateDeviceLayerProperties(
                          m_vk_phy_device, &support_layer_count, nullptr)) {
        support_layeries.resize(support_layer_count);
        if (VK_SUCCESS != vkEnumerateDeviceLayerProperties(
                              m_vk_phy_device, &support_layer_count,
                              support_layeries.data())) {
            spdlog::error("failed enumerate device layer");
            return false;
        }
    } else {
        spdlog::error("failed enumerate device layer count");
        return false;
    }

    // dump all supported layer
    for (const auto &support_layer : support_layeries) {
        spdlog::info("{}", support_layer.layerName);
    }

    uint32_t support_extension_count = 0;
    std::vector<VkExtensionProperties> support_extensions;

    if (VK_SUCCESS ==
        vkEnumerateDeviceExtensionProperties(
            m_vk_phy_device, nullptr, &support_extension_count, nullptr)) {
        support_extensions.resize(support_extension_count);
        if (VK_SUCCESS !=
            vkEnumerateDeviceExtensionProperties(m_vk_phy_device, nullptr,
                                                 &support_extension_count,
                                                 support_extensions.data())) {
            spdlog::error("failed enumerate device extension");
            return false;
        }
    } else {
        spdlog::error("failed enumerate device extension");
        return false;
    }

    // dump all supported extension
    for (const auto &support_extension : support_extensions) {
        spdlog::info("{}", support_extension.extensionName);
    }

    std::vector<const char *> required_layers;
    if (m_debug) {
        required_layers.assign(validation_layers.begin(),
                               validation_layers.end());
    }

    std::vector<const char *> required_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    };

    for (const auto &required_layer : required_layers) {
        if (std::ranges::none_of(
                support_layeries, [required_layer](const auto &support_layer) {
                    return !strcmp(required_layer, support_layer.layerName);
                })) {
            spdlog::error("device layer {} not supported", required_layer);
            return false;
        }
    }

    for (const auto &required_extension : required_extensions) {
        if (std::ranges::none_of(
                support_extensions,
                [required_extension](const auto &support_extension) {
                    return !strcmp(required_extension,
                                   support_extension.extensionName);
                })) {
            spdlog::error("device extension {} not supported",
                          required_extension);
            return false;
        }
    }

    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_render_feature{
        .sType =
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
        .pNext = nullptr,
        .dynamicRendering = VK_TRUE,
    };

    VkDeviceCreateInfo info{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = (VkPhysicalDeviceDynamicRenderingFeaturesKHR
                      *)&dynamic_render_feature,
        .flags = 0,
        .queueCreateInfoCount = static_cast<uint32_t>(queue_infos.size()),
        .pQueueCreateInfos = queue_infos.data(),
        .enabledLayerCount = static_cast<uint32_t>(required_layers.size()),
        .ppEnabledLayerNames = required_layers.data(),
        .enabledExtensionCount =
            static_cast<uint32_t>(required_extensions.size()),
        .ppEnabledExtensionNames = required_extensions.data(),
        .pEnabledFeatures = &m_vk_phy_info.features,
    };

    if (VK_SUCCESS !=
        vkCreateDevice(m_vk_phy_device, &info, nullptr, &m_vk_device)) {
        return false;
    }

    if (m_vk_queue_indices.graphics.has_value()) {
        vkGetDeviceQueue(m_vk_device, m_vk_queue_indices.graphics.value(), 0,
                         &m_vk_queues.graphics);
    }
    if (m_vk_queue_indices.present.has_value()) {
        vkGetDeviceQueue(m_vk_device, m_vk_queue_indices.present.value(), 0,
                         &m_vk_queues.present);
    }

    if (m_vk_queue_indices.transfer.has_value()) {
        vkGetDeviceQueue(m_vk_device, m_vk_queue_indices.transfer.value(), 0,
                         &m_vk_queues.transfer);
    }

    if (m_vk_queue_indices.compute.has_value()) {
        vkGetDeviceQueue(m_vk_device, m_vk_queue_indices.compute.value(), 0,
                         &m_vk_queues.compute);
    }

    return true;
}

bool Device::initCmds() {
    VkCommandPoolCreateInfo info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = m_vk_queue_indices.graphics.value(),
    };
    if (VK_SUCCESS !=
        vkCreateCommandPool(m_vk_device, &info, nullptr, &m_vk_cmd_pool)) {
        spdlog::error("failed to create command pool");
        return false;
    }

    VkCommandBufferAllocateInfo alloc_info{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = m_vk_cmd_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    if (VK_SUCCESS !=
        vkAllocateCommandBuffers(m_vk_device, &alloc_info, &m_vk_cmd)) {
        spdlog::error("failed to alloc commands");
        return false;
    }

    return true;
}

bool Device::initSync() {
    VkSemaphoreCreateInfo sinfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };
    VkFenceCreateInfo finfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    if (VK_SUCCESS != vkCreateSemaphore(m_vk_device, &sinfo, nullptr,
                                        &m_vk_sync.image_available)) {
        spdlog::error("failed to create semaphore for image available");
        return false;
    }
    if (VK_SUCCESS != vkCreateSemaphore(m_vk_device, &sinfo, nullptr,
                                        &m_vk_sync.render_done)) {
        spdlog::error("failed to create semaphore for render done");
        return false;
    }
    if (VK_SUCCESS != vkCreateFence(m_vk_device, &finfo, nullptr,
                                    &m_vk_sync.in_flight_fence)) {
        spdlog::error("failed to create fence for in flight fence");
        return false;
    }
    return true;
}

void Device::updateWindowSize() {
    if (m_vk_phy_device && m_vk_surface) {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_vk_phy_device, m_vk_surface,
                                                  &m_vk_phy_info.capabilities);
    }
}

bool Device::init(const VkInstance &instance) {
    if (!pickupPhyDevice(instance)) {
        spdlog::error("unable to found sutiable physical device");
        return false;
    }
    if (!initLogicDevice()) {
        return false;
    }
    if (!initCmds()) {
        return false;
    }
    if (!initSync()) {
        return false;
    }
    return true;
}
} // namespace vbr::device
