#include "renderer.h"
#include "VkBootstrap.h"

namespace Wrench {

    bool Renderer::init(std::shared_ptr<VulkanCtx> &vk_ctx) noexcept
    {
        this->ctx = vk_ctx;
        bool swapchain_ok = init_swapchain;
        init_frame_data();
        init_sync_structures();
        return swapchain_ok;
    }

    void Renderer::render([[maybe_unused]] Scene *scene) noexcept
    {
        m_frame_idx += 1;
    }

    void Renderer::cleanup() noexcept
    {
        destroy_swapchain();
    }

    bool Renderer::init_swapchain() noexcept
    {
        // create swapchain
        bool swapchain_created = create_swapchain(m_window_extent.width, m_window_extent.height);
        
        // TODO: break this out into its own function
        // create main render texture / draw image
        VkExtent3D draw_image_extent = { m_window_extent.width, m_window_extent.height, 1 };
        m_draw_image.format = VK_FORMAT_R16G16B16A16_SFLOAT;
        m_draw_image.extent = draw_image_extent;
        VkImageUsageFlags draw_image_usage =
            VK_IMAGE_USAGE_TRANSFER_DST_BIT
            | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
            | VK_IMAGE_USAGE_STORAGE_BIT
            | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        VkImageCreateInfo draw_img_create_info = vkinit::image_create_info(m_draw_image.format, draw_image_usage, m_draw_image.extent);
        
        // allocate using VMA
        // this is reused for the depth texture as well
        VmaAllocationCreateInfo draw_img_allocation_info{};
        draw_img_allocation_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
        draw_img_allocation_info.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        vmaCreateImage(ctx->allocator, &draw_img_create_info, &draw_img_allocation_info, &m_draw_image.image, &m_draw_image.allocation, nullptr);
        // associated image view 
        VkImageViewCreateInfo img_view_cinfo = vkinit::imageview_create_info(m_draw_image.format, m_draw_image.image, VK_IMAGE_ASPECT_COLOR_BIT);

        VkResult draw_img_view_res = vkCreateImageView(ctx->device, &img_view_cinfo, nullptr, &m_draw_image.view);
        bool draw_img_view_ok = draw_img_view_res == VK_SUCCESS;

        // create depth texture / depth image
        m_depth_image.format = VK_FORMAT_D32_SFLOAT;
        m_depth_image.extent = draw_image_extent;
        VkImageUsageFlags depth_usage =
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
            | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        VkImageCreateInfo depth_cinfo = vkinit::image_create_info(m_depth_image.format, depth_usage, m_depth_image.extent);
        vmaCreateImage(ctx->allocator, &depth_cinfo, &draw_img_allocation_info, &m_depth_image.image, &m_depth_image.allocation, nullptr);
        VkImageViewCreateInfo depth_view_cinfo = vkinit::imageview_create_info(m_depth_image.format, m_depth_image.image, VK_IMAGE_ASPECT_DEPTH_BIT);
        VkResult depth_img_view_res = vkCreateImageView(ctx->device, &depth_view_cinfo, nullptr, &m_depth_image.view);
        bool depth_img_view_ok = depth_img_view_res == VK_SUCCESS;

        ctx->deletion_queue.push_function([=]() 
            {
                vkDestroyImageView(ctx->device, m_depth_image.view, nullptr);
                vmaDestroyImage(ctx->allocator, m_depth_image.image, m_depth_image.allocation);

                vkDestroyImageView(ctx->device, m_draw_image.view, nullptr);
                vmaDestroyImage(ctx->allocator, m_draw_image.image, m_draw_image.allocation);
            }
        );

        return draw_img_view_ok && swapchain_created && depth_img_view_ok;
    }

    bool Renderer::create_swapchain(uint32_t width, uint32_t height) noexcept
    {
        // TODO: error handling
        vkb::SwapchainBuilder swapchain_builder{ ctx->physical_device, ctx->device, ctx->surface };
        m_swapchain.format = VK_FORMAT_B8G8R8A8_UNORM;
        auto swapchain_ret = swapchain_builder
            .set_desired_format(VkSurfaceFormatKHR
                {
                    .format = m_swapchain.format,
                    .colorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR
                }
            )
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_RELAXED_KHR)
            .set_desired_extent(width, height)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .build();
        if (!swapchain_ret.has_value()) {
            return false;
        }
        vkb::Swapchain vkb_swapchain = swapchain_ret.value();
        m_swapchain.extent = vkb_swapchain.extent;
        m_swapchain.swapchain = vkb_swapchain.swapchain;
        m_swapchain.images = vkb_swapchain.get_images().value();
        m_swapchain.image_views = vkb_swapchain.get_image_views().value();
        return true;
    }

    void Renderer::destroy_swapchain() noexcept
    {
        // destroy swapchain, including images
        vkDestroySwapchainKHR(ctx->device, m_swapchain.swapchain, nullptr);
        // also destroy swapchain image views
        for (uint32_t i = 0; i < m_swapchain.image_views.size(); i++)
        {
            vkDestroyImageView(ctx->device, m_swapchain.image_views[i], nullptr);
        }
    }

    void Renderer::init_frame_data() noexcept
    {
        // create command pool and main command buffer for each frame in flight
        // allow resetting of individual buffers
        VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        VkCommandPoolCreateInfo cmd_pool_cinfo = vkinit::command_pool_create_info(ctx->gfx_queue_index, flags);
        for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            FrameData& tgt_frame = get_current_frame();
            VK_CHECK_MACRO(vkCreateCommandPool(ctx->device, &cmd_pool_cinfo, nullptr, &tgt_frame.command_pool));
            VkCommandBufferAllocateInfo cmd_alloc_info = vkinit::command_buffer_allocate_info(tgt_frame.command_pool, 1);
            VK_CHECK_MACRO(vkAllocateCommandBuffers(ctx->device, &cmd_alloc_info, &tgt_frame.main_cmd_buf));
            ctx->deletion_queue.push_function([=]() 
                {
                    vkDestroyCommandPool(ctx->device, tgt_frame.command_pool, nullptr);
                }
            );
        }
        // TODO: imgui pool and command buffer here
    }

    void Renderer::init_sync_structures() noexcept
    {

    }

    FrameData& Renderer::get_current_frame() noexcept
    {
        return m_frame_data[m_frame_idx % FRAMES_IN_FLIGHT];
    }

    // TODO: handle correctly
    void Renderer::resize()
    {
        vkDeviceWaitIdle(ctx->device);

        destroy_swapchain();

        int w, h;
        SDL_GetWindowSize(ctx->window, &w, &h);

        m_window_extent.width = w;
        m_window_extent.height = h;
        create_swapchain(m_window_extent.width, m_window_extent.height);
        SDL_LogDebug(0, "Renderer resized to {%d, %d}", w, h);
    }

}; // namespace Wrench