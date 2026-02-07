#include "../../inc/graphics_pipeline.hpp"
#include "../../inc/util.hpp"
#include "spdlog/spdlog.h"
#include <SDL3/SDL_iostream.h>
#include <cstdint>
#include <vector>

namespace vbr::gpipeline {

Pipeline::Pipeline(VkDevice &device) : m_device(device) {}

Pipeline::~Pipeline() {
    if (m_device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(m_device);
    }

    if (m_device != VK_NULL_HANDLE && m_layout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device, m_layout, nullptr);
        m_layout = VK_NULL_HANDLE;
    }

    if (m_device != VK_NULL_HANDLE && m_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device, m_pipeline, nullptr);
        m_pipeline = VK_NULL_HANDLE;
    }
}

VkShaderModule Pipeline::createShaderModule(std::string_view path) {
    if (m_device == VK_NULL_HANDLE) {
        spdlog::error("invalid pipeline {}", __LINE__);
        return VK_NULL_HANDLE;
    }
    size_t size = 0;
    void *code = SDL_LoadFile(path.data(), &size);

    VkShaderModuleCreateInfo info{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = size,
        .pCode = static_cast<uint32_t *>(code),
    };
    VkShaderModule module = VK_NULL_HANDLE;
    if (VK_SUCCESS != vkCreateShaderModule(m_device, &info, nullptr, &module)) {
        spdlog::error("failed to create shader module");
        return VK_NULL_HANDLE;
    }
    return module;
}

void Pipeline::destroyAllShaderModule() {
    if (m_device == VK_NULL_HANDLE) {
        spdlog::error("invalid pipeline {}", __LINE__);
        return;
    }
    for (auto &info : m_shader_stages) {
        if (info.module != VK_NULL_HANDLE) {
            vkDestroyShaderModule(m_device, info.module, nullptr);
        }
    }
}

void Pipeline::addShader(const VkShaderStageFlagBits &stage,
                         std::string_view shader_path,
                         const VkSpecializationInfo *special_info,
                         std::string_view name) {
    VkShaderModule module = createShaderModule(shader_path);
    VkPipelineShaderStageCreateInfo info{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = stage,
        .module = module,
        .pName = name.data(),
        .pSpecializationInfo = special_info,
    };
    m_shader_stages.push_back(info);
}

bool Pipeline::init() {
    if (m_device == VK_NULL_HANDLE) {
        spdlog::error("invalid graphics pipeline {}", __LINE__);
        return false;
    }
    VkPipelineVertexInputStateCreateInfo vertex_input_info =
        vbr::util::fillPipelineVertexInput(m_vertex_bindings,
                                           m_vertex_attributes);
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info =
        vbr::util::fillPipelineInutAssembly(m_topology);
    VkPipelineTessellationStateCreateInfo tessellation_info =
        vbr::util::fillPipelineTessllation();
    VkPipelineViewportStateCreateInfo viewport_info =
        vbr::util::fillPipelineViewport(m_viewports, m_scissors);
    VkPipelineRasterizationStateCreateInfo rasterization_info =
        vbr::util::fillPipelineRasterization(m_polygon_mode);
    VkPipelineMultisampleStateCreateInfo multiple_sample_info =
        vbr::util::fillPipelineMultisample();
    VkPipelineDepthStencilStateCreateInfo depth_stencil_info =
        vbr::util::fillPipelineDepthStencil();
    VkPipelineColorBlendStateCreateInfo color_blend_info =
        vbr::util::fillPipelineColorBlend(m_color_blend_attachment);

    std::vector<VkDynamicState> dynamic_state = {VK_DYNAMIC_STATE_VIEWPORT,
                                                 VK_DYNAMIC_STATE_SCISSOR,
                                                 VK_DYNAMIC_STATE_LINE_WIDTH};
    VkPipelineDynamicStateCreateInfo dynamic_info =
        vbr::util::fillPipelineDynamicState(dynamic_state);

    // TODO warp ass object
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
        vkCreatePipelineLayout(m_device, &layout_info, nullptr, &m_layout)) {
        spdlog::error("failed to create pipeline layout");
        return false;
    }

    VkGraphicsPipelineCreateInfo info{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stageCount = static_cast<uint32_t>(m_shader_stages.size()),
        .pStages = m_shader_stages.data(),
        .pVertexInputState = &vertex_input_info,
        .pInputAssemblyState = &input_assembly_info,
        .pTessellationState = &tessellation_info,
        .pViewportState = &viewport_info,
        .pRasterizationState = &rasterization_info,
        .pMultisampleState = &multiple_sample_info,
        .pDepthStencilState = &depth_stencil_info,
        .pColorBlendState = &color_blend_info,
        .pDynamicState = &dynamic_info,
        .layout = m_layout,
        .renderPass = VK_NULL_HANDLE,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0,
    };

    if (VK_SUCCESS != vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1,
                                                &info, nullptr, &m_pipeline)) {
        spdlog::error("failed to create graphics pipeline");
        return false;
    }
    destroyAllShaderModule();
    return true;
}

void Pipeline::addViewport(float w, float h, float x, float y, float min,
                           float max) {
    VkViewport v{
        .x = x,
        .y = y,
        .width = w,
        .height = h,
        .minDepth = min,
        .maxDepth = max,
    };
    m_viewports.push_back(v);
}
void Pipeline::addScissor(uint32_t w, uint32_t h, int32_t x, int32_t y) {
    VkRect2D v{
        .offset =
            {
                .x = x,
                .y = y,
            },
        .extent =
            {
                .width = w,
                .height = h,

            },

    };
    m_scissors.push_back(v);
}

void Pipeline::addColorBlendAttachemt(
    const VkColorComponentFlags &color_write_mask, bool blend_enable,
    const VkBlendFactor &src_color_blend_factor,
    const VkBlendFactor &dst_color_blend_factor,
    const VkBlendOp &color_blend_op,
    const VkBlendFactor &src_alpha_blend_factor,
    const VkBlendFactor &dst_alpha_blend_factor,
    const VkBlendOp &alpha_blend_op) {
    VkPipelineColorBlendAttachmentState v{
        .blendEnable = blend_enable ? VK_TRUE : VK_FALSE,
        .srcColorBlendFactor = src_color_blend_factor,
        .dstColorBlendFactor = dst_color_blend_factor,
        .colorBlendOp = color_blend_op,
        .srcAlphaBlendFactor = src_alpha_blend_factor,
        .dstAlphaBlendFactor = dst_alpha_blend_factor,
        .alphaBlendOp = alpha_blend_op,
        .colorWriteMask = color_write_mask,
    };
    m_color_blend_attachment.push_back(v);
}

} // namespace vbr::gpipeline
