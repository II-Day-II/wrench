#ifndef RENDERER_H
#define RENDERER_H
#include <memory>
#include <vector>
#include "../scene/scene.h"
#include "../vulkan_ctx/vulkan_ctx.h"
#include "../util/util.h"
#include "vk_init.h"
#include "SDL3/SDL.h"

namespace Wrench {

    struct Swapchain
    {
        VkExtent2D extent;
        VkFormat format;
        VkSwapchainKHR swapchain;
        std::vector<VkImage> images;
        std::vector<VkImageView> image_views;
    };

    struct FrameData
    {
        VkCommandPool command_pool;
        VkCommandBuffer main_cmd_buf;
        VkSemaphore swapchain_semaphore, render_semaphore;
        VkFence render_fence;
        util::DeletionQueue deletion_queue;
        // DescriptorAllocator frame_descriptors;
    };

    constexpr uint32_t const FRAMES_IN_FLIGHT = 2;

    class Renderer
    {
        std::shared_ptr<VulkanCtx> ctx;
        Swapchain m_swapchain;
    public:
        bool init(std::shared_ptr<VulkanCtx> &vk_ctx) noexcept;
        void render(Scene *scene) noexcept;
        void resize();
        void cleanup() noexcept;
    private:
        bool init_swapchain() noexcept;
        bool create_swapchain(uint32_t width, uint32_t height) noexcept;
        void destroy_swapchain() noexcept;
        void init_frame_data() noexcept;
        void init_imgui() noexcept;
        FrameData& get_current_frame() noexcept;

        VkExtent2D m_window_extent{util::Constants::DEFAULT_WINDOW_WIDTH, util::Constants::DEFAULT_WINDOW_HEIGHT};
        AllocatedImage m_draw_image;
        AllocatedImage m_depth_image;
        FrameData m_frame_data[FRAMES_IN_FLIGHT];
        uint32_t m_frame_idx = 0;

    };

}; // namespace Wrench
#endif // !RENDERER_H
