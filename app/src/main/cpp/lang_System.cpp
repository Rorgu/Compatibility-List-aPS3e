//
// Created by aenu on 2025/4/10.
//
#include <stdlib.h>
#include <jni.h>

extern "C" JNIEXPORT void JNICALL  Java_aenu_lang_System_setenv(JNIEnv*  env,jclass cls,jstring jname,jstring jvalue,jboolean overwrite){
    const char* name = env->GetStringUTFChars(jname,NULL);
    const char* value = env->GetStringUTFChars(jvalue,NULL);
    setenv(name,value,overwrite);
    env->ReleaseStringUTFChars(jname,name);
    env->ReleaseStringUTFChars(jvalue,value);
}