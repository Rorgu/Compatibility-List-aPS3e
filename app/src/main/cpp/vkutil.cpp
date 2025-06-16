
// SPDX-License-Identifier: WTFPL
// Created by aenu on 2025/5/31.
//

 #include "vkutil.h"
 #include "glsl2spv.h"

#include <android/log.h>

#define LOG_TAG "vkutil"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
 #define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
std::optional<VkInstance> vk_create_instance(const char * name) {
        VkApplicationInfo appinfo = {};
        appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appinfo.pNext = nullptr;
        appinfo.pApplicationName = name;
        appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appinfo.pEngineName = name;
        appinfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appinfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo inst_create_info = {};
        inst_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        inst_create_info.pApplicationInfo = &appinfo;

        VkInstance inst;
        if (_vkCreateInstance(&inst_create_info, nullptr, &inst)!= VK_SUCCESS) {
            return std::nullopt;
        }
        return inst;
}

void vk_destroy_instance(VkInstance inst) {
    _vkDestroyInstance(inst, nullptr);
}

int vk_get_physical_device_count(VkInstance inst){
    uint32_t count;
    _vkEnumeratePhysicalDevices(inst, &count, nullptr);
    return count;
}
std::optional<VkPhysicalDevice> vk_get_physical_device(VkInstance inst, int index){
    uint32_t count = vk_get_physical_device_count(inst);
     if (count == 0)
        return std::nullopt;
     std ::vector<VkPhysicalDevice> devices(count);
     _vkEnumeratePhysicalDevices(inst, &count, devices.data());
     return devices[index];
}

 VkPhysicalDeviceProperties vk_get_physical_device_properties(VkPhysicalDevice dev){
    VkPhysicalDeviceProperties props;
    _vkGetPhysicalDeviceProperties(dev, &props);
    return props;
}

VkPhysicalDeviceLimits vk_get_physical_device_limits(VkPhysicalDevice dev){
    return  vk_get_physical_device_properties(dev).limits;
}

std::vector<VkExtensionProperties> vk_get_physical_device_extension_properties(VkPhysicalDevice dev){
    uint32_t count;
    _vkEnumerateDeviceExtensionProperties(dev, nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> props(count);
    _vkEnumerateDeviceExtensionProperties(dev, nullptr, &count, props.data());
    std::sort(props.begin(), props.end(), [](const VkExtensionProperties& a, const VkExtensionProperties& b) {
         return strcmp(a.extensionName, b.extensionName) < 0;
    });
    return props;
}

int vk_get_queue_family_properties_count(VkPhysicalDevice dev) {
    uint32_t count;
    _vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, nullptr);
    return count;
}

VkQueueFamilyProperties vk_get_queue_family_properties(VkPhysicalDevice dev,int index) {
    uint32_t count= vk_get_queue_family_properties_count( dev);
    std::vector<VkQueueFamilyProperties> props(count);
    _vkGetPhysicalDeviceQueueFamilyProperties(dev, &count, props.data());
    return props[index];
}

std::optional<VkDevice> vk_create_device(VkPhysicalDevice pdev,uint32_t queueFamilyIndex,VkQueueFamilyProperties  props) {
    float  queue_priority = 0.0f;
    VkDeviceQueueCreateInfo queue_create_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = queueFamilyIndex,
            .queueCount = props.queueCount,
            .pQueuePriorities = &queue_priority
    };

    VkDeviceCreateInfo device_create_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &queue_create_info,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = 0,
            .ppEnabledExtensionNames = nullptr,
            .pEnabledFeatures = nullptr
    };
    VkDevice dev;
    if(_vkCreateDevice(pdev, &device_create_info, nullptr, &dev) != VK_SUCCESS){
         return std::nullopt;
    }
    return dev;
}

void vk_destroy_device(VkDevice dev) {
    _vkDestroyDevice(dev, nullptr);
}

std::optional<VkDescriptorSetLayout> vk_create_descriptor_set_layout(VkDevice dev,const std::vector<VkDescriptorSetLayoutBinding>& binds) {
    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .bindingCount = static_cast<uint32_t>(binds.size()),
            .pBindings = binds .data()
    };
    VkDescriptorSetLayout layout;
    if(_vkCreateDescriptorSetLayout( dev, &descriptor_set_layout_create_info, nullptr, &layout)!= VK_SUCCESS){
        LOGE( "create descriptor set layout failed");
        return std::nullopt;
    }
     return layout;
 }

  void vk_destroy_descriptor_set_layout(VkDevice dev,VkDescriptorSetLayout layout) {
     _vkDestroyDescriptorSetLayout(dev,layout, nullptr);
 }

std::optional<VkPipelineLayout> vk_create_pipeline_layout(VkDevice dev,VkDescriptorSetLayout descriptor_set_layout) {
    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = 1,
            .pSetLayouts = &descriptor_set_layout,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = nullptr
    };
     VkPipelineLayout layout;
     if(_vkCreatePipelineLayout(dev, &pipeline_layout_create_info, nullptr, &layout)!= VK_SUCCESS){
          LOGE( "create pipeline layout failed");
         return std::nullopt;
     }
     return layout;
}

std::optional<std::vector<uint32_t>> vk_compile_glsl_to_spv(VkDevice dev,const std::string& source,VkPhysicalDeviceLimits limits) {
    glsl2spv_init(limits);
    struct clean_t{
        ~ clean_t(){
            glsl2spv_finalize();
        }
    }  clean;
     return glsl2spv_compile(source, EShLangCompute);
}

std::optional<VkShaderModule> vk_create_shader_module(VkDevice dev,const std::vector<uint32_t>& code) {
     VkShaderModuleCreateInfo shader_module_create_info = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = code.size() * sizeof(uint32_t),
            .pCode = code.data()
    };
     VkShaderModule module;
     if(_vkCreateShaderModule(dev, &shader_module_create_info, nullptr, &module)!= VK_SUCCESS){
          LOGE( "create shader module failed");
         return std::nullopt;
     }
     return module;
}

void vk_destroy_shader_module(VkDevice dev,VkShaderModule module){
    _vkDestroyShaderModule(dev, module, nullptr);
}

void vk_destroy_pipeline_layout(VkDevice dev,VkPipelineLayout layout) {
    _vkDestroyPipelineLayout(dev, layout, nullptr);
}

std::optional <VkPipeline> vk_create_compute_pipeline(VkDevice dev,VkPipelineLayout layout,VkShaderModule module) {
    VkComputePipelineCreateInfo compute_pipeline_create_info = {
            .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .stage = VK_SHADER_STAGE_COMPUTE_BIT,
                    .module = module,
                    .pName = "main",
            },
            .layout = layout,
            .basePipelineHandle= VK_NULL_HANDLE,
            .basePipelineIndex = -1,
     };
    VkPipeline pipeline;
     if(_vkCreateComputePipelines(dev, VK_NULL_HANDLE, 1, &compute_pipeline_create_info, nullptr, &pipeline)!= VK_SUCCESS){
          LOGE( "create pipeline failed");
        return std::nullopt;
    }
     return pipeline;
}

void  vk_destroy_pipeline(VkDevice dev,VkPipeline pipeline) {
    _vkDestroyPipeline(dev, pipeline, nullptr);
}