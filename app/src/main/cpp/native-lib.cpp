#include <jni.h>
#include <string>

extern "C" jstring Java_image_imageapp_MainActivity_stringFromJNI(JNIEnv *env, jobject /* this */,jstring imagepath)
{
    const char* cha = env->GetStringUTFChars(imagepath, nullptr);
    if(cha == NULL) {
        return NULL;
    }
    // char* 转 string
    std::string str = cha;
    std::string newstring="黄锦池"+str;

    jstring rtstr = env->NewStringUTF(newstring.c_str());//env->NewStringUTF(hello.c_str());
    return rtstr;
}
