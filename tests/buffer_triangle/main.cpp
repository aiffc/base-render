
#include "buffer_triangle.hpp"
#include <SDL3/SDL_init.h>
#include <algorithm>
#include <memory>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

std::unique_ptr<BufferTriangle> app;

SDL_AppResult SDL_AppInit(void **appstate [[maybe_unused]],
                          int argc [[maybe_unused]],
                          char **argv [[maybe_unused]]) {
    app = std::make_unique<BufferTriangle>();
    if (!app->init()) {
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate [[maybe_unused]], SDL_Event *event) {
    app->event(event);
    if (app->shouldQuit()) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate [[maybe_unused]]) {
    app->update();
    app->render();
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate [[maybe_unused]],
                 SDL_AppResult result [[maybe_unused]]) {
    // app->quit();
    app.reset();
}
