//
// Created by aenu on 2025/6/1.
// SPDX-License-Identifier: WTFPL
//
#include "glsl2spv.h"

namespace{
    TBuiltInResource resource;
}
void glsl2spv_init(const VkPhysicalDeviceLimits& limits){

    resource.maxLights = 32;
    resource.maxClipPlanes = 6;
    resource.maxTextureUnits = 32;
    resource.maxTextureCoords = 32;
    resource.maxVertexAttribs = 64;

    resource.maxVertexUniformComponents = 4096;
    resource.maxVaryingFloats = 64;
    resource.maxVertexTextureImageUnits = 32;
    resource.maxCombinedTextureImageUnits = 80;
    resource.maxTextureImageUnits = 32;

    resource.maxFragmentUniformComponents = 4096;
    resource.maxDrawBuffers = 32;
    resource.maxVertexUniformVectors = 128;
    resource.maxVaryingVectors = 8;
    resource.maxFragmentUniformVectors = 16;

    resource.maxVertexOutputVectors = limits.maxVertexOutputComponents;
    resource.maxFragmentInputVectors = limits.maxFragmentInputComponents;
    resource.minProgramTexelOffset = limits.minTexelOffset;
    resource.maxProgramTexelOffset = limits.maxTexelOffset;
    resource.maxClipDistances = limits.maxClipDistances;

    resource.maxComputeWorkGroupCountX = limits.maxComputeWorkGroupCount[0];
    resource.maxComputeWorkGroupCountY = limits.maxComputeWorkGroupCount[1];
    resource.maxComputeWorkGroupCountZ = limits.maxComputeWorkGroupCount[2];
    resource.maxComputeWorkGroupSizeX = limits.maxComputeWorkGroupSize[0];
    resource.maxComputeWorkGroupSizeY = limits.maxComputeWorkGroupSize[1];
    resource.maxComputeWorkGroupSizeZ = limits.maxComputeWorkGroupSize[2];

    resource.maxComputeUniformComponents = 1024;
    resource.maxComputeTextureImageUnits = 16;
    resource.maxComputeImageUniforms = 8;
    resource.maxComputeAtomicCounters = 8;
    resource.maxComputeAtomicCounterBuffers = 1;
    resource.maxVaryingComponents = 60;

    resource.maxVertexOutputComponents = limits.maxVertexOutputComponents;
    resource.maxGeometryInputComponents = limits.maxGeometryInputComponents;
    resource.maxGeometryOutputComponents = limits.maxGeometryOutputComponents;
    resource.maxFragmentInputComponents = limits.maxFragmentInputComponents;

    resource.maxImageUnits = 8;
    resource.maxCombinedImageUnitsAndFragmentOutputs = 8;
    resource.maxCombinedShaderOutputResources = 8;
    resource.maxImageSamples = 0;

    resource.maxVertexImageUniforms = 0;
    resource.maxTessControlImageUniforms = 0;
    resource.maxTessEvaluationImageUniforms = 0;

    resource.maxGeometryImageUniforms = 0;
    resource.maxFragmentImageUniforms = 0;
    resource.maxCombinedImageUniforms = 0;

    resource.maxGeometryTextureImageUnits = 16;
    resource.maxGeometryOutputVertices = limits.maxGeometryOutputVertices;
    resource.maxGeometryTotalOutputComponents = limits.maxGeometryTotalOutputComponents;
    resource.maxGeometryUniformComponents = 1024;
    resource.maxGeometryVaryingComponents = 64;

    resource.maxTessControlInputComponents = 128;
    resource.maxTessControlOutputComponents = 128;
    resource.maxTessControlTextureImageUnits = 16;
    resource.maxTessControlUniformComponents = 1024;
    resource.maxTessControlTotalOutputComponents = 4096;
    resource.maxTessEvaluationInputComponents = 128;
    resource.maxTessEvaluationOutputComponents = 128;
    resource.maxTessEvaluationTextureImageUnits = 16;
    resource.maxTessEvaluationUniformComponents = 1024;
    resource.maxTessPatchComponents = 120;

    resource.maxPatchVertices = 32;
    resource.maxTessGenLevel = limits.maxTessellationGenerationLevel;
    resource.maxViewports = limits.maxViewports;

    resource.maxVertexAtomicCounters = 0;
    resource.maxTessControlAtomicCounters = 0;
    resource.maxTessEvaluationAtomicCounters = 0;
    resource.maxGeometryAtomicCounters = 0;
    resource.maxFragmentAtomicCounters = 0;
    resource.maxCombinedAtomicCounters = 0;
    resource.maxAtomicCounterBindings = 0;
    resource.maxVertexAtomicCounterBuffers = 0;
    resource.maxTessControlAtomicCounterBuffers = 0;
    resource.maxTessEvaluationAtomicCounterBuffers = 0;
    resource.maxGeometryAtomicCounterBuffers = 0;
    resource.maxFragmentAtomicCounterBuffers = 0;
    resource.maxCombinedAtomicCounterBuffers = 0;
    resource.maxAtomicCounterBufferSize = 16384;

    resource.maxTransformFeedbackBuffers = 4;
    resource.maxTransformFeedbackInterleavedComponents = 64;
    resource.maxCullDistances = 8;
    resource.maxCombinedClipAndCullDistances = 8;
    resource.maxSamples = 4;

    /*
    int maxMeshOutputVerticesNV;
    int maxMeshOutputPrimitivesNV;
    int maxMeshWorkGroupSizeX_NV;
    int maxMeshWorkGroupSizeY_NV;
    int maxMeshWorkGroupSizeZ_NV;
    int maxTaskWorkGroupSizeX_NV;
    int maxTaskWorkGroupSizeY_NV;
    int maxTaskWorkGroupSizeZ_NV;
    int maxMeshViewCountNV;
    int maxMeshOutputVerticesEXT;
    int maxMeshOutputPrimitivesEXT;
    int maxMeshWorkGroupSizeX_EXT;
    int maxMeshWorkGroupSizeY_EXT;
    int maxMeshWorkGroupSizeZ_EXT;
    int maxTaskWorkGroupSizeX_EXT;
    int maxTaskWorkGroupSizeY_EXT;
    int maxTaskWorkGroupSizeZ_EXT;
    int maxMeshViewCountEXT;
    int maxDualSourceDrawBuffersEXT;
     */

    resource.limits= {
            .nonInductiveForLoops = true,
            .whileLoops = true,
            .doWhileLoops = true,
            .generalUniformIndexing = true,
            .generalAttributeMatrixVectorIndexing = true,
            .generalVaryingIndexing = true,
            .generalSamplerIndexing = true,
            .generalVariableIndexing = true,
            .generalConstantMatrixVectorIndexing = true,
    };

    glslang::InitializeProcess();
}
std::optional<std::vector<uint32_t >> glsl2spv_compile(const std::string& source,EShLanguage lang){
    glslang::TShader shader(lang);

    shader.setEnvInput(glslang::EShSourceGlsl,lang,glslang::EShClientVulkan,450);
    shader.setEnvClient(glslang::EShClientVulkan,glslang::EShTargetVulkan_1_0);
    shader.setEnvTarget(glslang::EshTargetSpv,glslang::EShTargetSpv_1_0);

    const char* shader_source = source.c_str();
    shader.setStrings(&shader_source,1);

    EShMessages  messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules);
    if(!shader.parse(&resource,450,false,messages)){
        return std::nullopt;
    }

    glslang::TProgram program;
    program.addShader(&shader);
    if(!program.link(messages)){
        return std::nullopt;
    }

    std::vector<uint32_t> spv;
    glslang::GlslangToSpv(*program.getIntermediate(lang),spv);
    return spv;
}

void glsl2spv_finalize(){
    glslang::FinalizeProcess();
}