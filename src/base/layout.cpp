#include "../../inc/layout.hpp"
#include "spdlog/spdlog.h"
#include "vulkan/vulkan_core.h"
#include <cstdint>

namespace vbr::layout {

Layout::Layout(VkDevice &device) : m_device(device) {}
Layout::~Layout() {
    if (m_device != VK_NULL_HANDLE && m_pipeline_layout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
        m_pipeline_layout = VK_NULL_HANDLE;
    }
}

bool Layout::init(const std::vector<VkDescriptorSetLayout> &dls) {
    VkPipelineLayoutCreateInfo layout_info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = static_cast<uint32_t>(dls.size()),
        .pSetLayouts = dls.data(),
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };

    if (VK_SUCCESS != vkCreatePipelineLayout(m_device, &layout_info, nullptr,
                                             &m_pipeline_layout)) {
        spdlog::error("failed to create pipeline layout");
        return false;
    }

    return true;
}

} // namespace vbr::layout
