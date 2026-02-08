#pragma once

#include "../../inc/base.hpp"
#include "../../inc/buffer.hpp"
#include "../../inc/graphics_pipeline.hpp"
#include <memory>

struct VertexInfo {
    glm::vec2 pos;
    glm::vec3 color;
};

class BufferTriangle : public vbr::app::App {
  private:
    std::unique_ptr<vbr::buffer::Buffer> m_vbuffer;
    vbr::gpipeline::Pipeline *m_pipeline;

  public:
    using vbr::app::App::App;
    ~BufferTriangle() override;

    [[nodiscard]] bool init(SDL_InitFlags flag = SDL_INIT_AUDIO) override;
    void update() override;
    void event(SDL_Event *event) override;
    void render() override;
    void quit() override;
};
