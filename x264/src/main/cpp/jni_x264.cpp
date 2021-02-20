#include "include/jni_x264.h"
#include "include/x264_encoder.h"
#include <string.h>
#include <stdlib.h>
#include <Macro.h>

#ifndef NELEM
# define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))
#endif
#define YUVBUFFER_IN_JAVA_OBJ_NAME   "mVideobuffer"

static JavaVM *VM;
jfieldID fid_yuvbuffer;


struct JavaEnv {
    JavaEnv() {
        if (VM != NULL && VM->AttachCurrentThread(&env, NULL) == JNI_OK) {
            //LOGI("env atarch");
            istarch = true;
        } else {
            //LOGI("env not atarch");
            istarch = false;
        }
    }

    ~JavaEnv() {
        if (istarch) {
            //LOGI("env detarch");
            //VM->DetachCurrentThread();
        }
    }

    JNIEnv *env;
    bool istarch;
};

struct JavaMethodID {
    const char *name;
    const char *signature;
    jmethodID id;

    jmethodID getMID(JNIEnv *env, jclass clazz) {
        if (id == NULL) {
            //LOGI("getMID,name=%s,sig=%s,class=%p",name,signature,clazz);
            id = env->GetMethodID(clazz, name, signature);
            //LOGI("getMID,id=%d",id);
        }
        return id;
    }
};

JavaMethodID h264datacallback;
jobject ehobj;
jclass jclz;
//************************************************************************************************************************************/
//************************************************************JNI_METHOD**************************************************************/
//************************************************************************************************************************************/

void CALLBACK H264DataCallBackFunc(void *pdata, int datalen) {
//    LOGI("/**********************H264DataCallBackFunc************************%d", datalen);

    h264datacallback.name = "H264DataCallBackFunc";
    h264datacallback.signature = "([BI)V";
    JavaEnv java;
    if (java.istarch) {
        JNIEnv *menv = NULL;
        VM->AttachCurrentThread(&menv, NULL);
        jbyteArray pcmdata = menv->NewByteArray(datalen);
        menv->SetByteArrayRegion(pcmdata, 0, datalen, (jbyte *) pdata);
        java.env->CallVoidMethod(ehobj, h264datacallback.getMID(java.env, jclz), pcmdata, datalen);
    }
}


static void
Java_com_cvte_excshare_x264_JniX264_initX264Encode(JNIEnv *env, jobject thiz) {
    env->GetJavaVM(&VM);
    if (VM != NULL && VM->AttachCurrentThread(&env, NULL) == JNI_OK) {
        ehobj = env->NewGlobalRef(thiz);
        jclz = (jclass) env->NewGlobalRef(env->GetObjectClass(thiz));
//        fid_yuvbuffer = env->GetFieldID(jclz, YUVBUFFER_IN_JAVA_OBJ_NAME,
//                                        "Ljava/nio/ByteBuffer;");
    }

    initX264Encode(H264DataCallBackFunc);
}

static void
Java_com_cvte_excshare_x264_JniX264_setParameter(JNIEnv *env, jobject thiz, jint width, jint height,
                                                 jint fps, jint bite) {
    setParameter(width, height, fps, bite);
}

static void
Java_com_cvte_excshare_x264_JniX264_encoderH264(JNIEnv *env, jobject thiz, jbyteArray srcData,
                                                jint length,
                                                jlong time) {
//    jobject input = (jobject) env->GetObjectField(thiz, fid_yuvbuffer);
//    unsigned char *jb = (unsigned char *) env->GetDirectBufferAddress(input);
    jbyte *bytes = env->GetByteArrayElements(srcData, NULL);
    encoderH264(bytes, length, time);
    env->ReleaseByteArrayElements(srcData, bytes, 0);
}

static void
Java_com_cvte_excshare_x264_JniX264_CloseX264Encode(JNIEnv *env, jobject thiz) {
    releaseX264Encode();
}

//************************************************************************************************************************************/
//************************************************************JNI_LOAL****************************************************************/
//************************************************************************************************************************************/

static JNINativeMethod gMethods[] = {{"initX264Encode",  "()V",     (void *) Java_com_cvte_excshare_x264_JniX264_initX264Encode},
                                     {"setParameter",    "(IIII)V", (void *) Java_com_cvte_excshare_x264_JniX264_setParameter},
                                     {"encoderH264",     "([BIJ)V", (void *) Java_com_cvte_excshare_x264_JniX264_encoderH264},
                                     {"CloseX264Encode", "()V",     (void *) Java_com_cvte_excshare_x264_JniX264_CloseX264Encode}};


int register_Native_Methods(JNIEnv *env) {
    jclass clazz = env->FindClass(kClassPathName);
    if ((env->RegisterNatives(clazz, gMethods, NELEM(gMethods))) > 1) {
        return -1;
    }
    return 0;
}


jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    jint result = -1;

    if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
        //LOG_PRINT("ERROR: GetEnv failed\n");
        goto bail;
    }

    if (register_Native_Methods(env) < 0) {
        goto bail;
    }
    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

    VM = vm;

    bail:
    return result;
}