#include <jni.h>

#ifndef NELEM
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif

static const char *const kClassPathName = "com/jokchen/x264/encoder/JniX264";

extern "C"
{
    static void JNICALL
    Java_com_jokchen_x264_encoder_JniX264_initX264Encode(JNIEnv *env, jobject thiz);

    static void JNICALL
    Java_com_jokchen_x264_encoder_JniX264_setParameter(JNIEnv *env, jobject thiz, jint width, jint height,
            jint fps, jint bite);

    static void JNICALL
    Java_com_jokchen_x264_encoder_JniX264_encoderH264(JNIEnv *env, jobject thiz, jbyteArray srcData, jint lenght, jlong time);

    static void JNICALL
    Java_com_jokchen_x264_encoder_JniX264_CloseX264Encode(JNIEnv *env, jobject thiz);
}