#ifndef WRENCH_UTIL_H
#define WRENCH_UTIL_H
#include <stack>
#include <functional>
namespace Wrench 
{
namespace util 
{
    namespace Constants {
        constexpr uint32_t const DEFAULT_WINDOW_WIDTH = 1700;
        constexpr uint32_t const DEFAULT_WINDOW_HEIGHT = 900;
    };

    struct DeletionQueue 
    {
        std::stack<std::function<void()>> deletors;
        void flush() noexcept;
        void push_function(std::function<void()> &&func) noexcept;
    };
}; // namespace util
}; // namespace Wrench
#endif // !WRENCH_UTIL_H
