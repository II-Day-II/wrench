#ifndef PIPELINE_H
#define PIPELINE_H

#include "vulkan/vulkan.h"
#include <vector>

namespace Wrench
{
	struct Pipeline
	{
		VkPipeline pipeline;
		VkPipelineLayout layout;
	};

	struct GraphicsPipelineBuilder
	{
		std::vector<VkPipelineShaderStageCreateInfo> m_shader_stage_infos;

		VkPipelineInputAssemblyStateCreateInfo m_input_assembly;
		VkPipelineRasterizationStateCreateInfo m_rasterizer;
		VkPipelineColorBlendAttachmentState m_color_blend_attachment;
		VkPipelineMultisampleStateCreateInfo m_multisampling;
		VkPipelineLayout m_pipeline_layout;
		VkPipelineDepthStencilStateCreateInfo m_depth_stencil;
		VkPipelineRenderingCreateInfo m_render_info;
		VkFormat m_color_attachment_format;

		GraphicsPipelineBuilder() { clear(); };
		void clear();

		Pipeline build_pipeline(VkDevice device);

		GraphicsPipelineBuilder& set_shaders(VkShaderModule vertex_shader, VkShaderModule fragment_shader);
		GraphicsPipelineBuilder& set_input_topology(VkPrimitiveTopology topology);
		GraphicsPipelineBuilder& set_polygon_mode(VkPolygonMode mode);
		GraphicsPipelineBuilder& set_cull_mode(VkCullModeFlags cull_mode, VkFrontFace front_face);
		GraphicsPipelineBuilder& set_multisampling_none();
		GraphicsPipelineBuilder& set_blending_none();
		GraphicsPipelineBuilder& set_blending_additive();
		GraphicsPipelineBuilder& set_blending_alpha();
		GraphicsPipelineBuilder& set_color_attachment_format(VkFormat format);
		GraphicsPipelineBuilder& set_depth_attachment_format(VkFormat format);
		GraphicsPipelineBuilder& set_depth_test_off();
		GraphicsPipelineBuilder& set_depth_test_on(bool depth_write_enable, VkCompareOp op);
	};


}; // namespace Wrench

#endif // !PIPELINE_H
