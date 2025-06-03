//
// Created by aenu on 2025/5/11.
//
#include "iso.h"

std::unique_ptr<iso_fs> iso_fs::from_fd(int fd) {
    std::unique_ptr<iso_fs> _iso_fs=std::make_unique<iso_fs>();
    _iso_fs->fd = fd;
    return _iso_fs;
}

bool iso_fs::load(){

    off_t pos=0x8000;

    std::optional<VolumeDescriptor> primary_vd;
    std::optional<VolumeDescriptor> supplementary_vd;

    for(;;){
        lseek(fd, pos, SEEK_SET);

        VolumeDescriptor vd;
        if(::read(fd, &vd, sizeof(vd))!=sizeof(vd)){
            return false;
        }

        if(memcmp(vd.identifier, "CD001", 5)!=0)
            return false;

        if(vd.version>2)
            return false;

        if(vd.type_code==255)
            break;

        pos+=0x800;

        if(vd.type_code==0){
            //Boot Record
            continue;
        }

        if(vd.type_code==1){
            primary_vd=vd;
            continue;
        }
        if(vd.type_code==2){
            supplementary_vd=vd;
            continue;
        }
    }

    if(supplementary_vd){
        parse<2>(*supplementary_vd);
        return true;
    }
    else if(primary_vd){
        parse<1>(*primary_vd);
        return true;
    }
    else
        return false;
}


bool iso_fs::exists(const std::string& path){
    if(path==":")
        return true;
    return files.find(path)!=files.end();
}

std::vector<uint8_t> iso_fs::get_data_tiny(const std::string& path){
    auto it = files.find(path);
    if (it != files.end()) {
        const entry_t& entry = it->second;
#if 0
        if(entry.blocks.size()!=1){
            //FIXME 应该抛出异常,get_data_tiny应该只用于获取轻量级文件
            return {};
        }
        lseek(fd, entry.blocks[0].offset, SEEK_SET);
        std::vector<uint8_t> buffer(entry.blocks[0].size);
#else
        lseek(fd, entry.offset, SEEK_SET);
        std::vector<uint8_t> buffer(entry.size);
#endif
        ::read(fd, buffer.data(), buffer.size());
        return buffer;
    }
    return {};
}

std::vector<iso_fs::entry_t>& iso_fs::list_dir(const std::string& path){
    return tree[path];
}

iso_fs::~iso_fs()
{
    close(fd);
}

template<const int VOLUME_TYPE>
void iso_fs::parse(VolumeDescriptor& vd){
    read_dir<VOLUME_TYPE>(vd.root_directory_record, ":");//std::nullopt);
}

template<const int VOLUME_TYPE>
void iso_fs::read_dir(RootDirectoryRecord& dir_record, std::optional<std::string> path) {
    if(dir_record.extended_attribute_length!=0){
        //return;
    }
    std::vector<uint8_t> buffer(dir_record.data_length.ne());
    lseek(fd, dir_record.extent_location.ne()*2048, SEEK_SET);
    if(::read(fd, buffer.data(), buffer.size())!=buffer.size())
        return;

    for(size_t offset=0;;){
        if(offset>=buffer.size())
            break;
        size_t length=buffer[offset];
        if(length==0) {
            offset&=~2047;
            offset+=2048;
            continue;
        }
        RootDirectoryRecord record;
        memcpy(&record, buffer.data()+offset, sizeof(record));
        if((record.file_identifier_length&1)==1){
            offset+=length;
            continue;
        }

        auto read_file_identifier_ascii=[&]()->std::string {
            return std::string(reinterpret_cast<char*>(buffer.data() + offset + sizeof(RootDirectoryRecord)), record.file_identifier_length);
        };
        //2 bytes
       auto read_file_identifier_unicode = [&]()->std::string {
    std::string result;
    char* file_identifier_base_ptr = reinterpret_cast<char*>(buffer.data() + offset + sizeof(RootDirectoryRecord));
    for (size_t i = 0; i + 1 < record.file_identifier_length; i += 2) {
        uint16_t code_unit = (static_cast<uint8_t>(file_identifier_base_ptr[i+0]) << 8) |
                             static_cast<uint8_t>(file_identifier_base_ptr[i + 1]);

        if (code_unit <= 0x7F) {
            result += static_cast<char>(code_unit);
        } else if (code_unit <= 0x7FF) {
            result += static_cast<char>(0xC0 | ((code_unit >> 6) & 0x1F));
            result += static_cast<char>(0x80 | (code_unit & 0x3F));
        } else {
            result += static_cast<char>(0xE0 | ((code_unit >> 12) & 0x0F));
            result += static_cast<char>(0x80 | ((code_unit >> 6) & 0x3F));
            result += static_cast<char>(0x80 | (code_unit & 0x3F));
        }
    }
    return result;
};

        std::string file_identifier;
        if constexpr (VOLUME_TYPE==1){
            file_identifier=read_file_identifier_ascii();
        }
        else if constexpr (VOLUME_TYPE==2){
            file_identifier=read_file_identifier_unicode();
        }

        //LOGE("%s", file_identifier.c_str());

        //if(path)
        //    file_identifier=path.value()+"/"+file_identifier;


        if(path.value()==":")
            file_identifier = path.value() + file_identifier;
        else
            file_identifier = path.value() + "/" + file_identifier;

        if(!(record.file_flags&0x2))
            file_identifier=file_identifier.substr(0,file_identifier.size()-2); //;1

#if 0
        if(auto it=files.find(file_identifier);it!=files.end()){
            it->second.blocks.push_back({
                .offset=record.extent_location.ne()*2048,
                .size=record.data_length.ne(),
                .entry_offset=it->second.blocks.back().entry_offset+it->second.blocks.back().size,
                });

        }
        else{
            files[file_identifier]=entry_t{
                    .path=file_identifier,
                    .blocks={{
                        .offset=record.extent_location.ne()*2048ull,
                        .size=record.data_length.ne(),
                        .entry_offset=0,
                        }},
                    .is_dir=!!(record.file_flags&0x2)
            };
        }

        //

        if(auto it=tree.find(path.value());it==tree.end()){
            tree[path.value()]=std::vector<entry_t>();
        }

        auto& dir_entries = tree[path.value()];

        if(auto it = std::find_if(dir_entries.begin(), dir_entries.end(), [&](const entry_t& entry) {
            return entry.path == file_identifier;});it != dir_entries.end())
        {
            *it = files[file_identifier];
        }
        else {
            dir_entries.push_back(files[file_identifier]);
        }
#else

        if(auto it=files.find(file_identifier);it!=files.end()){
            it->second.size+=record.data_length.ne();
        }
        else{
            files[file_identifier]=entry_t{
                .path=file_identifier,
                .offset=record.extent_location.ne()*2048ull,
                .size=record.data_length.ne(),
                .is_dir=!!(record.file_flags&0x2)
            };
        }
        if(auto it=tree.find(path.value());it==tree.end()){
            tree[path.value()]=std::vector<entry_t>();
        }
        auto& dir_entries = tree[path.value()];
        if(auto it = std::find_if(dir_entries.begin(), dir_entries.end(), [&](const entry_t& entry) {
            return entry.path == file_identifier;
        });it  != dir_entries.end()){
            *it = files[file_identifier];
        }
        else {
            dir_entries.push_back(files[file_identifier]);
        }

#endif
        if(record.file_flags&0x2){
            read_dir<VOLUME_TYPE>(record, file_identifier);
        }
        offset+=length;
    }
}