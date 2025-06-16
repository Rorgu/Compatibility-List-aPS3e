
// SPDX-License-Identifier: WTFPL
// Created by aenu on 2025/5/29.
//
#include <dlfcn.h>
#include <string>
#include "vkapi.h"

#include "rpcs3/3rdparty/libadrenotools/include/adrenotools/priv.h"
#include "rpcs3/3rdparty/libadrenotools/include/adrenotools/driver.h"

#if 0
#define VKFN(func) PFN_##func _##func
#include "vksym.h"
#undef VKFN
#endif

namespace {
    void* lib_handle = nullptr;
}
void vk_load(const char* lib_path,bool is_adreno_custom){
    if (lib_handle) return;
    if(!is_adreno_custom)
    lib_handle = dlopen(lib_path, RTLD_NOW);
    else{
        std::string hook_dir=std::string(getenv("APS3E_NATIVE_LIB_DIR"))+'/';
        std::string custom_lib_path=lib_path;
        std::string custom_lib_dir=custom_lib_path.substr(0,custom_lib_path.find_last_of('/')+1);
        std::string custom_lib_name=custom_lib_path.substr(custom_lib_path.find_last_of('/')+1);

        lib_handle= adrenotools_open_libvulkan(RTLD_NOW,ADRENOTOOLS_DRIVER_CUSTOM,nullptr
                ,hook_dir.c_str()
                ,custom_lib_dir.c_str()
                ,custom_lib_name.c_str()
                ,nullptr,nullptr);
    }
#define VKFN(func) _##func=reinterpret_cast<PFN_##func>(dlsym(lib_handle, #func))
#include "vksym.h"
#undef VKFN
}
void vk_unload(){
    if (!lib_handle) return;
    dlclose(lib_handle);
    lib_handle = nullptr;
#define VKFN(func) _##func=nullptr
#include "vksym.h"
#undef VKFN
}