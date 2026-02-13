#pragma once

#include "SDL3/SDL.h"
#include "device.hpp"
#include "glm/glm.hpp"
#include "swapchain.hpp"
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_core.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <memory>

namespace vbr::gpipeline {
class Pipeline;
}

namespace vbr::app {

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
    VkSurfaceKHR m_vk_surface;
    std::unique_ptr<vbr::device::Device> m_vk_device;
    std::unique_ptr<vbr::swapchain::Swapchain> m_vk_swapchain;

  protected:
    bool begin(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f);
    bool end();
    void setViewport(float w = 0.0f, float h = 0.0f, float x = 0.0f,
                     float y = 0.0f, float min = 0.0f, float max = 1.0f);
    void setScissor(uint32_t w = 0, uint32_t h = 0, int32_t x = 0,
                    int32_t y = 0);
    void bindPipeline(vbr::gpipeline::Pipeline &pipeline);
    void bindVertex(vbr::buffer::Buffer &buffer);
    void draw(uint32_t count);
    void bindIndex(vbr::buffer::Buffer &buffer);
    void drawIndex(uint32_t count);
    void bindDescriptorSet(const VkDescriptorSet &set,
                           const VkPipelineLayout &layout);

  private:
    // internal function for sdl
    void updateWindowSize();
    // internal function for vulkan init
    [[nodiscard]] bool initInstance();
    [[nodiscard]] bool initSurface();

  public:
    App(const glm::ivec2 &window_size = {1024, 980});
    virtual ~App();

    bool shouldQuit() const { return m_quit; }

    [[nodiscard]] virtual bool
    init(SDL_InitFlags flag = SDL_INIT_AUDIO,
         VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT);
    virtual void update();
    virtual void event(SDL_Event *event);
    virtual void render();
    virtual void quit();

    App(App &) = delete;
    App(App &&) = delete;
    App &operator=(App &) = delete;
    App &operator=(App &&) = delete;
};

} // namespace vbr::app
