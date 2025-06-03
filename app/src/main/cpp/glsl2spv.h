//
// Created by aenu on 2025/6/1.
// SPDX-License-Identifier: WTFPL
//

#ifndef APS3E_GLSL2SPV_H
#define APS3E_GLSL2SPV_H

#include <optional>
#include <vector>
#include <cstdint>
#include "vkapi.h"
#include "glslang/Public/ShaderLang.h"
#include "glslang/SPIRV/GlslangToSpv.h"
#include "glslang/Include/glslang_c_interface.h"

void glsl2spv_init(const VkPhysicalDeviceLimits& limits);
std::optional<std::vector<uint32_t >> glsl2spv_compile(const std::string& source,EShLanguage lang);
void glsl2spv_finalize();

#endif //APS3E_GLSL2SPV_H
