#include "shader_cursor.h"
#include <memory>
#include "vk_init.h"

namespace Wrench
{
	VkDescriptorType map_binding_type_to_vulkan(slang::BindingType slang_type)
	{
		
		switch (slang_type)
		{
		case slang::BindingType::Texture:
			return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			break;
		case slang::BindingType::MutableTexture:
			return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		case slang::BindingType::Sampler:
			return VK_DESCRIPTOR_TYPE_SAMPLER;
			break;
		default:
			SDL_LogError(0, "Mapping Slang BindingType to Vulkan VkDescriptorType failed!!!\nSlang type is: %d, which is not yet implemented", slang_type);
			return VK_DESCRIPTOR_TYPE_MAX_ENUM;
		}
	}

	ShaderCursor::ShaderCursor(ShaderObject* object) : m_object(object), m_type_layout(object->m_type_layout)
	{

	}

	void ShaderCursor::write(const void* data, size_t size)
	{
		m_object->write(m_offset, data, size);

	}

	void ShaderCursor::write(Texture* texture)
	{
		m_object->write(m_offset, texture);

	}

	void ShaderCursor::write(Sampler* sampler)
	{
		m_object->write(m_offset, sampler);
	}


	ShaderCursor ShaderCursor::field(const char* name)
	{
		return field((uint32_t)m_type_layout->findFieldIndexByName(name));
	}

	ShaderCursor ShaderCursor::field(uint32_t index)
	{
		slang::VariableLayoutReflection* field = m_type_layout->getFieldByIndex(index);

		ShaderCursor result = *this;
		result.m_type_layout = field->getTypeLayout();
		result.m_offset.byte_offset += field->getOffset();
		result.m_offset.binding_range_index += (uint32_t)m_type_layout->getFieldBindingRangeOffset(index);

		return result;
	}

	ShaderCursor ShaderCursor::element(uint32_t index)
	{
		slang::TypeLayoutReflection* element_type_layout = m_type_layout->getElementTypeLayout();

		ShaderCursor result = *this;
		result.m_type_layout = element_type_layout;
		result.m_offset.byte_offset += index * element_type_layout->getStride();

		result.m_offset.array_index_in_binding_range *= (uint32_t)m_type_layout->getElementCount();
		result.m_offset.array_index_in_binding_range += index;

		return result;
	}


	void VulkanShaderObject::write(ShaderOffset offset, Texture* texture)
	{
		uint32_t binding_index = (uint32_t)m_type_layout->getBindingRangeFirstDescriptorRangeIndex(offset.binding_range_index);

		VkDescriptorImageInfo image_info{};
		image_info.imageView = texture->view;
		image_info.imageLayout = texture->layout;

		VkDescriptorType type = map_binding_type_to_vulkan(m_type_layout->getBindingRangeType(offset.binding_range_index));
		VkWriteDescriptorSet write = vkinit::write_descriptor_image(type, m_descriptor_set, &image_info, binding_index);
		write.dstArrayElement = offset.array_index_in_binding_range;

		vkUpdateDescriptorSets(m_ctx->device, 1, &write, 0, nullptr);
	}

	void VulkanShaderObject::write(ShaderOffset offset, Sampler* sampler)
	{
		uint32_t binding_index = (uint32_t)m_type_layout->getBindingRangeFirstDescriptorRangeIndex(offset.binding_range_index);

		VkDescriptorImageInfo sampler_info{};
		sampler_info.sampler = sampler->sampler;

		VkDescriptorType type = map_binding_type_to_vulkan(m_type_layout->getBindingRangeType(offset.binding_range_index));
		VkWriteDescriptorSet write = vkinit::write_descriptor_image(type, m_descriptor_set, &sampler_info, binding_index);
		write.dstArrayElement = offset.array_index_in_binding_range;

		vkUpdateDescriptorSets(m_ctx->device, 1, &write, 0, nullptr);
	}

	void VulkanShaderObject::write(ShaderOffset offset, const void* data, size_t size)
	{
		// TODO: Handle case where m_buffer_data isn't already mapped to m_buffer on the GPU (also make sure m_buffer is mapped)
		std::memcpy(m_buffer_data + offset.byte_offset, data, size);
		//vkCmdUpdateBuffer(m_command_buffer, m_buffer, m_byte_offset, size, data);
	}

	// entry point into the recursive reflection gettening...stuff...
	void ShaderObjectLayoutBuilder::add_bindings_for_parameter_block(slang::TypeLayoutReflection* type_layout)
	{
		if (auto size = type_layout->getSize())
		{
			VkDescriptorSetLayoutBinding layout_binding;
			layout_binding.binding = m_binding_index++;
			layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			layout_binding.descriptorCount = 1;
			//...stage flags, immutable samplers...
			m_bindings.push_back(layout_binding);
		}
		add_bindings_from(type_layout, 1);
	}
	void ShaderObjectLayoutBuilder::add_bindings_from(slang::TypeLayoutReflection* type_layout, uint32_t descriptor_count)
	{

		uint32_t binding_range_count = (uint32_t)type_layout->getBindingRangeCount();
		for (uint32_t i = 0; i < binding_range_count; i++)
		{
			VkDescriptorSetLayoutBinding layout_binding{};
			layout_binding.binding = m_binding_index++;
			layout_binding.descriptorType = map_binding_type_to_vulkan(type_layout->getBindingRangeType(i));
			layout_binding.descriptorCount = descriptor_count * (uint32_t)type_layout->getBindingRangeBindingCount(i);
			//... stage flags, immutable samplers...
			m_bindings.push_back(layout_binding);
		}
	}
} // namespace Wrench