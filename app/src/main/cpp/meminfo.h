//
// Created by aenu on 2025/6/23.
// SPDX-License-Identifier: WTFPL
//

#ifndef APS3E_MEMINFO_H
#define APS3E_MEMINFO_H
#include <vector>
#include <cstdint>

struct mem_map_entry_t {
    void* start_addr;
    void* end_addr;
    std::string permissions;
    size_t offset;
    int dev_major;
    int dev_minor;
    unsigned long inode;
    std::string pathname;
};

void meminfo_init();
std::vector<mem_map_entry_t> meminfo_update();
uint64_t meminfo_calc_total_mem(const std::vector<mem_map_entry_t>& mem_map);
std::string meminfo_print_calc_total_mem(const std::vector<mem_map_entry_t>& mem_map);
std::string meminfo_to_string(const std::vector<mem_map_entry_t>& mem_map,int step_size=1);

float meminfo_sys_mem_usage(float scale=1.0);

#endif //APS3E_MEMINFO_H
