#pragma once

#include "vulkan/vulkan_core.h"
#include <cstdint>
#include <vector>
namespace vbr::layout {

class Layout {
  private:
    VkDevice &m_device;
    VkPipelineLayout m_pipeline_layout = VK_NULL_HANDLE;

  public:
    Layout(VkDevice &device);
    ~Layout();

    bool init(const std::vector<VkDescriptorSetLayout> &dls = {});

    VkPipelineLayout &operator*() { return m_pipeline_layout; }

    Layout(Layout &) = delete;
    Layout(Layout &&) = delete;
    Layout &operator=(Layout &) = delete;
    Layout &operator=(Layout &&) = delete;
};

} // namespace vbr::layout
