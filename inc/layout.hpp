#pragma once

#include "vulkan/vulkan_core.h"
namespace vbr::layout {

class Layout {
  private:
    VkDevice &m_device;
    VkPipelineLayout m_pipeline_layout;

  public:
    Layout(VkDevice &device);
    ~Layout();

    bool init();

    VkPipelineLayout &operator*() { return m_pipeline_layout; }

    Layout(Layout &) = delete;
    Layout(Layout &&) = delete;
    Layout &operator=(Layout &) = delete;
    Layout &operator=(Layout &&) = delete;
};

} // namespace vbr::layout
