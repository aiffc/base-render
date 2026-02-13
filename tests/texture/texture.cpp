#include "texture.hpp"
#include "vulkan/vulkan_core.h"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <spdlog/spdlog.h>

App::~App() { quit(); }

bool App::init(SDL_InitFlags flag, VkSampleCountFlagBits sample_count) {
    if (!vbr::app::App::init(flag, sample_count)) {
        return false;
    }

    m_descriptor = std::make_unique<vbr::descriptor::Descriptor>(**m_vk_device);
    m_descriptor->addDescriptorBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    m_descriptor->addDescriptorBinding(
        1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        VK_SHADER_STAGE_FRAGMENT_BIT);
    if (!m_descriptor->init()) {
        return false;
    }

    m_layout = std::make_unique<vbr::layout::Layout>(**m_vk_device);
    if (!m_layout->init({**m_descriptor})) {
        return false;
    }

    m_pipeline = std::make_unique<vbr::gpipeline::Pipeline>(*m_vk_device);
    m_pipeline->addShader(VK_SHADER_STAGE_VERTEX_BIT,
                          "../tests/shaders/texture/vert.spv");
    m_pipeline->addShader(VK_SHADER_STAGE_FRAGMENT_BIT,
                          "../tests/shaders/texture/frag.spv");
    m_pipeline->addViewport(static_cast<float>(m_window_size.x),
                            static_cast<float>(m_window_size.y));
    m_pipeline->addScissor(m_window_size.x, m_window_size.y);
    m_pipeline->addColorBlendAttachemt();
    m_pipeline->addBinding(0, sizeof(VertexInfo));
    m_pipeline->addAttribute(0, 0, VK_FORMAT_R32G32_SFLOAT,
                             offsetof(VertexInfo, pos));
    m_pipeline->addAttribute(1, 0, VK_FORMAT_R32G32B32_SFLOAT,
                             offsetof(VertexInfo, color));
    m_pipeline->addAttribute(2, 0, VK_FORMAT_R32G32_SFLOAT,
                             offsetof(VertexInfo, coord));
    m_pipeline->frontFace(VK_FRONT_FACE_COUNTER_CLOCKWISE);
    if (!m_pipeline->init(**m_layout)) {
        return false;
    }

    const std::vector<VertexInfo> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.5f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.5f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.5f, 0.5f}},
    };
    m_vbuffer = m_vk_device->createUsageBuffer<VertexInfo>(
        vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    std::vector<uint32_t> indexs{0, 1, 2, 2, 3, 0};
    m_ibuffer = m_vk_device->createUsageBuffer<uint32_t>(
        indexs, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

    m_uniform = m_vk_device->createUniformBuffer<UniformBufferObject>();
    m_descriptor->updateBuffer(*m_uniform, 0, 0,
                               VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

    m_texture = m_vk_device->createTexture("../asset/test.png");
    m_descriptor->updateTexture(*m_texture, 1, 0);
    return true;
}

void App::update() {
    vbr::app::App::update();
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(
                     currentTime - startTime)
                     .count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f),
                            glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view =
        glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj =
        glm::perspective(glm::radians(45.0f),
                         m_window_size.x / (float)m_window_size.y, 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    memcpy(m_uniform->data, &ubo, sizeof(ubo));
}

void App::event(SDL_Event *event) { vbr::app::App::event(event); }

void App::render() {
    if (begin()) {
        bindPipeline(*m_pipeline);
        bindDescriptorSet(m_descriptor->set(), **m_layout);
        bindVertex(*m_vbuffer);
        bindIndex(*m_ibuffer);
        setViewport();
        setScissor();
        drawIndex(6);
        end();
    }
}

void App::quit() {
    m_texture.reset();
    m_uniform.reset();
    m_ibuffer.reset();
    m_vbuffer.reset();
    m_descriptor.reset();
    m_layout.reset();
    m_pipeline.reset();
}
