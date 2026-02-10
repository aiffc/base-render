#include "../../inc/util.hpp"
#include <cstdint>
#include <spdlog/spdlog.h>

namespace vbr::util {

VkPipelineShaderStageCreateInfo fillPipelineShaderStage(
    const VkShaderStageFlagBits &stage, VkShaderModule &module,
    const VkSpecializationInfo *special_info, std::string_view name) {
    VkPipelineShaderStageCreateInfo ret{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = stage,
        .module = module,
        .pName = name.data(),
        .pSpecializationInfo = special_info,
    };
    return ret;
}

VkPipelineVertexInputStateCreateInfo fillPipelineVertexInput(
    const std::vector<VkVertexInputBindingDescription> &binding,
    const std::vector<VkVertexInputAttributeDescription> &attribute) {
    VkPipelineVertexInputStateCreateInfo ret{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = static_cast<uint32_t>(binding.size()),
        .pVertexBindingDescriptions = binding.data(),
        .vertexAttributeDescriptionCount =
            static_cast<uint32_t>(attribute.size()),
        .pVertexAttributeDescriptions = attribute.data(),
    };
    return ret;
}

VkPipelineInputAssemblyStateCreateInfo
fillPipelineInutAssembly(const VkPrimitiveTopology &topology,
                         bool restart_enable) {
    VkPipelineInputAssemblyStateCreateInfo ret{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = topology,
        .primitiveRestartEnable = restart_enable ? VK_TRUE : VK_FALSE,
    };
    return ret;
}

VkPipelineTessellationStateCreateInfo fillPipelineTessllation() {
    VkPipelineTessellationStateCreateInfo ret{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .patchControlPoints = 0,
    };
    return ret;
}

VkPipelineViewportStateCreateInfo
fillPipelineViewport(const std::vector<VkViewport> &viewports,
                     const std::vector<VkRect2D> &rects) {
    VkPipelineViewportStateCreateInfo ret{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .viewportCount = static_cast<uint32_t>(viewports.size()),
        .pViewports = viewports.data(),
        .scissorCount = static_cast<uint32_t>(rects.size()),
        .pScissors = rects.data(),
    };
    return ret;
}

VkPipelineRasterizationStateCreateInfo fillPipelineRasterization(
    const VkPolygonMode &polygon_mode, float line_width,
    const VkCullModeFlags &cull_mode, const VkFrontFace &front_face,
    bool depth_clamp_enable, bool discard_enable, bool depth_bias_enable,
    float depth_bias_constant_factor, float depth_bias_clamp,
    float depth_bias_slope_factor) {
    VkPipelineRasterizationStateCreateInfo ret{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthClampEnable = depth_clamp_enable ? VK_TRUE : VK_FALSE,
        .rasterizerDiscardEnable = discard_enable ? VK_TRUE : VK_FALSE,
        .polygonMode = polygon_mode,
        .cullMode = cull_mode,
        .frontFace = front_face,
        .depthBiasEnable = depth_bias_enable ? VK_TRUE : VK_FALSE,
        .depthBiasConstantFactor = depth_bias_constant_factor,
        .depthBiasClamp = depth_bias_clamp,
        .depthBiasSlopeFactor = depth_bias_slope_factor,
        .lineWidth = line_width,
    };
    return ret;
}

VkPipelineMultisampleStateCreateInfo
fillPipelineMultisample(const VkSampleCountFlagBits &samples,
                        bool shading_enable, float min_shading,
                        const VkSampleMask *mask, bool alpha2coverage_enable,
                        bool alpha2one_enable) {
    VkPipelineMultisampleStateCreateInfo ret{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizationSamples = samples,
        .sampleShadingEnable = shading_enable,
        .minSampleShading = min_shading,
        .pSampleMask = mask,
        .alphaToCoverageEnable = alpha2coverage_enable ? VK_TRUE : VK_FALSE,
        .alphaToOneEnable = alpha2one_enable ? VK_TRUE : VK_FALSE,
    };
    return ret;
}

VkPipelineDepthStencilStateCreateInfo
fillPipelineDepthStencil(bool test_enable, bool write_enable,
                         const VkCompareOp &compare_op, bool bounds_test_enable,
                         bool stencil_test_enable,
                         const VkStencilOpState &front,
                         const VkStencilOpState &back, float min, float max) {
    VkPipelineDepthStencilStateCreateInfo ret{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthTestEnable = test_enable ? VK_TRUE : VK_FALSE,
        .depthWriteEnable = write_enable ? VK_TRUE : VK_FALSE,
        .depthCompareOp = compare_op,
        .depthBoundsTestEnable = bounds_test_enable ? VK_TRUE : VK_FALSE,
        .stencilTestEnable = stencil_test_enable ? VK_TRUE : VK_FALSE,
        .front = front,
        .back = back,
        .minDepthBounds = min,
        .maxDepthBounds = max,
    };
    return ret;
}

VkPipelineColorBlendStateCreateInfo fillPipelineColorBlend(
    const std::vector<VkPipelineColorBlendAttachmentState> &attachments,
    bool logic_op_enable, const VkLogicOp &logic_op, float r, float g, float b,
    float a) {
    VkPipelineColorBlendStateCreateInfo ret{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = logic_op_enable ? VK_TRUE : VK_FALSE,
        .logicOp = logic_op,
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments = attachments.data(),
        .blendConstants = {r, g, b, a},
    };
    return ret;
}

VkPipelineDynamicStateCreateInfo
fillPipelineDynamicState(const std::vector<VkDynamicState> &states) {
    VkPipelineDynamicStateCreateInfo ret{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = static_cast<uint32_t>(states.size()),
        .pDynamicStates = states.data(),
    };
    return ret;
}

void transitionImageLayout(VkCommandBuffer &cmd, VkImage &image,
                           VkImageLayout old_layout, VkImageLayout new_layout) {
    VkImageMemoryBarrier barrier{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = nullptr,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .oldLayout = old_layout,
        .newLayout = new_layout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
    };
    VkPipelineStageFlags source_stage = VK_PIPELINE_STAGE_NONE_KHR;
    VkPipelineStageFlags destination_stage = VK_PIPELINE_STAGE_NONE_KHR;

    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
        new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    } else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
               new_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    } else if (old_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL &&
               new_layout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = 0;

        source_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        destination_stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    } else {
        spdlog::warn("unknow layout transmit");
    }
    if (source_stage != VK_PIPELINE_STAGE_NONE_KHR &&
        destination_stage != VK_PIPELINE_STAGE_NONE_KHR) {
        vkCmdPipelineBarrier(cmd, source_stage, destination_stage, 0, 0,
                             nullptr, 0, nullptr, 1, &barrier);
    }
}
} // namespace vbr::util
