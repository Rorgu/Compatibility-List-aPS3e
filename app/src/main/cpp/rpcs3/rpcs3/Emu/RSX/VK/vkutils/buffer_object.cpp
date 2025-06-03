#include "buffer_object.h"
#include "device.h"
#include "shared.h"

namespace vk
{
	buffer_view::buffer_view(VkDevice dev, VkBuffer buffer, VkFormat format, VkDeviceSize offset, VkDeviceSize size)
		: full_buffer(buffer),m_device(dev)
	{
		info.buffer = buffer;
		info.format = format;
		info.offset = offset;
		info.range  = size;
		info.sType  = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
		CHECK_RESULT(_vkCreateBufferView(m_device, &info, nullptr, &value));
	}

	buffer_view::~buffer_view()
	{
		_vkDestroyBufferView(m_device, value, nullptr);
	}

	bool buffer_view::in_range(u32 address, u32 size, u32& offset) const
	{
		if (address < info.offset)
			return false;

		const u32 _offset = address - static_cast<u32>(info.offset);
		if (info.range < _offset)
			return false;

		const auto remaining = info.range - _offset;
		if (size <= remaining)
		{
			offset = _offset;
			return true;
		}

		return false;
	}

	sub_buffer::sub_buffer(const buffer& buf, VkFormat format, VkDeviceSize offset, VkDeviceSize size)
		:  full_buffer(buf.value), info({.offset=static_cast<u32>(offset), .size=static_cast<u32>(size)}),m_device(buf.m_device)
	{
        VkBufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.flags = buf.info.flags;
		info.size = size;
		info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		CHECK_RESULT(_vkCreateBuffer(buf.m_device, &info, nullptr, &value));
		_vkBindBufferMemory(buf.m_device, value, buf.memory->get_vk_device_memory(), buf.memory->get_vk_device_memory_offset() + offset);
	}

    sub_buffer::~sub_buffer()
	{
		_vkDestroyBuffer(m_device, value, nullptr);
	}


	bool sub_buffer::in_range(u32 address, u32 size, u32& offset) const
	{
		if (address < info.offset)
			return false;

		const u32 _offset = address - static_cast<u32>(info.offset);
		if (info.size < _offset)
			return false;

		const auto remaining = info.size - _offset;
		if (size <= remaining)
		{
			offset = _offset;
			return true;
		}

		return false;
	}


    std::unique_ptr<buffer_upload> buffer_upload::create(const render_device& dev,const buffer& buf, VkFormat format, VkDeviceSize offset, VkDeviceSize size){

        if(cfg_vertex_buffer_upload_mode_use_buffer_view())
                return create_with_buffer_view(dev,buf.value,format,offset,size);
            else
                return create_with_sub_buffer(buf,format,offset,size);

    }


    bool buffer_upload::in_range(u32 address, u32 size, u32& offset) const{
        if (is_buffer_view)
            return ptr.view->in_range(address, size, offset);
        else
            return ptr.sub->in_range(address, size, offset);
    }

    bool buffer_upload::is(VkBuffer buffer) const{
        if (is_buffer_view)
            return ptr.view->full_buffer == buffer;
        else
            return ptr.sub->full_buffer == buffer;
    }


    buffer_upload::buffer_info buffer_upload::get_buffer() const{
        if (is_buffer_view)
            return ptr.view->value;
        else
            return ptr.sub->value;
    }

    std::unique_ptr<buffer_upload> buffer_upload::create_with_buffer_view(VkDevice dev, VkBuffer buffer, VkFormat format, VkDeviceSize offset, VkDeviceSize size)
	{
        std::unique_ptr<buffer_upload> result=std::make_unique<buffer_upload>();
        result->is_buffer_view = true;
        result->ptr.view=new buffer_view(dev, buffer, format, offset, size);
        return result;
	}

    std::unique_ptr<buffer_upload> buffer_upload::create_with_sub_buffer(const buffer& buf, VkFormat format, VkDeviceSize offset, VkDeviceSize size)
	{
        std::unique_ptr<buffer_upload> result=std::make_unique<buffer_upload>();
        result->is_buffer_view = false;
        result->ptr.sub=new sub_buffer(buf, format, offset, size);
        return result;
	}

    buffer_upload::~buffer_upload()
	{
		if (is_buffer_view)
		{
            if(ptr.view)
			delete ptr.view;
		}
		else
		{
            if(ptr.sub)
			delete ptr.sub;
		}
	}


	buffer::buffer(const vk::render_device& dev, u64 size, const memory_type_info& memory_type, u32 access_flags, VkBufferUsageFlags usage, VkBufferCreateFlags flags, vmm_allocation_pool allocation_pool)
		: m_device(dev)
	{
		info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.flags = flags;
		info.size = size;
		info.usage = usage;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		CHECK_RESULT(_vkCreateBuffer(m_device, &info, nullptr, &value));

		// Allocate vram for this buffer
		VkMemoryRequirements memory_reqs;
		_vkGetBufferMemoryRequirements(m_device, value, &memory_reqs);

		memory_type_info allocation_type_info = memory_type.get(dev, access_flags, memory_reqs.memoryTypeBits);
		if (!allocation_type_info)
		{
			fmt::throw_exception("No compatible memory type was found!");
		}

		memory = std::make_unique<memory_block>(m_device, memory_reqs.size, memory_reqs.alignment, allocation_type_info, allocation_pool);
		_vkBindBufferMemory(dev, value, memory->get_vk_device_memory(), memory->get_vk_device_memory_offset());
	}

	buffer::buffer(const vk::render_device& dev, VkBufferUsageFlags usage, void* host_pointer, u64 size)
		: m_device(dev)
	{
		info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.flags = 0;
		info.size = size;
		info.usage = usage;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkExternalMemoryBufferCreateInfoKHR ex_info;
		ex_info.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_BUFFER_CREATE_INFO_KHR;
		ex_info.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_HOST_ALLOCATION_BIT_EXT;
		ex_info.pNext = nullptr;

		info.pNext = &ex_info;
		CHECK_RESULT(_vkCreateBuffer(m_device, &info, nullptr, &value));

		auto& memory_map = dev.get_memory_mapping();
		ensure(_vkGetMemoryHostPointerPropertiesEXT);

		VkMemoryHostPointerPropertiesEXT memory_properties{};
		memory_properties.sType = VK_STRUCTURE_TYPE_MEMORY_HOST_POINTER_PROPERTIES_EXT;
		CHECK_RESULT(_vkGetMemoryHostPointerPropertiesEXT(dev, VK_EXTERNAL_MEMORY_HANDLE_TYPE_HOST_ALLOCATION_BIT_EXT, host_pointer, &memory_properties));

		VkMemoryRequirements memory_reqs;
		_vkGetBufferMemoryRequirements(m_device, value, &memory_reqs);

		auto required_memory_type_bits = memory_reqs.memoryTypeBits & memory_properties.memoryTypeBits;
		if (!required_memory_type_bits)
		{
			// AMD driver bug. Buffers created with external memory extension return type bits of 0
			rsx_log.warning("Could not match buffer requirements and host pointer properties.");
			required_memory_type_bits = memory_properties.memoryTypeBits;
		}

		const auto allocation_type_info = memory_map.host_visible_coherent.get(dev,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			required_memory_type_bits);

		if (!allocation_type_info)
		{
			fmt::throw_exception("No compatible memory type was found!");
		}

		memory = std::make_unique<memory_block_host>(m_device, host_pointer, size, allocation_type_info);
		CHECK_RESULT(_vkBindBufferMemory(dev, value, memory->get_vk_device_memory(), memory->get_vk_device_memory_offset()));
	}

	buffer::~buffer()
	{
		_vkDestroyBuffer(m_device, value, nullptr);
	}

	void* buffer::map(u64 offset, u64 size)
	{
		return memory->map(offset, size);
	}

	void buffer::unmap()
	{
		memory->unmap();
	}

	u32 buffer::size() const
	{
		return static_cast<u32>(info.size);
	}
}
