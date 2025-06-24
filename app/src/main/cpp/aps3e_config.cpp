//
// Created by aenu on 2025/6/2.
// SPDX-License-Identifier: WTFPL
//

static_assert(sizeof(YAML::Node*)==8,"");

static YAML::Node* open_config_file(JNIEnv* env,jobject self,jstring config_path){
    jboolean is_copy=false;
    const char* path=env->GetStringUTFChars(config_path,&is_copy);

    YAML::Node* config_node = new YAML::Node(YAML::LoadFile(path));
    env->ReleaseStringUTFChars(config_path,path);
    return config_node;
}

static jstring load_config_entry(JNIEnv* env,jobject self,YAML::Node* config_node,jstring tag){
    jboolean is_copy=false;
    const char* tag_cstr=env->GetStringUTFChars(tag,&is_copy);
    std::string  tag_str(tag_cstr);
    env->ReleaseStringUTFChars(tag,tag_cstr);
    size_t pos=tag_str.find('|');
    std::string  parent=tag_str.substr(0,pos);
    std::string  child=tag_str.substr(pos+1);

    switch(std::count(child.begin(),child.end(),'|')){
        case 0: {
            YAML::Node node=(*config_node)[parent][child];
            if(node.IsDefined())
                return env->NewStringUTF(node.as<std::string>().c_str());
        }
            break;
        case 1:{
            pos=child.find('|');
            std::string child2=child.substr(pos+1);
            child=child.substr(0,pos);

            YAML::Node node=(*config_node)[parent][child][child2];
            if(node.IsDefined())
                return env->NewStringUTF(node.as<std::string>().c_str());
        }
            break;

            case 2:{

            pos=child.find('|');
            std::string child2=child.substr(pos+1);
            child=child.substr(0,pos);
            pos=child2.find('|');
            std::string child3=child2.substr(pos+1);
            child2=child2.substr(0,pos);

            YAML::Node node=(*config_node)[parent][child][child2][child3];
            if(node.IsDefined())
                return env->NewStringUTF(node.as<std::string>().c_str());
        }
            break;

        default:
            aps3e_log.error("load_config_entry fail %s,level too deep",tag_str.c_str());
            LOGE("load_config_entry fail %s,level too deep",tag_str.c_str());
            break;
    }
    return NULL;
}


static jobjectArray load_config_entry_ty_arr(JNIEnv* env,jobject self,YAML::Node* config_node,jstring tag){
    jboolean is_copy=false;
    const char* tag_cstr=env->GetStringUTFChars(tag,&is_copy);
    std::string  tag_str(tag_cstr);
    env->ReleaseStringUTFChars(tag,tag_cstr);
    size_t pos=tag_str.find('|');
    std::string  parent=tag_str.substr(0,pos);
    std::string  child=tag_str.substr(pos+1);

    switch(std::count(child.begin(),child.end(),'|')){
        case 0: {
            YAML::Node node=(*config_node)[parent][child];
            if(node.IsDefined()) {
                std::vector<std::string> v=node.as<std::vector<std::string>>();
                jobjectArray arr=env->NewObjectArray(v.size(),env->FindClass("java/lang/String"),NULL);
                for(size_t i=0;i<v.size();i++){
                    env->SetObjectArrayElement(arr,i,env->NewStringUTF(v[i].c_str()));
                }
                return arr;
            }
        }

            break;

        default:
            aps3e_log.error("load_config_entry fail %s,level too deep",tag_str.c_str());
            LOGE("load_config_entry fail %s,level too deep",tag_str.c_str());
            break;
    }
    return NULL;
}

static void save_config_entry(JNIEnv* env,jobject self,YAML::Node* config_node,jstring tag,jstring val){

    jboolean is_copy=false;
    const char* tag_cstr=env->GetStringUTFChars(tag,&is_copy);
    std::string  tag_str(tag_cstr);
    env->ReleaseStringUTFChars(tag,tag_cstr);
    size_t pos=tag_str.find('|');
    std::string  parent=tag_str.substr(0,pos);
    std::string  child=tag_str.substr(pos+1);
    const char* val_cstr=env->GetStringUTFChars(val,&is_copy);

    switch(std::count(child.begin(),child.end(),'|')){
        case 0: {
            (*config_node)[parent][child] = std::string(val_cstr);
        }
            break;
        case 1:{
            pos=child.find('|');
            std::string child2=child.substr(pos+1);
            child=child.substr(0,pos);
            (*config_node)[parent][child][child2]=std::string(val_cstr);
        }
            break;

            case 2:{

            pos=child.find('|');
            std::string child2=child.substr(pos+1);
            child=child.substr(0,pos);
            pos=child2.find('|');
            std::string child3=child2.substr(pos+1);
            child2=child2.substr(0,pos);
            (*config_node)[parent][child][child2][child3]=std::string(val_cstr);
        }
            break;

        default:
            aps3e_log.error("save_config_entry fail %s,level too deep",tag_str.c_str());
            LOGE("save_config_entry fail %s,level too deep",tag_str.c_str());
            break;
    }

    env->ReleaseStringUTFChars(val,val_cstr);
}


static void save_config_entry_ty_arr(JNIEnv* env,jobject self,YAML::Node* config_node,jstring tag,jobjectArray val) {

    jboolean is_copy = false;
    const char *tag_cstr = env->GetStringUTFChars(tag, &is_copy);
    std::string tag_str(tag_cstr);
    env->ReleaseStringUTFChars(tag, tag_cstr);
    size_t pos = tag_str.find('|');
    std::string parent = tag_str.substr(0, pos);
    std::string child = tag_str.substr(pos + 1);

    switch (std::count(child.begin(), child.end(), '|')) {
        case 0: {
            std::vector<std::string> v;
            for (int i = 0; i < env->GetArrayLength(val); i++) {
                jstring jstr = (jstring) env->GetObjectArrayElement(val, i);
                const char *str = env->GetStringUTFChars(jstr, &is_copy);
                v.push_back(std::string(str));
                env->ReleaseStringUTFChars(jstr, str);
            }
            (*config_node)[parent][child] = v;
        }
            break;
    }
}
static void close_config_file(JNIEnv* env,jobject self,YAML::Node* config_node,jstring config_path){
    YAML::Emitter out;
    out << *config_node;
    jboolean is_copy=false;
    const char* path=env->GetStringUTFChars(config_path,&is_copy);
    std::ofstream fout(path);
    fout << out.c_str();
    fout.close();
    env->ReleaseStringUTFChars(config_path,path);
    delete config_node;
}

static auto gen_key=[](const std::string& name)->std::string{
    std::string k=name;
    if(size_t p=k.find("(");p!=std::string::npos){
        k=k.substr(0,p);
    }

    //replace(' ','_');
    //replace('/','_')
    for(auto& c:k){
        if(c==' '){
            c='_';
        }
        else if(c=='/'){
            c='_';
        }
        else if(c=='|'){
            c='_';
        }
    }

    //replace("-","");
    size_t p=k.find("-");
    while(p!=std::string::npos){
        k.erase(p,1);
        p=k.find("-");
    }

    //移除尾部的_
    while (k.at(k.size()-1)=='_'){
        k.erase(k.size()-1,1);
    }

    std::transform(k.begin(),k.end(),k.begin(),[](char c){
        return std::tolower(c);
    });

    return k;
};

static const std::string gen_skips[]={
        "Core|SPU LLVM Lower Bound",
        "Core|SPU LLVM Upper Bound",
        "Audio|Audio Device",
        "Audio|Microphone Devices",
        "Input/Output|Camera ID",
        "Input/Output|Emulated Midi devices",
        "System|System Name",
        "System|PSID high",
        "System|PSID low",
        "System|HDD Model Name",
        "System|HDD Serial Number",
        "Miscellaneous|GDB Server",
        "Miscellaneous|Window Title Format",

        "Video|Force Convert Texture"

        "Video|Performance Overlay|Font",
        "Video|Performance Overlay|Body Color (hex)",
        "Video|Performance Overlay|Body Background (hex)",
        "Video|Performance Overlay|Title Color (hex)",
        "Video|Performance Overlay|Title Background (hex)",
};

static bool gen_is_parent(const std::string& parent_name){
    if(parent_name=="Core"||parent_name=="Video"||parent_name=="Audio"||parent_name=="Input/Output"
    ||parent_name=="System"||parent_name=="Savestate"||parent_name=="Miscellaneous")
        return true;
    return false;
}
static jstring generate_config_xml(JNIEnv* env,jobject self){

    auto gen_one_preference=[&](const std::string parent_name,cfg::_base* node)->std::string{
        std::stringstream out;

        std::string parent_name_l=gen_key(parent_name);

        const std::string& name=node->get_name();
        const std::string key=gen_key(name);

        switch (node->get_type()) {
            case cfg::type::_bool:
                out<<"<CheckBoxPreference app:title=\"@string/emulator_settings_"<<parent_name_l<<"_"<<key<<"\" \n";
                out<<"app:key=\""<<parent_name<<"|"<<name<<"\" />\n";
                break;
            case cfg::type::_int:
            case cfg::type::uint:
                out<<"<aenu.preference.SeekbarPreference app:title=\"@string/emulator_settings_"<<parent_name_l<<"_"<<key<<"\" \n";
                out<<"app:min=\""<<node->get_min()<<"\"\n";
                if(node->get_max()!=-1)
                    out<<"android:max=\""<<node->get_max()<<"\"\n";
                else
                    out<<"android:max=\"0x7fffffff\"\n";
                out<<"app:showSeekBarValue=\"true\"\n";
                out<<"app:key=\""<<parent_name<<"|"<<name<<"\" />\n";
                break;

            case cfg::type::_enum:
                out<<"<ListPreference app:title=\"@string/emulator_settings_"<<parent_name_l<<"_"<<key<<"\" \n";
                out<<"app:entries=\""<<"@array/"<<parent_name_l<<"_"<<key<<"_entries\"\n";
                out<<"app:entryValues=\""<<"@array/"<<parent_name_l<<"_"<<key<<"_values\"\n";
                out<<"app:key=\""<<parent_name<<"|"<<name<<"\" />\n";
                break;
            default:

                out<<"<PreferenceScreen app:title=\"@string/emulator_settings_"<<parent_name_l<<"_"<<key<<"\"\n";
                out<<"app:key=\""<<parent_name<<"|"<<name<<"\" />\n";
                break;
        }
        return out.str();
    };

    auto gen_vk_debug_preferences=[&](const std::string parent_name,cfg::_base* node)->std::string {
        std::stringstream out;

        std::string parent_name_l=gen_key(parent_name);
        const std::string &name = node->get_name();
        const std::string key = gen_key(name);

        if(node->get_type()==cfg::type::node){
            out<<"<PreferenceScreen app:title=\"@string/emulator_settings_"<<parent_name_l<<"_"<<key<<"\" \n";
            out<<"app:key=\""<<parent_name<<"|"<<name<<"\" >\n";
            std::string node_key=parent_name+"|"+name;
            for(auto n3:reinterpret_cast<cfg::node*>(node)->get_nodes()) {
                if(n3->get_type()!=cfg::type::_bool){
                    LOGE("unexpected type");
                    continue;
                }
                out<<"<CheckBoxPreference app:title=\""<<n3->get_name()<<"\" \n";
                out<<"app:key=\""<<node_key<<"|"<<n3->get_name()<<"\" />\n";
            }
            out<<"</PreferenceScreen>\n";
        }

        return out.str();
    };

    std::ostringstream out;
    out<<R"(
<?xml version="1.0" encoding="utf-8"?>
<PreferenceScreen
    xmlns:android="http://schemas.android.com/apk/res/android"
    xmlns:app="http://schemas.android.com/apk/res-auto">
    )";

    for(auto n:g_cfg.get_nodes()){
        const std::string& name=n->get_name();
        if(gen_is_parent(name)){
            out<<" <PreferenceScreen app:title=\"@string/emulator_settings_"<<gen_key(name)<<"\" \n";
            out<<"app:key=\""<<name<<"\" >\n";

            for(auto n2:reinterpret_cast<cfg::node*>(n)->get_nodes()){

                if(std::find(std::begin(gen_skips),std::end(gen_skips),name+"|"+n2->get_name())!=std::end(gen_skips))
                    continue;

                //Video下的3个子项
                if(n2->get_type()==cfg::type::node){
                    out<<"<PreferenceScreen app:title=\"@string/emulator_settings_"<<gen_key(name)<<"_"<<gen_key(n2->get_name())<<"\" \n";
                    out<<"app:key=\""<<name+"|"+n2->get_name()<<"\" >\n";
                    for(auto n3:reinterpret_cast<cfg::node*>(n2)->get_nodes()) {
                        if(std::find(std::begin(gen_skips),std::end(gen_skips),name+"|"+n2->get_name()+"|"+n3->get_name())!=std::end(gen_skips))
                            continue;
                        //
                        if(n3->get_type()==cfg::type::node){
                            out << "\n" <<gen_vk_debug_preferences(name+"|"+n2->get_name(),n3)<< "\n";
                            continue;
                        }

                        out << "\n" << gen_one_preference(name+"|"+n2->get_name(), n3) << "\n";
                    }
                    out<<"</PreferenceScreen>\n";
                }
                else{
                    out<<"\n"<< gen_one_preference(name,n2)<<"\n";
                }
            }
            out<<"</PreferenceScreen>\n";
        }
    }

    out<<"</PreferenceScreen>\n";

    return env->NewStringUTF(out.str().c_str());
}

//public native String generate_strings_xml();
static jstring generate_strings_xml(JNIEnv* env,jobject self){

    auto gen_one_string=[&](const std::string parent_name,cfg::_base* node)->std::string{
        std::stringstream out;

        std::string parent_name_l=gen_key(parent_name);

        const std::string& name=node->get_name();
        const std::string key=gen_key(name);

        out<<"<string name=\"emulator_settings_"<<parent_name_l<<"_"<<key<<"\">"<<name<<"</string>\n";
        if(node->get_type()==cfg::type::_enum){
            out<<"<string-array name=\""<<parent_name_l<<"_"<<key<<"_entries\">\n";
            std::vector<std::string> list=node->to_list();
            for(const auto& e:list){
                out<<"<item>"<<e<<"</item>\n";
            }
            out<<"</string-array>\n";
        }

        return out.str();
    };

    std::ostringstream out;

    for(auto n:g_cfg.get_nodes()){
        const std::string& name=n->get_name();
        if(gen_is_parent(name)){

            out<<"<string name=\"emulator_settings_"<<gen_key(name)<<"\">"<<name<<"</string>\n";

            for(auto n2:reinterpret_cast<cfg::node*>(n)->get_nodes()){

                //Video下的3个子项
                if(n2->get_type()==cfg::type::node){

                    out<<"<string name=\"emulator_settings_"<<gen_key(name+"|"+n2->get_name())<<"\">"<<n2->get_name()<<"</string>\n";
                    for(auto n3:reinterpret_cast<cfg::node*>(n2)->get_nodes()) {
                        out << gen_one_string(name+"|"+n2->get_name(), n3);
                    }
                }
                else{
                    out<< gen_one_string(name,n2);
                }
            }
        }
    }

    //array
    for(auto n:g_cfg.get_nodes()){
        const std::string& name=n->get_name();
        if(gen_is_parent(name)){

            for(auto n2:reinterpret_cast<cfg::node*>(n)->get_nodes()){

                switch (n2->get_type()) {
                    case cfg::type::_enum: {

                        out << "<string-array name=\"" << gen_key(name + "|" + n2->get_name())
                            << "_values\">\n";
                        std::vector<std::string> list = n2->to_list();
                        for (const auto &e: list) {
                            out << "<item>" << e << "</item>\n";
                        }
                        out << "</string-array>\n";
                        break;
                    }
                        //Video下的3个子项
                    case cfg::type::node:{
                        for(auto n3:reinterpret_cast<cfg::node*>(n2)->get_nodes()) {
                            out << "<string-array name=\""<<gen_key(name+"|"+n2->get_name()+"|"+n3->get_name())<<"_values\">\n";
                            std::vector<std::string> list=n3->to_list();
                            for(const auto& e:list){
                                out<<"<item>"<<e<<"</item>\n";
                            }
                            out<<"</string-array>\n";
                        }
                        break;
                    }
                    default:
                        break;
                }

            }
        }
    }

    return env->NewStringUTF(out.str().c_str());
}

static jstring generate_java_string_arr(JNIEnv* env,jobject self){

    auto gen_one_key_string=[&](const std::string parent_name,cfg::_base* node,cfg::type test_ty)->std::string{
        std::string r="";
        if(node->get_type()==test_ty){
            return r+"\""+parent_name+"|"+node->get_name()+"\",\n";
        }
        return r;
    };

    auto gen_one_key_array=[&](const std::string prefix,cfg::type test_ty)->std::string{

        std::ostringstream out;
        out<<prefix<<"\n";
        for(auto n:g_cfg.get_nodes()){
            const std::string& name=n->get_name();
            if(gen_is_parent(name)){

                for(auto n2:reinterpret_cast<cfg::node*>(n)->get_nodes()){

                    if(std::find(std::begin(gen_skips),std::end(gen_skips),name+"|"+n2->get_name())!=std::end(gen_skips))
                        continue;


                    if(n2->get_type()!=cfg::type::node) {
                        out<< gen_one_key_string(name,n2,test_ty);
                        continue;
                    }

                    //node
                    for(auto n3:reinterpret_cast<cfg::node*>(n2)->get_nodes()) {
                        if(std::find(std::begin(gen_skips),std::end(gen_skips),name+"|"+n2->get_name()+"|"+n3->get_name())!=std::end(gen_skips))
                            continue;
                        if(n3->get_type()!=cfg::type::node){
                            out << gen_one_key_string(name+"|"+n2->get_name(), n3,test_ty);
                            continue;
                        }

                        for(auto n4:reinterpret_cast<cfg::node*>(n3)->get_nodes()){
                            out << gen_one_key_string(name+"|"+n2->get_name()+"|"+n3->get_name(), n4,test_ty);
                        }
                    }
                }
            }
        }
        out<<"};\n";
        return out.str();
    };

    auto gen_node_key_array=[&]()->std::string{

        std::ostringstream out;
        out<<"final String[] NODE_KEYS={\n";
        for(auto n:g_cfg.get_nodes()){
            const std::string& name=n->get_name();
            if(gen_is_parent(name)){

                out<<"\""<<name<<"\",\n";

                for(auto n2:reinterpret_cast<cfg::node*>(n)->get_nodes()){
                    if(std::find(std::begin(gen_skips),std::end(gen_skips),name+"|"+n2->get_name())!=std::end(gen_skips))
                        continue;

                    if(n2->get_type()==cfg::type::node){
                        out<<"\""<<name+"|"+n2->get_name()<<"\",\n";

                        for(auto n3:reinterpret_cast<cfg::node*>(n2)->get_nodes()) {

                            if(std::find(std::begin(gen_skips),std::end(gen_skips),name+"|"+n2->get_name()+"|"+n3->get_name()))
                                continue;

                            if(n3->get_type()==cfg::type::node){
                                out<<"\""<<name+"|"+n2->get_name()+"|"+n3->get_name()<<"\",\n";
                            }
                        }
                    }
                }
            }
        }
        out<<"};\n";
        return out.str();
    };

    std::ostringstream out;

    out<<gen_one_key_array("final String[] BOOL_KEYS={",cfg::type::_bool);
    out<<gen_one_key_array("final String[] INT_KEYS={",cfg::type::_int);
    out<<gen_one_key_array("final String[] INT_KEYS={",cfg::type::uint);
    out<<gen_one_key_array("final String[] STRING_ARR_KEYS={",cfg::type::_enum);

    //
    out<<gen_node_key_array();

    return env->NewStringUTF(out.str().c_str());
}

//public native String[] get_support_llvm_cpu_list();

static jobjectArray j_get_support_llvm_cpu_list(JNIEnv* env,jobject self){
    std::set<std::string> cpu_list=get_processor_name_set();
    int count=cpu_list.size();
    jobjectArray ret=env->NewObjectArray(count,env->FindClass("java/lang/String"),nullptr);
    int n=0;
    for(const std::string& cpu_name:cpu_list){
        env->SetObjectArrayElement(ret,n++,env->NewStringUTF(cpu_name.c_str()));
    }
    return ret;
}

static jobjectArray j_get_vulkan_physical_dev_list(JNIEnv* env,jobject self){

    std::vector<VkPhysicalDeviceProperties> physical_device_prop_list;
    {
        VkApplicationInfo appinfo = {};
        appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appinfo.pNext = nullptr;
        appinfo.pApplicationName = "aps3e-cfg-test";
        appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appinfo.pEngineName = "nul";
        appinfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appinfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo inst_create_info = {};
        inst_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        inst_create_info.pApplicationInfo = &appinfo;

        VkInstance inst;
        if (vkCreateInstance(&inst_create_info, nullptr, &inst)!= VK_SUCCESS) {
            __android_log_print(ANDROID_LOG_FATAL, LOG_TAG,"%s : %d",__func__,__LINE__);
            aps3e_log.fatal("%s : %d",__func__,__LINE__);
        }

        // 获取物理设备数量
        uint32_t physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(inst, &physicalDeviceCount, nullptr);

        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(inst, &physicalDeviceCount, physicalDevices.data());

        for (const auto& physicalDevice : physicalDevices) {
            VkPhysicalDeviceProperties physicalDeviceProperties;
            vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
            physical_device_prop_list.push_back(physicalDeviceProperties);
        }

        vkDestroyInstance(inst, nullptr);
    }
    int count=physical_device_prop_list.size();
    jobjectArray ret=env->NewObjectArray(count,env->FindClass("java/lang/String"),nullptr);

    int n=0;
    for(const VkPhysicalDeviceProperties& prop:physical_device_prop_list){
        env->SetObjectArrayElement(ret,n++,env->NewStringUTF(prop.deviceName));
    }
    return ret;
}
