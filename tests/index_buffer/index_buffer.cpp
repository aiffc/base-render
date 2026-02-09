#include "index_buffer.hpp"
#include "vulkan/vulkan_core.h"
#include <cstddef>
#include <cstdint>

App::~App() { quit(); }

bool App::init(SDL_InitFlags flag) {
    if (!vbr::app::App::init(flag)) {
        return false;
    }
    m_pipeline = std::make_unique<vbr::gpipeline::Pipeline>(**m_vk_device);
    m_pipeline->addShader(VK_SHADER_STAGE_VERTEX_BIT,
                          "../tests/shaders/buffer_triangle/vert.spv");
    m_pipeline->addShader(VK_SHADER_STAGE_FRAGMENT_BIT,
                          "../tests/shaders/buffer_triangle/frag.spv");
    m_pipeline->addViewport(static_cast<float>(m_window_size.x),
                            static_cast<float>(m_window_size.y));
    m_pipeline->addScissor(m_window_size.x, m_window_size.y);
    m_pipeline->addColorBlendAttachemt();
    m_pipeline->addBinding(0, sizeof(VertexInfo));
    m_pipeline->addAttribute(0, 0, VK_FORMAT_R32G32_SFLOAT,
                             offsetof(VertexInfo, pos));
    m_pipeline->addAttribute(1, 0, VK_FORMAT_R32G32B32_SFLOAT,
                             offsetof(VertexInfo, color));
    if (!m_pipeline->init()) {
        return false;
    }

    const std::vector<VertexInfo> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
    };
    m_vbuffer = m_vk_device->createUsageBuffer<VertexInfo>(
        vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    std::vector<uint32_t> indexs{0, 1, 2, 2, 3, 0};
    m_ibuffer = m_vk_device->createUsageBuffer<uint32_t>(
        indexs, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    return true;
}

void App::update() { vbr::app::App::update(); }

void App::event(SDL_Event *event) { vbr::app::App::event(event); }

void App::render() {
    if (begin()) {
        bindPipeline(*m_pipeline);
        bindVertex(*m_vbuffer);
        bindIndex(*m_ibuffer);
        setViewport();
        setScissor();
        drawIndex(6);
        end();
    }
}

void App::quit() {
    m_ibuffer.reset();
    m_vbuffer.reset();
    m_pipeline.reset();
}
