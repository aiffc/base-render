#include "base_triangle.hpp"
#include <memory>

App::~App() { quit(); }

bool App::init(SDL_InitFlags flag, VkSampleCountFlagBits sample_count) {
    if (!vbr::app::App::init(flag, sample_count)) {
        return false;
    }

    m_layout = std::make_unique<vbr::layout::Layout>(**m_vk_device);
    m_layout->init();

    m_pipeline = std::make_unique<vbr::gpipeline::Pipeline>(*m_vk_device);
    m_pipeline->addShader(VK_SHADER_STAGE_VERTEX_BIT,
                          "../tests/shaders/base_triangle/vert.spv");
    m_pipeline->addShader(VK_SHADER_STAGE_FRAGMENT_BIT,
                          "../tests/shaders/base_triangle/frag.spv");
    m_pipeline->addViewport(static_cast<float>(m_window_size.x),
                            static_cast<float>(m_window_size.y));
    m_pipeline->addScissor(m_window_size.x, m_window_size.y);
    m_pipeline->addColorBlendAttachemt();
    if (!m_pipeline->init(**m_layout)) {
        return false;
    }
    return true;
}

void App::update() { vbr::app::App::update(); }

void App::event(SDL_Event *event) { vbr::app::App::event(event); }

void App::render() {
    if (begin()) {
        bindPipeline(*m_pipeline);
        setViewport();
        setScissor();
        draw(3);
        end();
    }
}

void App::quit() {
    m_layout.reset();
    m_pipeline.reset();
}
