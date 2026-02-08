#include "buffer_triangle.hpp"
#include <cstddef>

BufferTriangle::~BufferTriangle() { quit(); }

bool BufferTriangle::init(SDL_InitFlags flag) {
    if (!vbr::app::App::init(flag)) {
        return false;
    }
    m_pipeline = new vbr::gpipeline::Pipeline(**m_vk_device);
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
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
    m_vbuffer = m_vk_device->createUsageBuffer<VertexInfo>(
        vertices, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    return true;
}

void BufferTriangle::update() { vbr::app::App::update(); }

void BufferTriangle::event(SDL_Event *event) { vbr::app::App::event(event); }

void BufferTriangle::render() {
    if (begin()) {
        bindPipeline(*m_pipeline);
        bindVertex(*m_vbuffer);
        setViewport();
        setScissor();
        draw();
        end();
    }
}

void BufferTriangle::quit() {
    m_vbuffer.reset();
    delete m_pipeline;
}
