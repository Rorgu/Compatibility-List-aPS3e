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
#if 0
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
#endif
        default:
            aps3e_log.error("load_config_entry fail %s",tag_str.c_str());
            LOGE("load_config_entry fail %s",tag_str.c_str());
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
#if 0
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
#endif
        default:
            aps3e_log.error("save_config_entry fail %s",tag_str.c_str());
            LOGE("save_config_entry fail %s",tag_str.c_str());
            break;
    }

    env->ReleaseStringUTFChars(val,val_cstr);
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

auto gen_key=[](const std::string& name)->std::string{
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
                out<<"<SeekBarPreference app:title=\"@string/emulator_settings_"<<parent_name_l<<"_"<<key<<"\" \n";
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

    std::ostringstream out;
    out<<R"(
<?xml version="1.0" encoding="utf-8"?>
<PreferenceScreen
    xmlns:android="http://schemas.android.com/apk/res/android"
    xmlns:app="http://schemas.android.com/apk/res-auto">
    )";

    for(auto n:g_cfg.get_nodes()){
        const std::string& name=n->get_name();
        if(name=="Core"||name=="Video"||name=="Audio"||name=="Input/Output"||name=="System"
           ||name=="Net"||name=="Savestate"||name=="Miscellaneous"){
            out<<" <PreferenceScreen app:title=\"@string/emulator_settings_"<<gen_key(name)<<"\" \n";
            out<<"app:key=\""<<name<<"\" >\n";
            for(auto n2:reinterpret_cast<cfg::node*>(n)->get_nodes()){

                //Video下的3个子项
                if(n2->get_type()==cfg::type::node){
                    out<<"<PreferenceScreen app:title=\"@string/emulator_settings_"<<gen_key(name)<<"_"<<gen_key(n2->get_name())<<"\" \n";
                    out<<"app:key=\""<<name+"|"+n2->get_name()<<"\" >\n";
                    for(auto n3:reinterpret_cast<cfg::node*>(n2)->get_nodes()) {
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
        if(name=="Core"||name=="Video"||name=="Audio"||name=="Input/Output"||name=="System"
           ||name=="Net"||name=="Savestate"||name=="Miscellaneous"){

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
        if(name=="Core"||name=="Video"||name=="Audio"||name=="Input/Output"||name=="System"
           ||name=="Net"||name=="Savestate"||name=="Miscellaneous"){

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
            if(name=="Core"||name=="Video"||name=="Audio"||name=="Input/Output"||name=="System"
               ||name=="Net"||name=="Savestate"||name=="Miscellaneous"){

                for(auto n2:reinterpret_cast<cfg::node*>(n)->get_nodes()){

                    //Video下的3个子项
                    if(n2->get_type()==cfg::type::node){
                        for(auto n3:reinterpret_cast<cfg::node*>(n2)->get_nodes()) {
                            out << gen_one_key_string(name+"|"+n2->get_name(), n3,test_ty);
                        }
                    }
                    else{
                        out<< gen_one_key_string(name,n2,test_ty);
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
            if(name=="Core"||name=="Video"||name=="Audio"||name=="Input/Output"||name=="System"
               ||name=="Net"||name=="Savestate"||name=="Miscellaneous"){

                out<<"\""<<name<<"\",\n";

                for(auto n2:reinterpret_cast<cfg::node*>(n)->get_nodes()){

                    //Video下的3个子项
                    if(n2->get_type()==cfg::type::node){
                        out<<"\""<<name+"|"+n2->get_name()<<"\",\n";
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
