#include <chrono>
#include "../util/util.h"
#include "application.h"
#include "SDL3/SDL.h"

namespace Wrench {

    bool Application::init() noexcept
    {
        SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_INVALID, "[INVALID]: ");
        SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_TRACE, "[TRACE]: ");
        SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_VERBOSE, "[VERBOSE]: ");
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

        m_window = SDL_CreateWindow("Wrench Renderer", util::Constants::DEFAULT_WINDOW_WIDTH, util::Constants::DEFAULT_WINDOW_HEIGHT, window_flags);

        m_ctx = std::make_shared<VulkanCtx>(m_window);
        bool vulkan_ok = m_ctx->initialized;
        
        bool window_ok = m_window != nullptr;

        bool renderer_ok = m_renderer.init(m_ctx);

        return sdl_ok && window_ok && vulkan_ok && renderer_ok;
    }

    void Application::run() noexcept
    {
        SDL_Event e;
        bool quit = false;
        auto start = std::chrono::system_clock::now();
        auto elapsed = std::chrono::microseconds(16'000); // assume 16 milliseconds for first frame
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

            m_scene->update(elapsed.count() / 1'000'000.0f); // to seconds
            m_renderer.render(m_scene);

            auto end = std::chrono::system_clock::now();
            elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            stats.frame_time_ms = elapsed.count() / 1000.0f;
            start = end;
        }
    }

    void Application::exit() noexcept
    {
        m_renderer.cleanup();
        m_ctx->cleanup();
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }

}; // namespace Wrench