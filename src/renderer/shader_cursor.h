#ifndef SHADER_CURSOR_H
#define SHADER_CURSOR_H

#include "vulkan/vulkan.h"
#include "slang/slang.h"

#include "../vulkan_ctx/vulkan_ctx.h"
#include "texture.h"
#include "sampler.h"

namespace Wrench
{
    VkDescriptorType map_binding_type_to_vulkan(slang::BindingType slang_type);

    struct ShaderOffset
    {
        size_t byte_offset = 0;
        uint32_t binding_range_index = 0;
        uint32_t array_index_in_binding_range = 0;
    };

    class ShaderObject
    {
    public:
        slang::TypeLayoutReflection* m_type_layout;
        virtual void write(ShaderOffset offset, Texture* texture) = 0;
        virtual void write(ShaderOffset offset, Sampler* sampler) = 0;
        virtual void write(ShaderOffset offset, const void* data, size_t size) = 0;
    };

    class VulkanShaderObject : public ShaderObject
    {
    public: 
        void write(ShaderOffset offset, Texture* texture) override;
        void write(ShaderOffset offset, Sampler* sampler) override;
        void write(ShaderOffset offset, const void* data, size_t size) override;

    private:
        VkBuffer m_buffer;
        uint8_t* m_buffer_data;
        VkDescriptorSet m_descriptor_set;

        std::shared_ptr<VulkanCtx> m_ctx;
    };

    struct ShaderCursor
    {
    public:

        ShaderCursor(ShaderObject* object);

        // find pointer into struct field by name
        ShaderCursor field(const char* name);
        // find pointer into struct field by index
        ShaderCursor field(uint32_t index);
        // find array element by index
        ShaderCursor element(uint32_t index);

        // write a value into a shader parameter pointed to by this cursor
        void write(Texture* texture); // TODO: type Texture
        void write(Sampler* sampler); // TODO: type Sampler
        // write arbitrary data into the place pointed to by this cursor
        void write(const void* data, size_t size);

        template<typename T>
        void write(const T* data) { write(data, sizeof(T)); }
        template<typename T>
        void write(T data) { write(&data, sizeof(T)); }

    private:
        ShaderObject* m_object = nullptr;
        ShaderOffset m_offset;

        slang::TypeLayoutReflection* m_type_layout;

        
    };

    struct ShaderObjectLayoutBuilder
    {
        std::vector<VkDescriptorSetLayoutBinding> m_bindings;
        uint32_t m_binding_index = 0;
        
        void add_bindings_for_parameter_block(slang::TypeLayoutReflection* type_layout);
        void add_bindings_from(slang::TypeLayoutReflection* type_layout, uint32_t element_count);
    };

} // namespace Wrench

#endif // !SHADER_CURSOR_H
