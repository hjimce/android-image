#include <string.h>
#include <jni.h>

jstring Java_com_example_hellojni_HelloJni_stringFromJNI( JNIEnv* env, jobject thiz )
{
    char* a="ddd";
    return (*env)->NewStringUTF(env, a);
}
