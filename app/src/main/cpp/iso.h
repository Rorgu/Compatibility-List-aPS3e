//
// Created by aenu on 2025/5/17.
//

#ifndef APS3E_ISO_H
#define APS3E_ISO_H

#include <stdint.h>
#include <endian.h>
#include <bit>
#include <fcntl.h>
#include <unistd.h>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <cassert>
#include <numeric>

#include "util/logs.hpp"

LOG_CHANNEL(iso_fs_log);

template<typename T>
struct le_be_t {
    T le;
    T be;
    T ne() {
#if __BYTE_ORDER==__LITTLE_ENDIAN
        return le;
#elif  __BYTE_ORDER==__BIG_ENDIAN
        return be;
#else
#error "unknown byte order"
#endif
    }
};

struct path_table_t{
    uint32_t path_table;
    uint32_t ext_path_table;
};

static_assert(sizeof(path_table_t)==8, "sizeof(path_table_t) != 8");

#pragma pack(push, 1)
struct RootDirectoryRecord {
    uint8_t length;
    uint8_t extended_attribute_length;
    le_be_t<uint32_t> extent_location;
    le_be_t<uint32_t> data_length;
    uint8_t recording_date[7];
    uint8_t file_flags;
    uint8_t file_unit_size;
    uint8_t interleave_gap_size;
    le_be_t<uint16_t> volume_sequence_number;
    uint8_t file_identifier_length;
};

static_assert(sizeof(RootDirectoryRecord)==33, "sizeof(RootDirectoryRecord) != 33");

struct VolumeDescriptor {
    uint8_t type_code;
    char identifier[5];
    uint8_t version;
    uint8_t unused[1];
    char system_identifier[32];
    char volume_identifier[32];
    uint8_t unused2[8];
    le_be_t<uint32_t> volume_space_size;
    uint8_t unused3[32];
    le_be_t<uint16_t> volume_set_size;
    le_be_t<uint16_t> volume_sequence_number;
    le_be_t<uint16_t> logical_block_size;
    le_be_t<uint32_t> path_table_size;
    le_be_t<path_table_t> path_table_data;
    RootDirectoryRecord root_directory_record;
    uint8_t unused4[1];
    char volume_set_identifier[128];
    char publisher_identifier[128];
    char data_preparer_identifier[128];
    char application_identifier[128];
    char copyright_file_identifier[37];
    char abstract_file_identifier[37];
    char bibliographic_file_identifier[37];
    char volume_creation_date[17];
    char volume_modification_date[17];
    char volume_expiration_date[17];
    char volume_effective_date[17];
    uint8_t file_structure_version;
    uint8_t unused5[1];
    uint8_t application_data[512];
    uint8_t reserved[653];
};

static_assert(sizeof(VolumeDescriptor)==2048, "sizeof(VolumeDescriptor) != 2048");

struct iso_fs{
    static std::unique_ptr<iso_fs> from_fd(int fd);

    iso_fs()=default;
    ~iso_fs();

    iso_fs(const iso_fs&) = delete;
    iso_fs(iso_fs&&)      = delete;

    bool load();

    bool exists(const std::string& path);

    std::vector<uint8_t> get_data_tiny(const std::string& path);

#if 0
    struct block_t {
        uint64_t offset;
        uint64_t size;

        uint64_t entry_offset;
    };

    struct entry_t {
        std::string path;
        std::vector<block_t> blocks;
        bool  is_dir;

        uint64_t size(){return std::accumulate(blocks.begin(),blocks.end(),0ull,[](uint64_t sum,const block_t& b){return sum+b.size;});}
    };
#else

    struct entry_t {
        std::string path;
        uint64_t offset;
        uint64_t size;
        bool  is_dir;
    };
#endif
    entry_t get_entry(const std::string& path){
        if(files.find(path)==files.end()) {
            iso_fs_log.warning("iso_fs::get_entry(%s) not found",path);
            return {};
        }else {
            iso_fs_log.warning("iso_fs::get_entry(%s) found",path);
            return files[path];
        }
    }

    std::vector<entry_t>& list_dir(const std::string& path);

    off_t seek(uint64_t offset) const {return lseek(fd, offset, SEEK_SET);}
    ssize_t read(uint8_t* buffer, uint64_t size){return ::read(fd, buffer, size);}

#if 0
    void save(int fp,const std::string& path){
        auto entry=get_entry(path);
        uint8_t buffer[1024*1024];
        for(auto& block:entry.blocks){
            uint64_t  offset=block.offset;
            uint64_t remaining=block.size;
            uint64_t buffer_size=std::min(remaining,sizeof(buffer));
            while(remaining>0){
                seek(offset);
                read(buffer,buffer_size);
                write(fp,buffer,buffer_size);
                remaining-=buffer_size;
                offset+=buffer_size;
                buffer_size=std::min(remaining,sizeof(buffer));
            }
        }
    }
#endif
private:

    template<const int VOLUME_TYPE>
    void parse(VolumeDescriptor& vd);

    template<const int VOLUME_TYPE>
    void read_dir(RootDirectoryRecord& dir_record,std::optional<std::string> path);

    int fd;
    std::unordered_map <std::string, entry_t> files;
    std::unordered_map <std::string, std::vector<entry_t>> tree;
};

#pragma pack(pop)

#endif //APS3E_ISO_H
