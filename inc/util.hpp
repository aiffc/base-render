#pragma once

#include "vulkan/vulkan_core.h"
#include <optional>
#include <string_view>
#include <vector>
#include <vulkan/vulkan.h>

namespace vbr::util {

VkPipelineShaderStageCreateInfo
fillPipelineShaderStage(const VkShaderStageFlagBits &stage,
                        VkShaderModule &module,
                        const VkSpecializationInfo *special_info = nullptr,
                        std::string_view name = "main");

VkPipelineVertexInputStateCreateInfo fillPipelineVertexInput(
    const std::vector<VkVertexInputBindingDescription> &binding,
    const std::vector<VkVertexInputAttributeDescription> &attribute);

VkPipelineInputAssemblyStateCreateInfo
fillPipelineInutAssembly(const VkPrimitiveTopology &topology,
                         bool restart_enable = false);

VkPipelineTessellationStateCreateInfo fillPipelineTessllation();

VkPipelineViewportStateCreateInfo
fillPipelineViewport(const std::vector<VkViewport> &viewports,
                     const std::vector<VkRect2D> &rects);

VkPipelineRasterizationStateCreateInfo fillPipelineRasterization(
    const VkPolygonMode &polygon_mode, float line_width = 1.0f,
    const VkCullModeFlags &cull_mode = VK_CULL_MODE_BACK_BIT,
    const VkFrontFace &front_face = VK_FRONT_FACE_CLOCKWISE,
    bool depth_clamp_enable = false, bool discard_enable = false,
    bool depth_bias_enable = false, float depth_bias_constant_factor = 0.0f,
    float depth_bias_clamp = 0.0f, float depth_bias_slope_factor = 0.0f);

VkPipelineMultisampleStateCreateInfo fillPipelineMultisample(
    const VkSampleCountFlagBits &samples = VK_SAMPLE_COUNT_1_BIT,
    bool shading_enable = false, float min_shading = 1.0f,
    const VkSampleMask *mask = nullptr, bool alpha2coverage_enable = false,
    bool alpha2one_enable = false);

VkPipelineDepthStencilStateCreateInfo fillPipelineDepthStencil(
    bool test_enable = false, bool write_enable = false,
    const VkCompareOp &compare_op = VK_COMPARE_OP_NEVER,
    bool bounds_test_enable = false, bool stencil_test_enable = false,
    const VkStencilOpState &front = {}, const VkStencilOpState &back = {},
    float min = 0.0f, float max = 0.0f);

VkPipelineColorBlendStateCreateInfo fillPipelineColorBlend(
    const std::vector<VkPipelineColorBlendAttachmentState> &attachments,
    bool logic_op_enable = false, const VkLogicOp &logic_op = VK_LOGIC_OP_COPY,
    float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f);

VkPipelineDynamicStateCreateInfo
fillPipelineDynamicState(const std::vector<VkDynamicState> &states);

struct GPUInfo {
    VkPhysicalDeviceFeatures features;
    // format properties
    // image format properties
    VkPhysicalDeviceMemoryProperties memory_properties;
    VkPhysicalDeviceProperties properties;
    std::vector<VkQueueFamilyProperties> queue_family_properties;
    VkPresentModeKHR present_mode;
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR surface_format;
};

void transitionImageLayout(VkCommandBuffer &cmd, VkImage &image,
                           VkImageLayout old_layout, VkImageLayout new_layout);

struct QueueFamilyIndices {
    std::optional<uint32_t> graphics;
    std::optional<uint32_t> transfer;
    std::optional<uint32_t> present;
    std::optional<uint32_t> compute;
};
} // namespace vbr::util
