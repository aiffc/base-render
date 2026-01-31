#pragma once

#include "SDL3/SDL.h"
#include "vulkan/vulkan_raii.hpp"
#include <SDL3/SDL_video.h>
#include <memory>

class App {
  protected:
    struct WindowDeleter {
        void operator()(SDL_Window *window) {
            if (window) {
                SDL_DestroyWindow(window);
                window = nullptr;
            }
        }
    };
    std::unique_ptr<SDL_Window, WindowDeleter> m_window;
    bool m_quit = false;

  public:
    App();
    virtual ~App();

    bool shouldQuit() const { return m_quit; }

    [[nodiscard]] virtual bool init();
    virtual void update();
    virtual void event(SDL_Event *event);
    virtual void render();
    virtual void quit();

    App(App &) = delete;
    App(App &&) = delete;
    App &operator=(App &) = delete;
    App &operator=(App &&) = delete;
};
