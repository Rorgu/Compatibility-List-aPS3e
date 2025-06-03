
#include <iostream>
#include <vector>
#include <string>
#include <jni.h>

#include <iostream>
#include <csignal>
#include <cstring>
#include <thread>
#include <vector>
#include <tuple>
#include <sys/mman.h>
#include <unistd.h>
#include <atomic>
#include "vkapi.h"
#include "vkutil.h"
#include "cpuinfo.h"

#if 0
std::string get_gpu_info() {
    VkApplicationInfo appinfo = {};
    appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appinfo.pNext = nullptr;
    appinfo.pApplicationName = "aps3e-test";
    appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appinfo.pEngineName = "nul";
    appinfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appinfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo inst_create_info = {};
    inst_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    inst_create_info.pApplicationInfo = &appinfo;

    VkInstance inst;
    if (_vkCreateInstance(&inst_create_info, nullptr, &inst)!= VK_SUCCESS) {
        return "获取gpu信息失败";;
    }

    // 获取物理设备数量
    uint32_t physicalDeviceCount = 0;
    _vkEnumeratePhysicalDevices(inst, &physicalDeviceCount, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    _vkEnumeratePhysicalDevices(inst, &physicalDeviceCount, physicalDevices.data());

    if (physicalDeviceCount != 1) {
        if (physicalDeviceCount == 0) {
            _vkDestroyInstance(inst, nullptr);
            return "没有有效的gpu!";
        }

        std::string result = "多个gpu! [";
        for (int i = 0; i < physicalDeviceCount; ++i) {

            VkPhysicalDeviceProperties deviceProperties;
            _vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);
            result += deviceProperties.deviceName;
            if (i != physicalDeviceCount - 1)
                result += ", ";
        }
        _vkDestroyInstance(inst, nullptr);
        return result+"]";
    }

    VkPhysicalDevice physicalDevice = physicalDevices[0];

    // 获取物理设备属性以获取设备名称
    VkPhysicalDeviceProperties deviceProperties;
    _vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

    uint32_t deviceExtensionCount = 0;
    _vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr);
    std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);
    _vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, deviceExtensions.data());

    std::ostringstream result;

    auto gen_vk_version = [](uint32_t v) {
        std::ostringstream oss;
        oss << (v >> 22) << "." << ((v >> 12) & 0x3ff) << "." << (v & 0xfff);
        return oss.str();
    };

    result << "GPU ["<<deviceProperties.deviceName<<"(Vulkan "<<gen_vk_version(deviceProperties.apiVersion)<<")]:\n\n";

auto pr_u64 = [](uint64_t v) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);

    const double GB = 1024.0 * 1024.0 * 1024.0;
    const double MB = 1024.0 * 1024.0;
    const double KB = 1024.0;

    if (v >= GB) {
        double value = v / GB;
        oss << (value == floor(value) ? std::setprecision(0) : std::setprecision(2))
            << value << "GB(" << v << ")";
    } else if (v >= MB) {
        double value = v / MB;
        oss << (value == floor(value) ? std::setprecision(0) : std::setprecision(2))
            << value << "MB(" << v << ")";
    } else if (v >= KB) {
        double value = v / KB;
        oss << (value == floor(value) ? std::setprecision(0) : std::setprecision(2))
            << value << "KB(" << v << ")";
    } else {
        oss << v ;
    }

    std::string result = oss.str();
    return result;
};

auto pr_u32 = [&](uint32_t v) {
    return pr_u64(v);
};

auto pr_f32 = [](float v) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4)<<v;
    return oss.str();
};
// 宏定义
#define w_u32(field)  result << "    " #field ": " << pr_u32(deviceProperties.limits.field) << "\n"
#define w_u64(field)  result << "    " #field ": " << pr_u64(deviceProperties.limits.field) << "\n"
#define w_f32(field)  result << "    " #field ": " << pr_f32(deviceProperties.limits.field) << "\n"

result << "limits:\n";

// 字段映射列表
w_u32(maxImageDimension1D);
w_u32(maxImageDimension2D);
w_u32(maxImageDimension3D);
w_u32(maxImageDimensionCube);
w_u32(maxImageArrayLayers);
w_u32(maxTexelBufferElements);
w_u32(maxUniformBufferRange);
w_u32(maxStorageBufferRange);
w_u32(maxPushConstantsSize);
w_u32(maxMemoryAllocationCount);
w_u32(maxSamplerAllocationCount);
w_u64(bufferImageGranularity);
w_u64(sparseAddressSpaceSize);
w_u32(maxBoundDescriptorSets);
w_u32(maxPerStageDescriptorSamplers);
w_u32(maxPerStageDescriptorUniformBuffers);
w_u32(maxPerStageDescriptorStorageBuffers);
w_u32(maxPerStageDescriptorSampledImages);
w_u32(maxPerStageDescriptorStorageImages);
w_u32(maxPerStageDescriptorInputAttachments);
w_u32(maxPerStageResources);
w_u32(maxDescriptorSetSamplers);
w_u32(maxDescriptorSetUniformBuffers);
w_u32(maxDescriptorSetUniformBuffersDynamic);
w_u32(maxDescriptorSetStorageBuffers);
w_u32(maxDescriptorSetStorageBuffersDynamic);
w_u32(maxDescriptorSetSampledImages);
w_u32(maxDescriptorSetStorageImages);
w_u32(maxDescriptorSetInputAttachments);
w_u32(maxVertexInputAttributes);
w_u32(maxVertexInputBindings);
w_u32(maxVertexInputAttributeOffset);
w_u32(maxVertexInputBindingStride);
w_u32(maxVertexOutputComponents);
w_u32(maxTessellationGenerationLevel);
w_u32(maxTessellationPatchSize);
w_u32(maxTessellationControlPerVertexInputComponents);
w_u32(maxTessellationControlPerVertexOutputComponents);
w_u32(maxTessellationControlPerPatchOutputComponents);
w_u32(maxTessellationControlTotalOutputComponents);
w_u32(maxTessellationEvaluationInputComponents);
w_u32(maxTessellationEvaluationOutputComponents);
w_u32(maxGeometryShaderInvocations);
w_u32(maxGeometryInputComponents);
w_u32(maxGeometryOutputComponents);
w_u32(maxGeometryOutputVertices);
w_u32(maxGeometryTotalOutputComponents);
w_u32(maxFragmentInputComponents);
w_u32(maxFragmentOutputAttachments);
w_u32(maxFragmentDualSrcAttachments);
w_u32(maxFragmentCombinedOutputResources);
w_u32(maxComputeSharedMemorySize);
result << "    maxComputeWorkGroupCount: [x="
       << pr_u32(deviceProperties.limits.maxComputeWorkGroupCount[0]) << ", y="
       << pr_u32(deviceProperties.limits.maxComputeWorkGroupCount[1]) << ", z="
       << pr_u32(deviceProperties.limits.maxComputeWorkGroupCount[2]) << "]\n";
w_u32(maxComputeWorkGroupInvocations);
result << "    maxComputeWorkGroupSize: [x="
       << pr_u32(deviceProperties.limits.maxComputeWorkGroupSize[0]) << ", y="
       << pr_u32(deviceProperties.limits.maxComputeWorkGroupSize[1]) << ", z="
       << pr_u32(deviceProperties.limits.maxComputeWorkGroupSize[2]) << "]\n";
w_u32(subPixelPrecisionBits);
w_u32(subTexelPrecisionBits);
w_u32(mipmapPrecisionBits);
w_u32(maxDrawIndexedIndexValue);
w_u32(maxDrawIndirectCount);
w_f32(maxSamplerLodBias);
w_f32(maxSamplerAnisotropy);
w_u32(maxViewports);
// 二维数组处理：
result << "    maxViewportDimensions: ["
       << pr_u32(deviceProperties.limits.maxViewportDimensions[0]) << " | "
       << pr_u32(deviceProperties.limits.maxViewportDimensions[1]) << "]\n";
result << "    viewportBoundsRange: ["
       << pr_f32(deviceProperties.limits.viewportBoundsRange[0]) << " | "
       << pr_f32(deviceProperties.limits.viewportBoundsRange[1]) << "]\n";
w_u32(viewportSubPixelBits);
w_u64(minMemoryMapAlignment);
w_u64(minTexelBufferOffsetAlignment);
w_u64(minUniformBufferOffsetAlignment);
w_u64(minStorageBufferOffsetAlignment);
w_u32(minTexelOffset);
w_u32(maxTexelOffset);
w_u32(minTexelGatherOffset);
w_u32(maxTexelGatherOffset);
w_f32(minInterpolationOffset);
w_f32(maxInterpolationOffset);
w_u32(subPixelInterpolationOffsetBits);
w_u32(maxFramebufferWidth);
w_u32(maxFramebufferHeight);
w_u32(maxFramebufferLayers);
w_u32(framebufferColorSampleCounts);
w_u32(framebufferDepthSampleCounts);
w_u32(framebufferStencilSampleCounts);
w_u32(framebufferNoAttachmentsSampleCounts);
w_u32(maxColorAttachments);
w_u32(sampledImageColorSampleCounts);
w_u32(sampledImageIntegerSampleCounts);
w_u32(sampledImageDepthSampleCounts);
w_u32(sampledImageStencilSampleCounts);
w_u32(storageImageSampleCounts);
w_u32(maxSampleMaskWords);
w_u32(timestampComputeAndGraphics);
w_f32(timestampPeriod);
w_u32(maxClipDistances);
w_u32(maxCullDistances);
w_u32(maxCombinedClipAndCullDistances);
w_u32(discreteQueuePriorities);
// 浮点数组处理：
result << "    pointSizeRange: ["
       << pr_f32(deviceProperties.limits.pointSizeRange[0]) << " | "
       << pr_f32(deviceProperties.limits.pointSizeRange[1]) << "]\n";
result << "    lineWidthRange: ["
       << pr_f32(deviceProperties.limits.lineWidthRange[0]) << " | "
       << pr_f32(deviceProperties.limits.lineWidthRange[1]) << "]\n";
w_f32(pointSizeGranularity);
w_f32(lineWidthGranularity);
w_u32(strictLines);
w_u32(standardSampleLocations);
w_u64(optimalBufferCopyOffsetAlignment);
w_u64(optimalBufferCopyRowPitchAlignment);
w_u64(nonCoherentAtomSize);

#undef w_f32
#undef w_u32
#undef w_u64

result << "\n";

result << "extensions:\n";
    for (const auto& deviceExt : deviceExtensions) {
        result << "    * " << deviceExt.extensionName<< "\n";
    }

    vkDestroyInstance(inst, nullptr);
    return result.str();
}
#endif

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#if 0
std::string get_cpu_info() {
    std::ifstream cpuinfo("/proc/cpuinfo");
    if (!cpuinfo.is_open()) {
        return "获取cpuinfo失败";
    }

    struct CoreInfo {
        int implementer;
        int variant;
        int part;
        std::vector<std::string> features;
    };

    std::vector<CoreInfo> cores;
    std::map<std::tuple<int,int,int>,int> core_n;

    std::string line;

    CoreInfo core;

    while (std::getline(cpuinfo, line)) {

        if (line.find("CPU implementer") != std::string::npos) {
            core.implementer = std::stoi(line.substr(line.find(":") + 2), nullptr, 16);
        }
        else if (line.find("CPU variant") != std::string::npos) {
            core.variant = std::stoi(line.substr(line.find(":") + 2), nullptr, 16);
        }
        else if (line.find("CPU part") != std::string::npos) {
            core.part = std::stoi(line.substr(line.find(":") + 2), nullptr, 16);
            cores.push_back(core);
            if (core_n.find({core.implementer, core.variant, core.part}) == core_n.end()) {
                core_n[{core.implementer, core.variant, core.part}] = 1;
            }
            else {
                core_n[{core.implementer, core.variant, core.part}]++;
            }
        }
        else if (line.find("Features") != std::string::npos) {
            std::string features = line.substr(line.find(":") + 2);
            std::istringstream iss(features);
            std::string feature;
            while (iss >> feature) {
               core.features.push_back(feature);
            }
        }
    }

    auto cpu_core_info = [&]() {

        static const std::map<std::tuple<int,int>,std::string> core_map{

            {{ 0x41, 0xd01}, "Cortex-A32" },
            {{ 0x41, 0xd04}, "Cortex-A35" },
            {{ 0x41, 0xd03}, "Cortex-A53" },
            {{ 0x41, 0xd07}, "Cortex-A57" },
            {{ 0x41, 0xd08}, "Cortex-A72" },
            {{ 0x41, 0xd09}, "Cortex-A73" },

            {{ 0x41, 0xd05}, "Cortex-A55" },
            {{ 0x41, 0xd0a}, "Cortex-A75" },
            {{ 0x41, 0xd0b}, "Cortex-A76" },
            {{ 0x41, 0xd0d}, "Cortex-A77" },
            {{ 0x41, 0xd41}, "Cortex-A78" },
            {{ 0x41, 0xd44}, "Cortex-X1" },

            {{ 0x41, 0xd46}, "Cortex-A510" },
            {{ 0x41, 0xd47}, "Cortex-A710" },
            {{ 0x41, 0xd48}, "Cortex-X2" },
            {{ 0x41, 0xd4d}, "Cortex-A715" },
            {{ 0x41, 0xd4e}, "Cortex-X3" },

            {{ 0x41, 0xd80}, "Cortex-A520" },
            {{ 0x41, 0xd81}, "Cortex-A720" },
            {{ 0x41, 0xd87}, "Cortex-A725" },
            {{ 0x41, 0xd82}, "Cortex-X4" },
            {{ 0x41, 0xd85}, "Cortex-X925" },

            {{ 0x51, 0x001}, "X-Elite" },
        };

        std::ostringstream ss;
        for(const auto& [k,v] : core_n){
            auto &[implementer, variant, part] = k;
            if (auto core = core_map.find(std::make_tuple(implementer, part)); core != core_map.end()) {
                ss <<core->second << "*"<<  v << "+";
            }
            else{
                ss <<"[" <<implementer<<" | "<<part<< "]*"<<  v << "+";
            }
        }
        std::string r=ss.str();
        if(r.size()<1) return "获取cpu信息失败"s;
        return r.substr(0,r.size()-1);
    }();

    std::stringstream result;

    result << "CPU [" << cpu_core_info << "]:\n";

    if  (cores.size() > 0) {
        //TODO 理论上来讲，所有核心的feature应该都是相同的
        for (const auto& frature : cores[0].features) {
            result << "    * " << frature << "\n";
        }
    }

    return result.str();
}
#endif
/*
int main() {
	std::cout<< get_supported_extensions() << std::endl;
}*/

std::string get_auther_info(){
	return "b站 路人aenu \n" \
	"个人主页https://aenu.cc \n";
}

std::pair<std::string,bool> vk_lib_info(){
    const std::pair<std::string,bool> dedault_info={"libvulkan.so",false};
    const char* cfg_path=getenv("APS3E_CONFIG_YAML_PATH");
    if(!cfg_path)
        return dedault_info;
    if(!std::filesystem::exists(cfg_path))
        return dedault_info;

    YAML::Node config_node = YAML::LoadFile(cfg_path);
    if(!config_node.IsDefined())
        return dedault_info;
    YAML::Node use_custom_driver=config_node["Video"]["Vulkan"]["Use Custom Driver"];
    if(!use_custom_driver.IsDefined())
        return dedault_info;
    if(!use_custom_driver.as<bool>())
        return dedault_info;

    YAML::Node custom_lib_path=config_node["Video"]["Vulkan"]["Custom Driver Library Path"];
    if(!custom_lib_path.IsDefined())
        return dedault_info;
    std::string custom_lib_path_str=custom_lib_path.as<std::string>();
    if(custom_lib_path_str.empty()||!std::filesystem::exists(custom_lib_path_str))
        return dedault_info;
    return {custom_lib_path_str,true};
}


std::string test_compute_pipeline_source(){
   return R"(
#version 430
layout(local_size_x=32, local_size_y=1, local_size_z=1) in;
layout(set = 0, binding=0, std430) buffer ssbo{ uint data[]; };


#define KERNEL_SIZE 1

// Generic swap routines
#define bswap_u16(bits)     (bits & 0xFF) << 8 | (bits & 0xFF00) >> 8 | (bits & 0xFF0000) << 8 | (bits & 0xFF000000) >> 8
#define bswap_u32(bits)     (bits & 0xFF) << 24 | (bits & 0xFF00) << 8 | (bits & 0xFF0000) >> 8 | (bits & 0xFF000000) >> 24
#define bswap_u16_u32(bits) (bits & 0xFFFF) << 16 | (bits & 0xFFFF0000) >> 16

// Depth format conversions
#define d24_to_f32(bits)             floatBitsToUint(float(bits) / 16777215.f)
#define f32_to_d24(bits)             uint(uintBitsToFloat(bits) * 16777215.f)
#define d24f_to_f32(bits)            (bits << 7)
#define f32_to_d24f(bits)            (bits >> 7)
#define d24x8_to_f32(bits)           d24_to_f32(bits >> 8)
#define d24x8_to_d24x8_swapped(bits) (bits & 0xFF00) | (bits & 0xFF0000) >> 16 | (bits & 0xFF) << 16
#define f32_to_d24x8_swapped(bits)   d24x8_to_d24x8_swapped(f32_to_d24(bits))

uint linear_invocation_id()
{
	uint size_in_x = (gl_NumWorkGroups.x * gl_WorkGroupSize.x);
	return (gl_GlobalInvocationID.y * size_in_x) + gl_GlobalInvocationID.x;
}


void main()
{
	uint invocation_id = linear_invocation_id();
	uint index = invocation_id * KERNEL_SIZE;
	uint value;


	{
		value = data[index];
		data[index] = bswap_u32(value);
	}
}
)";
}

std::string get_gpu_info(){
    std::pair<std::string,bool> lib_info=vk_lib_info();
    vk_load(lib_info.first.c_str(),lib_info.second);

    struct clean_t{
        std::vector<std::function<void()>> funcs;
        ~clean_t(){
            for(auto it=funcs.rbegin();it!=funcs.rend();it++){
                (*it)();
            }
        }
    }clean;

    clean.funcs.push_back([](){
        vk_unload();
    });

    std::optional<VkInstance> inst=vk_create_instance("aps3e-gpu_info");
    if(!inst) {
        return "获取gpu信息失败";
    }

    clean.funcs.push_back([&](){
        vk_destroy_instance(*inst);
    });

    if(int count=vk_get_physical_device_count(*inst);count!=1) {

        if(count<1){
            return "获取gpu信息失败";
        }
        if(count>1){
            return "多个gpu!";
        }
    }
    if(auto pdev=vk_get_physical_device(*inst);pdev) {
        std::string gpu_name=vk_get_physical_device_properties(*pdev).deviceName;
        std::string gpu_vk_ver=[](uint32_t v) {
            std::ostringstream oss;
            oss << (v >> 22) << "." << ((v >> 12) & 0x3ff) << "." << (v & 0xfff);
            return oss.str();
        }(vk_get_physical_device_properties(*pdev).apiVersion);

        std::string gpu_ext=[&]() {
            std::ostringstream oss;
            for (auto ext : vk_get_physical_device_extension_properties(*pdev)) {
                oss <<"    * " << ext.extensionName << "\n";
            }
            return oss.str();
        }();
#if 0
        VkQueueFamilyProperties queue_family_props=vk_get_queue_family_properties(*pdev,0);
        if(auto dev=vk_create_device(*pdev,0,queue_family_props);dev){
            clean.funcs.push_back([&](){
                vk_destroy_device(*dev);
            });
            std::vector<VkDescriptorSetLayoutBinding> binds;
            binds.push_back(
                    VkDescriptorSetLayoutBinding{0,VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,1,VK_SHADER_STAGE_COMPUTE_BIT,nullptr});
            auto descriptor_set_layout=vk_create_descriptor_set_layout(*dev,binds);
            clean.funcs.push_back([&](){
                vk_destroy_descriptor_set_layout(*dev,*descriptor_set_layout);
            });
            auto pipeline_layout=vk_create_pipeline_layout(*dev,*descriptor_set_layout);
            clean.funcs.push_back([&](){
                vk_destroy_pipeline_layout(*dev,*pipeline_layout);
            });
            std::optional<std::vector<uint32_t>> spv=vk_compile_glsl_to_spv(*dev,test_compute_pipeline_source(),vk_get_physical_device_limits(*pdev));
            auto module=vk_create_shader_module(*dev,*spv);

            clean.funcs.push_back([&](){
                vk_destroy_shader_module(*dev,*module);
            });

            auto pipeline=vk_create_compute_pipeline(*dev,*pipeline_layout,*module);
            if(pipeline){
                gpu_name+=":ok";
                clean.funcs.push_back([&](){
                    vk_destroy_pipeline(*dev,*pipeline);
                });
                return "GPU [" + gpu_name +"(VK: "+gpu_vk_ver+ ")]:\n" + gpu_ext;
            }

        }
#endif
        return "GPU [" + gpu_name +"(VK: "+gpu_vk_ver+ ")]:\n" + gpu_ext;

    }
    return "获取gpu信息失败";

}

std::string get_cpu_info() {

    std::vector<core_info_t> core_info=cpu_get_core_info();
    std::string cpu_name=cpu_get_simple_info(core_info);
    std::string cpu_features=[&](){
        std::ostringstream oss;
        for(const auto& feature : core_info[0].features){
            oss <<"    * " << feature << "\n";
        }
        return oss.str();
    }();
    return "CPU [" + cpu_name + "]:\n" + cpu_features;
}

extern "C"
{
	JNIEXPORT jstring JNICALL Java_aenu_proptest_HelloJni_stringFromJNI(JNIEnv* env, jobject thiz)
	{

		std::string vv;//=get_auther_info();

        vv+=get_cpu_info();
		vv+="\n"+get_gpu_info();

		return env->NewStringUTF(vv.c_str());
	}
}
