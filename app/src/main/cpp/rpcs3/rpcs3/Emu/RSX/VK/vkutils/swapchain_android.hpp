#pragma once

#include "swapchain_core.h"

namespace vk
{
#if defined(__ANDROID__)
    using swapchain_ANDROID = native_swapchain_base;
    using swapchain_NATIVE = swapchain_ANDROID;

    [[maybe_unused]] static
    VkSurfaceKHR make_WSI_surface(VkInstance vk_instance, display_handle_t window_handle, WSI_config* /*config*/)
    {

        VkSurfaceKHR result = VK_NULL_HANDLE;

        VkAndroidSurfaceCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
        createInfo.window =reinterpret_cast<ANativeWindow *>(window_handle);

        CHECK_RESULT(_vkCreateAndroidSurfaceKHR(vk_instance, &createInfo, NULL, &result));
        return result;
    }
#endif
}


