#include "../../inc/layout.hpp"
#include "spdlog/spdlog.h"
#include "vulkan/vulkan_core.h"

namespace vbr::layout {

Layout::Layout(VkDevice &device) : m_device(device) {}
Layout::~Layout() {
    if (m_device != VK_NULL_HANDLE && m_pipeline_layout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
    }
}

bool Layout::init() {
    VkPipelineLayoutCreateInfo layout_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };
    if (VK_SUCCESS !=
        vkCreatePipelineLayout(m_device, &layout_info, nullptr, &m_pipeline_layout)) {
        spdlog::error("failed to create pipeline layout");
        return false;
    }
    return true;
}

} // namespace vbr::layout
