#pragma once

#include "../../inc/base.hpp"
#include "../../inc/buffer.hpp"
#include "../../inc/graphics_pipeline.hpp"
#include "../../inc/layout.hpp"
#include <memory>

struct VertexInfo {
    glm::vec2 pos;
    glm::vec3 color;
};

class App : public vbr::app::App {
  private:
    std::unique_ptr<vbr::buffer::Buffer> m_vbuffer;
    std::unique_ptr<vbr::layout::Layout> m_layout;
    std::unique_ptr<vbr::gpipeline::Pipeline> m_pipeline;

  public:
    using vbr::app::App::App;
    ~App() override;

    [[nodiscard]] bool
    init(SDL_InitFlags flag = SDL_INIT_AUDIO,
         VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT) override;
    void update() override;
    void event(SDL_Event *event) override;
    void render() override;
    void quit() override;
};
