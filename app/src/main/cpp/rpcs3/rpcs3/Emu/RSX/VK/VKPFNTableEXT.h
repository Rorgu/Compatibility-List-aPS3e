
#if defined(INSTANCE_VK_FUNCTION)
//instance
VK_FUNC(vkCreateDebugReportCallbackEXT);
VK_FUNC(vkDestroyDebugReportCallbackEXT);

VK_FUNC(vkGetPhysicalDeviceFeatures2KHR);
VK_FUNC(vkGetPhysicalDeviceProperties2KHR);

VK_FUNC(vkGetPhysicalDeviceSurfaceCapabilities2KHR);

#endif


#if defined(DEVICE_VK_FUNCTION)
//device

/*
VK_FUNC(vkCreateSwapchainKHR);
VK_FUNC(vkDestroySwapchainKHR);

VK_FUNC(vkGetSwapchainImagesKHR);

VK_FUNC(vkAcquireNextImageKHR);

VK_FUNC(vkQueuePresentKHR);
 */

// EXT_conditional_rendering
VK_FUNC(vkCmdBeginConditionalRenderingEXT);
VK_FUNC(vkCmdEndConditionalRenderingEXT);

// EXT_debug_utils
VK_FUNC(vkSetDebugUtilsObjectNameEXT);
VK_FUNC(vkQueueInsertDebugUtilsLabelEXT);
VK_FUNC(vkCmdInsertDebugUtilsLabelEXT);

// KHR_synchronization2
VK_FUNC(vkCmdSetEvent2KHR);
VK_FUNC(vkCmdWaitEvents2KHR);
VK_FUNC(vkCmdPipelineBarrier2KHR);

// EXT_device_fault
VK_FUNC(vkGetDeviceFaultInfoEXT);

// EXT_multi_draw
VK_FUNC(vkCmdDrawMultiEXT);
VK_FUNC(vkCmdDrawMultiIndexedEXT);

// EXT_external_memory_host
VK_FUNC(vkGetMemoryHostPointerPropertiesEXT);

#endif
