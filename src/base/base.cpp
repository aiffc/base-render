#include "base.hpp"
#include "spdlog/spdlog.h"
#include "vulkan/vulkan_core.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_vulkan.h>
#include <algorithm>
#include <cstring>
#include <memory>
#include <set>
#include <spdlog/common.h>
#include <vector>

const std::vector<char const *> validation_layers = {
    "VK_LAYER_KHRONOS_validation",
};

App::App(const glm::ivec2 &window_size) : m_window_size(window_size) {}
App::~App() = default;

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType [[maybe_unused]],
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
              void *pUserData [[maybe_unused]]) {
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        spdlog::trace("vk dbg {}", pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        spdlog::info("vk dbg {}", pCallbackData->pMessage);
    } else if (messageSeverity &
               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        spdlog::warn("vk dbg {}", pCallbackData->pMessage);
    } else if (messageSeverity &
               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        spdlog::error("vk dbg {}", pCallbackData->pMessage);
    }
    // spdlog::error("vk dbg {}", pCallbackData->pMessage);
    return VK_FALSE;
}

bool App::initInstance() {
    // check layer
    uint32_t support_layer_count = 0;
    std::vector<VkLayerProperties> support_layers;
    if (VK_SUCCESS ==
        vkEnumerateInstanceLayerProperties(&support_layer_count, nullptr)) {
        support_layers.resize(support_layer_count);
        if (VK_SUCCESS != vkEnumerateInstanceLayerProperties(
                              &support_layer_count, support_layers.data())) {
            spdlog::error("failed to enumerate instance layer");
            return false;
        }
    } else {
        spdlog::error("failed to enumerate instance layer");
        return false;
    }
    // dump all supported layer
    for (auto &support_layer : support_layers) {
        spdlog::info("{}", support_layer.layerName);
    }
    std::vector<char const *> required_layers;
    if (m_debug) {
        required_layers.assign(validation_layers.begin(),
                               validation_layers.end());
        for (const auto &required_layer : required_layers) {
            if (std::ranges::none_of(
                    support_layers,
                    [required_layer](const auto &support_layer) {
                        return !strncmp(support_layer.layerName, required_layer,
                                        strlen(required_layer));
                    })) {
                spdlog::error("{} layer not support", required_layer);
                return false;
            }
        }
    }

    // check extension
    uint32_t sdl_extension_count = 0;
    auto sdl_extensions =
        SDL_Vulkan_GetInstanceExtensions(&sdl_extension_count);
    // required extensions
    std::vector<char const *> required_extensions(
        sdl_extensions, sdl_extensions + sdl_extension_count);
    if (m_debug) {
        required_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    // enabled extensions
    uint32_t support_extension_count = 0;
    std::vector<VkExtensionProperties> support_extensions;
    if (VK_SUCCESS == vkEnumerateInstanceExtensionProperties(
                          nullptr, &support_extension_count, nullptr)) {
        support_extensions.resize(support_extension_count);
        if (VK_SUCCESS !=
            vkEnumerateInstanceExtensionProperties(
                nullptr, &support_extension_count, support_extensions.data())) {
            spdlog::error("failed to enumerate instance extensions");
            return false;
        }
    } else {
        spdlog::error("failed to enumerate instance extensions");
        return false;
    }
    // dump all supported extension
    for (auto &support_extension : support_extensions) {
        spdlog::info("{}", support_extension.extensionName);
    }
    // check all required extension support
    for (const auto &extension : required_extensions) {
        if (std::ranges::none_of(
                support_extensions, [extension](const auto &support_extension) {
                    return !strncmp(support_extension.extensionName, extension,
                                    strlen(extension));
                })) {
            spdlog::error("{} extension not support", extension);
            return false;
        }
    }

    VkApplicationInfo app_info{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "vbr",
        .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
        .pEngineName = "",
        .engineVersion = VK_MAKE_VERSION(0, 0, 0),
        .apiVersion = VK_API_VERSION_1_3,
    };

    VkInstanceCreateInfo info{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &app_info,
        .enabledLayerCount = static_cast<uint32_t>(required_layers.size()),
        .ppEnabledLayerNames = required_layers.data(),
        .enabledExtensionCount =
            static_cast<uint32_t>(required_extensions.size()),
        .ppEnabledExtensionNames = required_extensions.data(),
    };

    VkDebugUtilsMessengerCreateInfoEXT dbg_message_info{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                       VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugCallback,
        .pUserData = nullptr,
    };

    if (m_debug) {
        info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&dbg_message_info;
    }

    if (VK_SUCCESS != vkCreateInstance(&info, nullptr, &m_vk_instance)) {
        spdlog::error("failed to create vulkan instance");
        return false;
    }

    if (m_debug) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            m_vk_instance, "vkCreateDebugUtilsMessengerEXT");
        if (func) {
            if (VK_SUCCESS != func(m_vk_instance, &dbg_message_info, nullptr,
                                   &m_vk_dbg_messager)) {
                spdlog::error("failed to create debug messager");
                return false;
            }
        }
    }

    return true;
}

bool App::initSurface() {
    return SDL_Vulkan_CreateSurface(m_window.get(), m_vk_instance, nullptr,
                                    &m_vk_surface);
}

bool App::pickupPhyDevice() {
    uint32_t count = 0;
    if (VK_SUCCESS !=
        vkEnumeratePhysicalDevices(m_vk_instance, &count, nullptr)) {
        spdlog::error("failed to pickup physical device");
        return false;
    }
    std::vector<VkPhysicalDevice> physical_devices{count};
    if (VK_SUCCESS != vkEnumeratePhysicalDevices(m_vk_instance, &count,
                                                 physical_devices.data())) {
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

            m_vk_phy_info.features = features;
            m_vk_phy_info.properties = properties;
            vkGetPhysicalDeviceMemoryProperties(
                m_vk_phy_device, &m_vk_phy_info.memory_properties);
            uint32_t pcount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(m_vk_phy_device, &pcount,
                                                     nullptr);
            m_vk_phy_info.queue_family_properties.resize(pcount);
            vkGetPhysicalDeviceQueueFamilyProperties(
                m_vk_phy_device, &pcount,
                m_vk_phy_info.queue_family_properties.data());
            break;
        }
    }

    // select the bad one
    if (!found && count != 0) {
        spdlog::warn("no sutiable device, select the bad one");
        m_vk_phy_device = physical_devices[0];
        found = true;
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
    }

    if (found) {
        m_vk_queue_indices.graphics = std::nullopt;
        m_vk_queue_indices.transfer = std::nullopt;
        m_vk_queue_indices.present = std::nullopt;
        m_vk_queue_indices.compute = std::nullopt;
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
    }
    return found;
}

bool App::initLogicDevice() {
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

    VkDeviceCreateInfo info{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = static_cast<uint32_t>(queue_infos.size()),
        .pQueueCreateInfos = queue_infos.data(),
        .enabledLayerCount = static_cast<uint32_t>(required_layers.size()),
        .ppEnabledLayerNames = required_layers.data(),
        .enabledExtensionCount =
            static_cast<uint32_t>(required_extensions.size()),
        .ppEnabledExtensionNames = required_extensions.data(),
        .pEnabledFeatures = nullptr,
    };

    if (VK_SUCCESS !=
        vkCreateDevice(m_vk_phy_device, &info, nullptr, &m_vk_device)) {
        return false;
    }
    return true;
}

bool App::init(SDL_InitFlags flags) {
    if (m_debug) {
        spdlog::set_level(spdlog::level::info);
    }
    if (!SDL_Init(flags)) {
        spdlog::error("sdl init failed", SDL_GetError());
        return false;
    }

    SDL_Window *raw_window = SDL_CreateWindow(
        "vbr", m_window_size.x, m_window_size.y, SDL_WINDOW_VULKAN);

    if (raw_window == nullptr) {
        spdlog::error("sdl create window failed {}", SDL_GetError());
        return false;
    }

    m_window = std::unique_ptr<SDL_Window, WindowDeleter>(raw_window);

    if (!initInstance()) {
        return false;
    }
    if (!initSurface()) {
        spdlog::error("sdl init vulkan surface failed {}", SDL_GetError());
        return false;
    }
    if (!pickupPhyDevice()) {
        spdlog::error("unable to found sutiable physical device");
        return false;
    }
    if (!initLogicDevice()) {
        return false;
    }
    return true;
}

void App::update() {}

void App::event(SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        m_quit = true;
    } else {
        m_quit = false;
    }
}

void App::render() {}

void App::quit() {
    if (m_vk_device) {
        vkDestroyDevice(m_vk_device, nullptr);
    }

    if (m_vk_surface) {
        vkDestroySurfaceKHR(m_vk_instance, m_vk_surface, nullptr);
    }

    if (m_debug && m_vk_dbg_messager) {
        auto fun = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            m_vk_instance, "vkDestroyDebugUtilsMessengerEXT");
        if (fun) {
            fun(m_vk_instance, m_vk_dbg_messager, nullptr);
        }
    }

    if (m_vk_instance) {
        vkDestroyInstance(m_vk_instance, nullptr);
    }
}
