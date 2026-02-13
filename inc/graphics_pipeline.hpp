#pragma once

#include "device.hpp"
#include <string_view>
#include <vector>
#include <vulkan/vulkan.h>

namespace vbr::gpipeline {

class Pipeline {
  private:
    vbr::device::Device &m_device;
    VkPipeline m_pipeline = VK_NULL_HANDLE;

    std::vector<VkPipelineShaderStageCreateInfo> m_shader_stages;
    std::vector<VkVertexInputBindingDescription> m_vertex_bindings;
    std::vector<VkVertexInputAttributeDescription> m_vertex_attributes;
    std::vector<VkViewport> m_viewports;
    std::vector<VkRect2D> m_scissors;
    std::vector<VkPipelineColorBlendAttachmentState> m_color_blend_attachment;
    VkPrimitiveTopology m_topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkPolygonMode m_polygon_mode = VK_POLYGON_MODE_FILL;
    VkFrontFace m_rasterization_front_face = VK_FRONT_FACE_CLOCKWISE;

  private:
    VkShaderModule createShaderModule(std::string_view path);
    void destroyAllShaderModule();

  public:
    Pipeline(vbr::device::Device &device);
    ~Pipeline();

    bool init(VkPipelineLayout &layout);

    void addShader(const VkShaderStageFlagBits &stage,
                   std::string_view shader_path,
                   const VkSpecializationInfo *special_info = nullptr,
                   std::string_view name = "main");

    void addViewport(float w, float h, float x = 0.0f, float y = 0.0f,
                     float min = 0.0f, float max = 1.0f);

    void addScissor(uint32_t w, uint32_t h, int32_t x = 0, int32_t y = 0);

    void addColorBlendAttachemt(
        const VkColorComponentFlags &color_write_mask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        bool blend_enable = false,
        const VkBlendFactor &src_color_blend_factor = VK_BLEND_FACTOR_ZERO,
        const VkBlendFactor &dst_color_blend_factor = VK_BLEND_FACTOR_ZERO,
        const VkBlendOp &color_blend_op = VK_BLEND_OP_ADD,
        const VkBlendFactor &src_alpha_blend_factor = VK_BLEND_FACTOR_ZERO,
        const VkBlendFactor &dst_alpha_blend_factor = VK_BLEND_FACTOR_ZERO,
        const VkBlendOp &alpha_blend_op = VK_BLEND_OP_ADD);

    void addBinding(uint32_t binding, uint32_t stride,
                    VkVertexInputRate rate = VK_VERTEX_INPUT_RATE_VERTEX);

    void addAttribute(uint32_t location, uint32_t binding, VkFormat format,
                      uint32_t offset);

    void frontFace(VkFrontFace v) { m_rasterization_front_face = v; }

    VkPipeline operator*() { return m_pipeline; }
};

} // namespace vbr::gpipeline
