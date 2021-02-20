#include <jni.h>

#ifndef NELEM
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

#define YUVBUFFER_IN_JAVA_OBJ_NAME   "mVideobuffer"

static const char *const kClassPathName = "com/cvte/excshare/x264/JniX264";

extern "C"
{
    static void JNICALL
    Java_com_cvte_excshare_x264_JniX264_initX264Encode(JNIEnv *env, jobject thiz);

    static void JNICALL
    Java_com_cvte_excshare_x264_JniX264_setParameter(JNIEnv *env, jobject thiz, jint width, jint height,
            jint fps, jint bite);

    static void JNICALL
    Java_com_cvte_excshare_x264_JniX264_encoderH264(JNIEnv *env, jobject thiz, jbyteArray srcData, jint lenght, jlong time);

    static void JNICALL
    Java_com_cvte_excshare_x264_JniX264_CloseX264Encode(JNIEnv *env, jobject thiz);
}