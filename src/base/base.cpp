#include "base.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <memory>

App::App() = default;
App::~App() = default;

bool App::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        // error log
        return false;
    }

    SDL_Window *raw_window =
        SDL_CreateWindow("vbr", 1024, 980, SDL_WINDOW_VULKAN);

    if (raw_window == nullptr) {
        // error log
        return false;
    }

    m_window = std::unique_ptr<SDL_Window, WindowDeleter>(raw_window);
    return true;
}

void App::update() {}

void App::event(SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        m_quit = true;
    } else {
        m_quit = false;
    }
}

void App::render() {}

void App::quit() {
    // vulkan quit
}
