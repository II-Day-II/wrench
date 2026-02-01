#include "shader.h"
#include <array>


namespace Wrench
{
	ShaderCompilerCtx::ShaderCompilerCtx()
	{
		slang::createGlobalSession(m_slang_global_session.writeRef());
		auto slang_targets
		{ 
			std::to_array<slang::TargetDesc>( 
				{ { .format{SLANG_SPIRV}, .profile{ m_slang_global_session->findProfile("spirv_1_4") } } } 
			) 
		};
		auto slang_options
		{ 
			std::to_array<slang::CompilerOptionEntry>(
				{ 
					{ 
						slang::CompilerOptionName::EmitSpirvDirectly, 
						{ slang::CompilerOptionValueKind::Int, 1 } 
					}
				}
			)
		};
		m_session_desc = {
			.targets{slang_targets.data()},
			.targetCount{SlangInt(slang_targets.size())},
			.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR,
			.compilerOptionEntries{slang_options.data()},
			.compilerOptionEntryCount{uint32_t(slang_options.size())}
		};
	}

	ShaderKernel ShaderProgram::compile(const char* entry_point)
	{
		Slang::ComPtr<slang::IEntryPoint> entry_pt;
		slang_module->findEntryPointByName(entry_point, entry_pt.writeRef());
		slang::IComponentType* components[] = { slang_module, entry_pt };
		Slang::ComPtr<slang::IComponentType> program;
		Slang::ComPtr<slang::IBlob> compose_diagnostics;
		slang_module->getSession()->createCompositeComponentType(
			components, 
			sizeof(components) / sizeof(slang::IComponentType*), 
			program.writeRef(), 
			compose_diagnostics.writeRef()
		);

		if (compose_diagnostics)
		{
			SDL_LogWarn(0, "slang::createCompositeComponentType:\n%s", (const char*)compose_diagnostics->getBufferPointer());
		}

		[[maybe_unused]] slang::ProgramLayout* layout = program->getLayout(); // TODO: This only lives as long as `program`, which dies at the end of this scope. That's bad.

		Slang::ComPtr<slang::IComponentType> linked_program;
		Slang::ComPtr<slang::IBlob> linking_diagnostics;
		program->link(linked_program.writeRef(), linking_diagnostics.writeRef());

		if (linking_diagnostics)
		{
			SDL_LogWarn(0, "slang::link:\n%s", (const char*)linking_diagnostics->getBufferPointer());
		}

		Slang::ComPtr<slang::IBlob> kernel_spirv;
		Slang::ComPtr<slang::IBlob> get_spirv_diagnostics;
		linked_program->getEntryPointCode(0, 0, kernel_spirv.writeRef(), get_spirv_diagnostics.writeRef());

		if (get_spirv_diagnostics)
		{
			SDL_LogWarn(0, "slang::getEntryPointCode:\n%s", (const char*)get_spirv_diagnostics->getBufferPointer());
		}


		VkShaderModuleCreateInfo sm_ci = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = kernel_spirv->getBufferSize(),
			.pCode = (uint32_t*)kernel_spirv->getBufferPointer()
		};
		VkShaderModule shader_module;
		VK_CHECK_MACRO(vkCreateShaderModule(m_ctx->device, &sm_ci, nullptr, &shader_module));
		ShaderKernel ret;
		ret.shader_module = shader_module;
		return ret;

	}

	// TODO: Handle case with per-entry point linking
	void ShaderProgram::init()
	{
		auto session = slang_module->getSession();
		std::vector<slang::IComponentType*> components;
		components.push_back(slang_module);
		for (const auto& ep : entry_points)
		{
			components.push_back(ep);
		}
		Slang::ComPtr<slang::IBlob> compose_diagnostics;
		session->createCompositeComponentType(components.data(), components.size(), linked_program.writeRef(), compose_diagnostics.writeRef());
		if (compose_diagnostics)
		{
			SDL_LogWarn(0, "slang::createCompositeComponentType:\n%s", (const char*)compose_diagnostics->getBufferPointer());
		}
	}


	ShaderProgram load_shader(std::shared_ptr<VulkanCtx> &ctx, const char* name, const char* filename, std::vector<const char*> entry_point_names)
	{
		Slang::ComPtr<slang::ISession> slang_session;
		ctx->shader_compiler.m_slang_global_session->createSession(ctx->shader_compiler.m_session_desc, slang_session.writeRef());

		Slang::ComPtr<slang::IBlob> load_diagnostics;
		// TODO: get compiler diagnostics and reflection data
		Slang::ComPtr<slang::IModule> slang_module{ slang_session->loadModuleFromSource(name, filename, nullptr, load_diagnostics.writeRef()) };
		
		if (load_diagnostics)
		{
			SDL_LogWarn(0, "Slang::loadModuleFromSource:\n%s", (const char*)load_diagnostics->getBufferPointer());
		}

		std::vector<Slang::ComPtr<slang::IComponentType>> entry_points;
		for (const auto & ep_name : entry_point_names)
		{
			Slang::ComPtr<slang::IEntryPoint> entry_point;
			if (!SLANG_SUCCEEDED(slang_module->findEntryPointByName(ep_name, entry_point.writeRef())))
			{
				SDL_LogError(0, "Slang::findEntryPointByName:\n>Failed to find entry point %s in module %s at %s", ep_name, name, filename);
				// TODO: abort
				break;
			}
			entry_points.push_back(static_cast<Slang::ComPtr<slang::IComponentType>>(entry_point));
		}

		slang_module->getSession();

		// TODO: move this to a compile_shader(entrypoint) or smth
		//Slang::ComPtr<slang::IBlob> compile_diagnostics;
		//Slang::ComPtr<slang::IBlob> spirv;

		////slang_module->getTargetCode(0, spirv.writeRef(), compile_diagnostics.writeRef());

		//if (compile_diagnostics)
		//{
		//	SDL_LogWarn(0, "slang::getTargetCode:\n%s", (const char*)compile_diagnostics->getBufferPointer());
		//}

		ShaderProgram ret;
		ret.m_ctx = ctx;
		ret.slang_module = slang_module;
		ret.entry_points = entry_points;
		ret.init();
		return ret;
	}

} // namespace Wrench