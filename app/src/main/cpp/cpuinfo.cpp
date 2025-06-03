//
// Created by aenu on 2025/5/31.
//
#include "cpuinfo.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
std::vector<core_info_t> cpu_get_core_info(){
    std::ifstream cpuinfo("/proc/cpuinfo");
    if (!cpuinfo.is_open()) {
        return {};
    }

    std::vector<core_info_t> cores;
    core_info_t core;

    std::string line;
    while (std::getline(cpuinfo, line)) {

        if (line.find("processor") != std::string::npos) {
            core.processor = std::stoi(line.substr(line.find(":") + 2));
        }
        else if (line.find("CPU implementer") != std::string::npos) {
            core.implementer = std::stoi(line.substr(line.find(":") + 2), nullptr, 16);
        }
        else if (line.find("CPU variant") != std::string::npos) {
            core.variant = std::stoi(line.substr(line.find(":") + 2), nullptr, 16);
        }
        else if (line.find("CPU part") != std::string::npos) {
            core.part = std::stoi(line.substr(line.find(":") + 2), nullptr, 16);
            cores.push_back(core);
        }
        else if (line.find("Features") != std::string::npos) {
            //FIXME 转换其为编译feature
            std::string features = line.substr(line.find(":") + 2);
            std::istringstream iss(features);
            std::string feature;
            while (iss >> feature) {
                core.features.push_back(feature);
            }
        }
    }
    std::sort(cores.begin(), cores.end(), [](const core_info_t& a, const core_info_t& b) {
        return a.processor < b.processor;
    });
    return cores;
}

//map.find
bool operator<(const  core_info_t& lhs,const  core_info_t& rhs){
    if(lhs.implementer!= rhs.implementer) return lhs.implementer < rhs.implementer;
    if(lhs.part!= rhs.part) return lhs.part < rhs.part;
    return lhs.variant < rhs.variant;
}

std::string cpu_get_simple_info(const std::vector<core_info_t>& core_info_list){
    std::map<core_info_t, int> core_counts;
    for (const auto& core : core_info_list) {
        if(core_counts.find(core) == core_counts.end())
            core_counts[core] = 1;
        else
            core_counts[core]++;
    }
    std::stringstream ss;
    for (auto it=core_counts.rbegin(); it!=core_counts.rend();it++) {
        ss<<cpu_get_processor_name(it->first)<<"*"<<it->second<<"+";
    }
    std::string r = ss.str();
    return r.substr(0,r.size()-1);
}


std::set<core_info_t> get_processor_info_set(){
    std::vector<core_info_t> core_info_list = cpu_get_core_info();
    return std::set<core_info_t>(core_info_list.begin(), core_info_list.end());
}

std::set<std::string> get_processor_name_set(){
    std::vector<core_info_t> core_info_list = cpu_get_core_info();
    std::set<std::string> processor_name_set;
    std::for_each(core_info_list.begin(), core_info_list.end(), [&processor_name_set](const core_info_t& core_info) {
        processor_name_set.insert(cpu_get_processor_name(core_info));
    });
    return processor_name_set;
}


std::string cpu_get_processor_name(const core_info_t& core_info){
    static const std::map<std::tuple<int,int>,std::string> processor_map{

            //ARM
            {{ 0x41, 0xd04}, "cortex-a35" },
            {{ 0x41, 0xd03}, "cortex-a53" },
            {{ 0x41, 0xd07}, "cortex-a57" },
            {{ 0x41, 0xd08}, "cortex-a72" },
            {{ 0x41, 0xd09}, "cortex-a73" },

            {{ 0x41, 0xd05}, "cortex-a55" },
            {{ 0x41, 0xd0a}, "cortex-a75" },
            {{ 0x41, 0xd0b}, "cortex-a76" },
            {{ 0x41, 0xd0d}, "cortex-a77" },
            {{ 0x41, 0xd41}, "cortex-a78" },
            {{ 0x41, 0xd44}, "cortex-x1" },

            {{ 0x41, 0xd46}, "cortex-a510" },
            {{ 0x41, 0xd47}, "cortex-a710" },
            {{ 0x41, 0xd48}, "cortex-x2" },
            {{ 0x41, 0xd4d}, "cortex-a715" },
            {{ 0x41, 0xd4e}, "cortex-x3" },

            {{ 0x41, 0xd80}, "cortex-a520" },
            {{ 0x41, 0xd81}, "cortex-a720" },
            {{ 0x41, 0xd87}, "cortex-a725" },
            {{ 0x41, 0xd82}, "cortex-x4" },
            {{ 0x41, 0xd85}, "cortex-x925" },

            {{ 0x41, 0xd88}, "cortex-a520ae" },
            {{ 0x41, 0xd06}, "cortex-a65" },
            {{ 0x41, 0xd43}, "cortex-a65ae" },
            {{ 0x41, 0xd0e}, "cortex-a76ae" },
            {{ 0x41, 0xd0d}, "cortex-a77" },
            {{ 0x41, 0xd42}, "cortex-a78ae" },
            {{ 0x41, 0xd4b}, "cortex-a78c" },

            //高通
            {{ 0x51, 0x801}, "cortex-a73" },// Kryo 2xx Silver
            {{ 0x51, 0x802}, "cortex-a75" },// Kryo 3xx Gold
            {{ 0x51, 0x803}, "cortex-a75" },// Kryo 3xx Silver
            {{ 0x51, 0x804}, "cortex-a76" },// Kryo 4xx Gold
            {{ 0x51, 0x805}, "cortex-a76" },// Kryo 4xx/5xx Silver
            //{{ 0x51, 0xc00}, "falkor" },
            //{{ 0x51, 0xc01}, "saphira" },
            {{ 0x51, 0x001}, "oryon-1" },

            //海思
            //{{ 0x48, 0xd01}, "tsv110" },

            //三星
            {{ 0x53, 0x002}, "exynos-m3" },
            {{ 0x53, 0x003}, "exynos-m4" },
    };

    int implementer=core_info.implementer;
    int variant=core_info.variant;
    int part=core_info.part;

    if (auto core = processor_map.find(std::make_tuple(implementer, part)); core != processor_map.end()) {
        return core->second;
    }

    return "Unknown";
}