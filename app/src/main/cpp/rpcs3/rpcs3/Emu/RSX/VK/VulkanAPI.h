#pragma once

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__APPLE__)
#define VK_USE_PLATFORM_MACOS_MVK
#elif defined(__ANDROID__)
#define VK_USE_PLATFORM_ANDROID_KHR
#define VK_NO_PROTOTYPES
#elif HAVE_X11
#define VK_USE_PLATFORM_XLIB_KHR
#endif

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4005 )
#endif

#include <vulkan/vulkan.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <util/types.hpp>

#if VK_HEADER_VERSION < 287
constexpr VkDriverId VK_DRIVER_ID_MESA_HONEYKRISP = static_cast<VkDriverId>(26);
#endif

extern bool cfg_vertex_buffer_upload_mode_use_buffer_view();

#define DECL_VK_FUNCTION 1
#include "VKPFNTable.h"
#undef DECL_VK_FUNCTION

#if 0
#define VK_FUNC(func) extern PFN_##func _##func
#define INSTANCE_VK_FUNCTION
#define DEVICE_VK_FUNCTION
#include "VKPFNTableEXT.h"
#undef INSTANCE_VK_FUNCTION
#undef DEVICE_VK_FUNCTION
#undef VK_FUNC
#endif
namespace vk
{
    void init_base_pfn();
	void init_instance_pfn(VkInstance instance);
	void init_device_pfn(VkDevice device);
}
