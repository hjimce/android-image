#include <jni.h>
#include <string>
#include "face_detection/CDlibFace.h"
#include <vector>

extern "C" jintArray  Java_image_imageapp_MainActivity_stringFromJNI(JNIEnv *env, jobject /* this */
        ,jintArray image_data,jint image_height,jint image_widht)
{
    //const char* cha = env->GetStringUTFChars(imagepath, nullptr);
    // char* 转 string
   // std::string image_str(cha);
   // if(cha == NULL) {
        //return NULL;
   // }
    //jstring rtstr = env->NewStringUTF(image_str.c_str());//env->NewStringUTF(hello.c_str());
    // return rtstr;
    std::vector<int>image_datacpp(image_height*image_widht);
    jsize len = env->GetArrayLength(image_data);
    jint *body = env->GetIntArrayElements(image_data, 0);
    for (jsize i=0;i<len;i++){
        image_datacpp[i]=(int)body[i];
    }


    CDlibFace face;
    std::vector<int>rect=face.getfacerect(image_datacpp,image_height,image_widht);
    jintArray result =env->NewIntArray(4);
    env->SetIntArrayRegion(result, 0, 4, &rect[0]);

    return result;

}
