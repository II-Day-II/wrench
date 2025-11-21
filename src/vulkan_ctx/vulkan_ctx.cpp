#include "vulkan_ctx.h"
#include "SDL3/SDL_Vulkan.h"
#include "VkBootstrap.h"


namespace Wrench {

	
	VulkanCtx::VulkanCtx(SDL_Window* window) noexcept
	{
		// get VkInstance
		vkb::InstanceBuilder inst_builder;
		auto inst_ret = inst_builder.set_app_name("Wrench Renderer")
			.request_validation_layers()
			.use_default_debug_messenger()
			.build();
		if (!inst_ret) 
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load Vulkan Instance!");
			return;
		}
		vkb::Instance vkb_instance = inst_ret.value();
		debug_messenger = vkb_instance.debug_messenger;

		// get VkSurface
		SDL_Vulkan_CreateSurface(window, vkb_instance.instance, nullptr, &surface);

		// Declared required features
		VkPhysicalDeviceVulkan13Features features_1_3{};
		features_1_3.dynamicRendering = true;
		features_1_3.synchronization2 = true;

		VkPhysicalDeviceVulkan12Features features_1_2{};
		features_1_2.bufferDeviceAddress = true;
		features_1_2.descriptorIndexing = true;

		// get VkPhysicalDevice
		vkb::PhysicalDeviceSelector selector{ vkb_instance };
		auto phys_ret = selector.set_minimum_version(1, 3) // 1.4 not available on my laptop ;_;
			.set_required_features_12(features_1_2)
			.set_required_features_13(features_1_3)
			.set_surface(surface)  
			.select();
		if (!phys_ret) 
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to select Vulkan Physical Device!");
			return;
		}

		// get VkDevice
		vkb::DeviceBuilder device_builder{ phys_ret.value() };
		auto dev_ret = device_builder.build();
		if (!dev_ret) 
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get Vulkan Device!");
			return;
		}
		vkb::Device vkb_device = dev_ret.value();

		// get graphics queue and its index
		auto gfx_queue_ret = vkb_device.get_queue(vkb::QueueType::graphics);
		if (!gfx_queue_ret) 
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to get Vulkan Graphics Queue!");
			return;
		}
		gfx_queue_index = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

		// store context
		gfx_queue = gfx_queue_ret.value();
		device = vkb_device.device;
		instance = vkb_instance.instance;
		physical_device = phys_ret.value();

		// create VMA allocator
		VmaAllocatorCreateInfo allocator_info{};
		allocator_info.physicalDevice = physical_device;
		allocator_info.device = device;
		allocator_info.instance = instance;
		allocator_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		VkResult vma_ok = vmaCreateAllocator(&allocator_info, &allocator);
		if (vma_ok != VK_SUCCESS)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create VMA Allocator!");
			return;
		}

		// remember reverse creation order
		deletion_queue.push_function([&]() 
			{
				vmaDestroyAllocator(allocator);
				vkDestroyDevice(device, nullptr);
				SDL_Vulkan_DestroySurface(instance, surface, nullptr);
				vkb::destroy_debug_utils_messenger(instance, debug_messenger, nullptr);
				vkDestroyInstance(instance, nullptr);
			}
		);

		this->window = window;
		initialized = true;
	}

	void VulkanCtx::cleanup() noexcept
	{
		// make sure device is done working before destroying it
		vkDeviceWaitIdle(device);

		// flush remaining resources in the queue (recorded in or after init_vulkan)
		deletion_queue.flush();
	}

}; // namespace Wrench
