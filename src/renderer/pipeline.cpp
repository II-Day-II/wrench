#include "pipeline.h"
#include "vk_init.h"
#include "SDL3/SDL.h"



namespace Wrench
{
	void GraphicsPipelineBuilder::clear()
	{
		m_input_assembly = { .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		m_rasterizer = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		m_color_blend_attachment = {};
		m_multisampling = { .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
		m_pipeline_layout = {};
		m_depth_stencil = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
		m_render_info = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
		
		m_shader_stage_infos.clear();
	}

	Pipeline GraphicsPipelineBuilder::build_pipeline(VkDevice device)
	{
		Pipeline ret;

		VkPipelineViewportStateCreateInfo viewport_state = {.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
		viewport_state.viewportCount = 1;
		viewport_state.scissorCount = 1;

		VkPipelineColorBlendStateCreateInfo color_blend_state = {.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
		color_blend_state.logicOpEnable = VK_FALSE;
		color_blend_state.logicOp = VK_LOGIC_OP_COPY;
		color_blend_state.attachmentCount = 1;
		color_blend_state.pAttachments = &m_color_blend_attachment;

		// no VertexInputBindingDescription or VertexInputAttributeDescription here..?
		VkPipelineVertexInputStateCreateInfo vertex_input_ci = { .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
		
		VkDynamicState dyn_state[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dyn_info = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
		dyn_info.pDynamicStates = dyn_state;
		dyn_info.dynamicStateCount = sizeof(dyn_state) / sizeof(VkDynamicState);

		VkGraphicsPipelineCreateInfo pipeline_info = { .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
		pipeline_info.pNext = &m_render_info;
		pipeline_info.stageCount = (uint32_t)m_shader_stage_infos.size();
		pipeline_info.pStages = m_shader_stage_infos.data();
		pipeline_info.pVertexInputState = &vertex_input_ci;
		pipeline_info.pInputAssemblyState = &m_input_assembly;
		pipeline_info.pViewportState = &viewport_state;
		pipeline_info.pRasterizationState = &m_rasterizer;
		pipeline_info.pMultisampleState = &m_multisampling;
		pipeline_info.pColorBlendState = &color_blend_state;
		pipeline_info.pDepthStencilState = &m_depth_stencil;
		pipeline_info.layout = m_pipeline_layout;
		pipeline_info.pDynamicState = &dyn_info;

		ret.layout = m_pipeline_layout;

		if (VkResult result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &ret.pipeline); 
			VK_SUCCESS != result)
		{
			SDL_LogError(0, "Failed to create graphics pipeline! Error: %d", result);
			ret.pipeline = VK_NULL_HANDLE;
		}

		return ret;
	}

	GraphicsPipelineBuilder& GraphicsPipelineBuilder::set_shaders(VkShaderModule vtx, VkShaderModule fgmt)
	{
		m_shader_stage_infos.clear();
		m_shader_stage_infos.push_back(vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_VERTEX_BIT, vtx));
		m_shader_stage_infos.push_back(vkinit::pipeline_shader_stage_create_info(VK_SHADER_STAGE_FRAGMENT_BIT, fgmt));
		return *this;
	}

	GraphicsPipelineBuilder& GraphicsPipelineBuilder::set_input_topology(VkPrimitiveTopology topology)
	{
		m_input_assembly.topology = topology;
		m_input_assembly.primitiveRestartEnable = VK_FALSE; // used for triangle strip/line strip..?
		return *this;
	}

	GraphicsPipelineBuilder& GraphicsPipelineBuilder::set_polygon_mode(VkPolygonMode mode)
	{
		m_rasterizer.polygonMode = mode;
		m_rasterizer.lineWidth = 1.0f;
		return *this;
	}
	
	GraphicsPipelineBuilder& GraphicsPipelineBuilder::set_cull_mode(VkCullModeFlags cull_mode, VkFrontFace front_face)
	{
		m_rasterizer.cullMode = cull_mode;
		m_rasterizer.frontFace = front_face;
		return *this;
	}

	GraphicsPipelineBuilder& GraphicsPipelineBuilder::set_multisampling_none()
	{
		m_multisampling.sampleShadingEnable = VK_FALSE;
		m_multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		m_multisampling.minSampleShading = 1.0f;
		m_multisampling.pSampleMask = nullptr;
		m_multisampling.alphaToCoverageEnable = VK_FALSE;
		m_multisampling.alphaToOneEnable = VK_FALSE;
		return *this;
	}
	GraphicsPipelineBuilder& GraphicsPipelineBuilder::set_blending_none()
	{
		m_color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		m_color_blend_attachment.blendEnable = VK_FALSE;
		return *this;
	}
	GraphicsPipelineBuilder& GraphicsPipelineBuilder::set_blending_additive()
	{
		m_color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		m_color_blend_attachment.blendEnable = VK_TRUE;
		m_color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		m_color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
		m_color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
		m_color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		m_color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		m_color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
		return *this;
	}
	GraphicsPipelineBuilder& GraphicsPipelineBuilder::set_blending_alpha()
	{
		m_color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		m_color_blend_attachment.blendEnable = VK_TRUE;
		m_color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		m_color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; // this is the only difference to additive
		m_color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
		m_color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		m_color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		m_color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
		return *this;
	}
	GraphicsPipelineBuilder& GraphicsPipelineBuilder::set_color_attachment_format(VkFormat format)
	{
		m_color_attachment_format = format;
		m_render_info.colorAttachmentCount = 1;
		m_render_info.pColorAttachmentFormats = &m_color_attachment_format;
		return *this;
	}
	GraphicsPipelineBuilder& GraphicsPipelineBuilder::set_depth_attachment_format(VkFormat format)
	{
		m_render_info.depthAttachmentFormat = format;
		return *this;
	}

	GraphicsPipelineBuilder& GraphicsPipelineBuilder::set_depth_test_off()
	{
		m_depth_stencil.depthTestEnable = VK_FALSE;
		m_depth_stencil.depthWriteEnable = VK_FALSE;
		m_depth_stencil.depthCompareOp = VK_COMPARE_OP_NEVER;
		m_depth_stencil.depthBoundsTestEnable = VK_FALSE;
		m_depth_stencil.stencilTestEnable = VK_FALSE;
		m_depth_stencil.front = {};
		m_depth_stencil.back = {};
		m_depth_stencil.minDepthBounds = 0.0f;
		m_depth_stencil.maxDepthBounds = 1.0f;
		return *this;
	}
	GraphicsPipelineBuilder& GraphicsPipelineBuilder::set_depth_test_on(bool depth_write_enable, VkCompareOp op)
	{
		m_depth_stencil.depthTestEnable = VK_TRUE;
		m_depth_stencil.depthWriteEnable = depth_write_enable;
		m_depth_stencil.depthCompareOp = op;
		m_depth_stencil.depthBoundsTestEnable = VK_FALSE;
		m_depth_stencil.stencilTestEnable = VK_FALSE;
		m_depth_stencil.front = {};
		m_depth_stencil.back = {};
		m_depth_stencil.minDepthBounds = 0.0f;
		m_depth_stencil.maxDepthBounds = 1.0f;
		return *this;
	}
} // namespace Wrench
