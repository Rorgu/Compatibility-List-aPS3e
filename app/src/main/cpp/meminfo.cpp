//
// Created by aenu on 2025/6/23.
// SPDX-License-Identifier: WTFPL
//
#include "meminfo.h"
#include <fstream>
#include <sstream>

bool operator==(const mem_map_entry_t& lhs, const mem_map_entry_t& rhs) {
    return lhs.start_addr == rhs.start_addr&&lhs.end_addr == rhs.end_addr;
}

bool operator!=(const mem_map_entry_t& lhs, const mem_map_entry_t& rhs) {
    return !(lhs == rhs);
}

std::vector<mem_map_entry_t> meminfo_update(const std::vector<mem_map_entry_t>& origin) {
    const std::vector<mem_map_entry_t>& start_entries=origin;
    std::vector<mem_map_entry_t> entries;
    std::ifstream maps_file("/proc/self/maps");
    std::string line;

    while (std::getline(maps_file, line)) {
        std::istringstream iss(line);
        mem_map_entry_t entry;
        char dash;
        char colon;
        iss >> std::hex >> entry.start_addr >> dash >> entry.end_addr;
        iss >> entry.permissions;
        iss >> std::hex >> entry.offset;
        iss >> std::dec >> entry.dev_major >> colon >> entry.dev_minor;
        iss >> entry.inode;
        std::getline(iss, entry.pathname);
        if (!entry.pathname.empty() && entry.pathname[0] == ' ') {
            entry.pathname.erase(0, 1);
        }
        if(auto it =std::find(start_entries.begin(), start_entries.end(), entry);it ==start_entries.end()){
            entries.push_back(entry);
        }
    }

    return entries;
}


uint64_t meminfo_calc_total_mem(const std::vector<mem_map_entry_t>& mem_map){
    uint64_t total_mem = 0;
    for (auto& entry : mem_map) {
        if (entry.permissions.find('r') != std::string::npos) {
            total_mem += reinterpret_cast<uintptr_t>(entry.end_addr) - reinterpret_cast<uintptr_t>(entry.start_addr);
        }
    }
    return total_mem;
}
std::string meminfo_print_calc_total_mem(const std::vector<mem_map_entry_t>& mem_map){
    uint64_t total_mem = meminfo_calc_total_mem(mem_map);
    if(total_mem>=1024*1024*1024){
        double gb = static_cast<double>(total_mem) / (1024.0 * 1024.0 * 1024.0);
        return std::to_string(gb) + " GB";
    }
    else if(total_mem>=1024*1024){
        double mb = static_cast<double>(total_mem) / (1024.0 * 1024.0);
        return std::to_string(mb) + " MB";
    }
    else if(total_mem>=1024){
        double kb = static_cast<double>(total_mem) / 1024.0;
        return std::to_string(kb) + " KB";
    }
    return std::to_string(total_mem) + " B";
}
std::string meminfo_to_string(const std::vector<mem_map_entry_t>& mem_map,int step_size){
    std::ostringstream ss;
    int step = 0;
    for(auto& entry:mem_map){
        ss << entry.pathname << " " << reinterpret_cast<uintptr_t>(entry.end_addr) - reinterpret_cast<uintptr_t>(entry.start_addr) ;
        if(++step%step_size==0)
            ss << std::endl;
        else
            ss << " ";
    }
    return ss.str();
}


float meminfo_sys_mem_usage(float scale){
    std::ifstream maps_file("/proc/meminfo");
    std::string line;
    uint64_t total_mem = 0;
    uint64_t available_mem = 0;
    while (std::getline(maps_file, line)) {
        if (line.find("MemTotal:") != std::string::npos) {
            sscanf(line.c_str(), "MemTotal: %lu kB", &total_mem);
        }
        else if (line.find("MemAvailable:") != std::string::npos) {
            sscanf(line.c_str(), "MemAvailable: %lu kB", &available_mem);
        }
    }
    return (total_mem - available_mem) *scale/ total_mem;
}

uint64_t meminfo_sys_mem_usage_kb(){
    std::ifstream maps_file("/proc/meminfo");
    std::string line;
    uint64_t total_mem = 0;
    uint64_t available_mem = 0;
    while (std::getline(maps_file, line)) {
        if (line.find("MemTotal:") != std::string::npos) {
            sscanf(line.c_str(), "MemTotal: %lu kB", &total_mem);
        }
        else if (line.find("MemAvailable:") != std::string::npos) {
            sscanf(line.c_str(), "MemAvailable: %lu kB", &available_mem);
        }
    }
    return total_mem - available_mem;
}

uint64_t meminfo_gpu_mem_usage_kb(){
    std::ifstream maps_file("/proc/meminfo");
    std::string line;
    uint64_t gpu_totalused = 0;
    while (std::getline(maps_file, line)) {
        if (line.find("GPUTotalUsed:") != std::string::npos) {
            sscanf(line.c_str(), "GPUTotalUsed: %lu kB", &gpu_totalused);
            break;
        }
    }
    return gpu_totalused;
}

std::string meminfo_gpu_mem_usage(){
    uint64_t gpu_totalused = meminfo_gpu_mem_usage_kb();

    if(gpu_totalused){
        float gpu_usage = gpu_totalused / 1024.0f/ 1024.0f;
        return std::to_string(gpu_usage) + " GB";
    }
    return "N/A";
}