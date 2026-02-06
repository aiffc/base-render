#pragma once

#include "../src/base/base.hpp"
#include "../src/base/graphics_pipeline.hpp"

class BaseTriangle : public vbr::app::App {
  private:
    vbr::gpipeline::Pipeline *m_pipeline;

  public:
    using vbr::app::App::App;
    ~BaseTriangle() override;

    [[nodiscard]] bool init(SDL_InitFlags flag = SDL_INIT_AUDIO) override;
    void update() override;
    void event(SDL_Event *event) override;
    void render() override;
    void quit() override;
};
