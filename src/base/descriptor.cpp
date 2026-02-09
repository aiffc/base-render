#include "../../inc/descriptor.hpp"
#include "../../inc/buffer.hpp"
#include "vulkan/vulkan_core.h"
#include <spdlog/spdlog.h>

namespace vbr::descriptor {

Descriptor::Descriptor(VkDevice &device) : m_device(device) {}
Descriptor::~Descriptor() {
    if (m_device != VK_NULL_HANDLE && m_descriptor_layout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(m_device, m_descriptor_layout, nullptr);
        m_descriptor_layout = VK_NULL_HANDLE;
    }
    if (m_device != VK_NULL_HANDLE && m_descriptor_set != VK_NULL_HANDLE) {
        vkFreeDescriptorSets(m_device, m_descriptor_pool, 1, &m_descriptor_set);
        m_descriptor_set = VK_NULL_HANDLE;
    }
    if (m_device != VK_NULL_HANDLE && m_descriptor_pool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(m_device, m_descriptor_pool, nullptr);
        m_descriptor_pool = VK_NULL_HANDLE;
    }
}

void Descriptor::addDescriptorBinding(uint32_t binding, VkDescriptorType type,
                                      uint32_t count, VkShaderStageFlags flags,
                                      const VkSampler *sampler) {
    VkDescriptorSetLayoutBinding v{
        .binding = binding,
        .descriptorType = type,
        .descriptorCount = count,
        .stageFlags = flags,
        .pImmutableSamplers = sampler,

    };
    m_descriptor_bindings.push_back(v);
}

void Descriptor::addPoolSize(VkDescriptorType type, uint32_t count) {
    VkDescriptorPoolSize v{
        .type = type,
        .descriptorCount = count,
    };
    m_pool_size.push_back(v);
}

bool Descriptor::init() {
    if (!m_descriptor_bindings.empty()) {
        VkDescriptorSetLayoutCreateInfo dlayout_info{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .bindingCount = static_cast<uint32_t>(m_descriptor_bindings.size()),
            .pBindings = m_descriptor_bindings.data(),
        };

        if (VK_SUCCESS != vkCreateDescriptorSetLayout(m_device, &dlayout_info,
                                                      nullptr,
                                                      &m_descriptor_layout)) {
            spdlog::error("failed to create descriptor set layout");
            return false;
        }
        VkDescriptorPoolCreateInfo pcinfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .maxSets = 1,
            .poolSizeCount = static_cast<uint32_t>(m_pool_size.size()),
            .pPoolSizes = m_pool_size.data(),
        };
        if (VK_SUCCESS != vkCreateDescriptorPool(m_device, &pcinfo, nullptr,
                                                 &m_descriptor_pool)) {
            spdlog::error("failed to create descriptor set layout");
            return false;
        }
        VkDescriptorSetAllocateInfo dsalloc_info{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool = m_descriptor_pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &m_descriptor_layout,
        };
        if (VK_SUCCESS != vkAllocateDescriptorSets(m_device, &dsalloc_info,
                                                   &m_descriptor_set)) {
            spdlog::error("failed to alloc descriptor sets");
            return false;
        }
    } else {
        spdlog::warn("invalid descriptor");
        return true;
    }
    return true;
}

void Descriptor::updateBuffer(const vbr::buffer::Buffer &buffer,
                              uint32_t dst_binding, uint32_t dst_array_element,
                              VkDescriptorType type) {
    VkDescriptorBufferInfo buffer_info{
        .buffer = buffer.buffer,
        .offset = 0,
        .range = buffer.size,

    };
    VkWriteDescriptorSet write_info{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_descriptor_set,
        .dstBinding = dst_binding,
        .dstArrayElement = dst_array_element,
        .descriptorCount = 1,
        .descriptorType = type,
        .pImageInfo = nullptr,
        .pBufferInfo = &buffer_info,
        .pTexelBufferView = nullptr,
    };
    vkUpdateDescriptorSets(m_device, 1, &write_info, 0, nullptr);
}
} // namespace vbr::descriptor
