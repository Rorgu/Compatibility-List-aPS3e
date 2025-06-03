#pragma once

#include "../VulkanAPI.h"
#include "device.h"
#include "memory.h"

namespace vk
{
	struct buffer_view
	{
		VkBufferView value;
		VkBufferViewCreateInfo info = {};
        VkBuffer full_buffer;

		buffer_view(VkDevice dev, VkBuffer buffer, VkFormat format, VkDeviceSize offset, VkDeviceSize size);
		~buffer_view();

		buffer_view(const buffer_view&) = delete;
		buffer_view(buffer_view&&)      = delete;

		bool in_range(u32 address, u32 size, u32& offset) const;

	private:
		VkDevice m_device;
	};
struct buffer;

	struct sub_buffer
	{

		VkBuffer full_buffer;

        struct info_t {
            u32 offset;
            u32 size;
        } info;

		VkBuffer value;

		sub_buffer(const buffer& buf, VkFormat format, VkDeviceSize offset, VkDeviceSize size);
		~sub_buffer();

		sub_buffer(const sub_buffer&) = delete;
		sub_buffer(sub_buffer&&) = delete;

		bool in_range(u32 address, u32 size, u32& offset) const;

    private:
        VkDevice m_device;
	};

    struct buffer_upload{

        ~buffer_upload();
        buffer_upload()=default;
        buffer_upload(const buffer_upload&)=delete;
        buffer_upload(buffer_upload&&)=delete;

        bool in_range(u32 address, u32 size, u32& offset) const;

        bool is(VkBuffer buffer) const;
        using buffer_info=std::variant<VkBufferView,VkBuffer>;
        buffer_info get_buffer() const;

        static std::unique_ptr<buffer_upload> create(const render_device& dev,const buffer& buf, VkFormat format, VkDeviceSize offset, VkDeviceSize size);

    private:
        static std::unique_ptr<buffer_upload> create_with_buffer_view(VkDevice dev, VkBuffer buffer, VkFormat format, VkDeviceSize offset, VkDeviceSize size);
        static std::unique_ptr<buffer_upload> create_with_sub_buffer(const buffer& buf, VkFormat format, VkDeviceSize offset, VkDeviceSize size);

        private:
        bool  is_buffer_view;
        union {
            buffer_view* view;
            sub_buffer* sub;
        } ptr;
    };

	struct buffer
	{
		VkBuffer value;
		VkBufferCreateInfo info = {};
		std::unique_ptr<vk::memory_block> memory;

		buffer(const vk::render_device& dev, u64 size, const memory_type_info& memory_type, u32 access_flags, VkBufferUsageFlags usage, VkBufferCreateFlags flags, vmm_allocation_pool allocation_pool);
		buffer(const vk::render_device& dev, VkBufferUsageFlags usage, void* host_pointer, u64 size);
		~buffer();

		void* map(u64 offset, u64 size);
		void unmap();
		u32 size() const;

		buffer(const buffer&) = delete;
		buffer(buffer&&) = delete;

	//private:
		VkDevice m_device;
	};
}
