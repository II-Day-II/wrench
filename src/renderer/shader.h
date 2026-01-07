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

	struct Shader
	{
		std::shared_ptr<VulkanCtx> m_ctx;
		Slang::ComPtr<slang::ISession> slang_session;
		Slang::ComPtr<slang::IModule> slang_module;

		ShaderKernel compile(const char* entry_point);
	};
	Shader load_shader(std::shared_ptr<VulkanCtx>& compiler_ctx, const char* name, const char* filename);

}; // namespace Wrench

#endif // !SHADER_H
