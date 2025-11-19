#include "application.h"
#include "SDL3/SDL.h"
#include <chrono>

namespace Wrench {

    bool Application::init() noexcept
    {
        SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_INVALID, "[INVALID]: ");
        SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_TRACE, "[TRACE]: ");
        SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_DEBUG, "[DEBUG]: ");
        SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_INFO, "[INFO]: ");
        SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_WARN, "[WARNING]: ");
        SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_ERROR, "[ERROR]: ");


#ifdef _DEBUG
        SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);
#else
        SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
#endif


        bool sdl_ok = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

        SDL_WindowFlags window_flags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE;

        m_window = SDL_CreateWindow("Wrench Renderer", 1700, 900, window_flags);

        bool vulkan_ok = init_vulkan(ctx, m_window);

        bool window_ok = m_window != nullptr;

        bool renderer_ok = m_renderer->init(m_window);

        return sdl_ok && window_ok && vulkan_ok && renderer_ok;
    }

    void Application::run() noexcept
    {
        SDL_Event e;
        bool quit = false;
        auto start = std::chrono::system_clock::now();
        auto elapsed = std::chrono::microseconds(16'000);
        while (!quit)
        {
            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_EVENT_QUIT)
                {
                    quit = true;
                }
                // TODO: handle SDL events, both locally and for imgui
            }

            m_scene->update(elapsed.count() / 1'000'000.0f);
            m_renderer->render(m_scene);

            auto end = std::chrono::system_clock::now();
            elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            stats.frame_time_ms = elapsed.count() / 1000.0f;
            start = end;
        }
    }

    void Application::exit() noexcept
    {
        ctx.cleanup();
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }

}; // namespace Wrench