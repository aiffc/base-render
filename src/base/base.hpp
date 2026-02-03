#pragma once

#include "SDL3/SDL.h"
#include "glm/glm.hpp"
#include "vulkan/vulkan.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <memory>

class App {
  protected:
    bool m_debug = true;
    glm::ivec2 m_window_size;
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

    // vulkan things
    VkInstance m_vk_instance;
    VkDebugUtilsMessengerEXT m_vk_dbg_messager;

  private:
    // internal function
    bool initInstance();

  public:
    App(const glm::ivec2 &window_size = {1024, 980});
    virtual ~App();

    bool shouldQuit() const { return m_quit; }

    [[nodiscard]] virtual bool init(SDL_InitFlags flag = SDL_INIT_AUDIO);
    virtual void update();
    virtual void event(SDL_Event *event);
    virtual void render();
    virtual void quit();

    App(App &) = delete;
    App(App &&) = delete;
    App &operator=(App &) = delete;
    App &operator=(App &&) = delete;
};
