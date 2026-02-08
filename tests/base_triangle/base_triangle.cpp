#include "base_triangle.hpp"

BaseTriangle::~BaseTriangle() { quit(); }

bool BaseTriangle::init(SDL_InitFlags flag) {
    if (!vbr::app::App::init(flag)) {
        return false;
    }
    m_pipeline = new vbr::gpipeline::Pipeline(**m_vk_device);
    m_pipeline->addShader(VK_SHADER_STAGE_VERTEX_BIT,
                          "../tests/shaders/base_triangle/vert.spv");
    m_pipeline->addShader(VK_SHADER_STAGE_FRAGMENT_BIT,
                          "../tests/shaders/base_triangle/frag.spv");
    m_pipeline->addViewport(static_cast<float>(m_window_size.x),
                            static_cast<float>(m_window_size.y));
    m_pipeline->addScissor(m_window_size.x, m_window_size.y);
    m_pipeline->addColorBlendAttachemt();
    if (!m_pipeline->init()) {
        return false;
    }
    return true;
}

void BaseTriangle::update() { vbr::app::App::update(); }

void BaseTriangle::event(SDL_Event *event) { vbr::app::App::event(event); }

void BaseTriangle::render() {
    if (begin()) {
        bindPipeline(*m_pipeline);
        setViewport();
        setScissor();
        draw();
        end();
    }
}

void BaseTriangle::quit() { delete m_pipeline; }
