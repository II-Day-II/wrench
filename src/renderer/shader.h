#ifndef SHADER_H
#define SHADER_H

#include "../vulkan_ctx/vulkan_ctx.h"
#include "vulkan/vulkan.h"
#include "slang/slang-com-ptr.h"
#include "slang/slang.h"
#include <memory>

namespace Wrench
{

	struct ShaderKernel
	{
		// TODO: I don't want to keep this around here, the VkShaderModule should be destroyed once the pipeline is built.
		VkShaderModule shader_module;

	};

	struct ShaderProgram
	{
		std::shared_ptr<VulkanCtx> m_ctx;
		Slang::ComPtr<slang::IModule> slang_module; // also holds a reference to the slang::ISession that created it
		std::vector<Slang::ComPtr<slang::IComponentType>> entry_points;
		Slang::ComPtr<slang::IComponentType> linked_program;

		ShaderKernel compile(const char* entry_point);
		void init();
	};
	ShaderProgram load_shader(std::shared_ptr<VulkanCtx>& compiler_ctx, const char* name, const char* filename, std::vector<const char*> entry_point_names);

}; // namespace Wrench

#endif // !SHADER_H
