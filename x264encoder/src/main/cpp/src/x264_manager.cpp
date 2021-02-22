#include "x264_manager.h"

#include "x264_encoder.h"

X264Encode *_x264Encoder;
H264DataCallBack h264callbackFunc;

void InitX264Encode(H264DataCallBack h264callback) {
    if (NULL != _x264Encoder) {
        ReleaseX264Encode();
    }
    _x264Encoder = new X264Encode();
    h264callbackFunc = h264callback;
    LOGI("/**********************InitX264Encode*************************/");
}

void SetParameter(int width, int height, int fps, int bite) {
    _x264Encoder->SetParameter(width, height, fps, bite);
}

void EncoderH264(void *pdata, unsigned int datalen, long long time) {
    if (_x264Encoder == NULL) {
        LOGI("_x264Encoder is Null");
        return;
    }
    int i = 0;
    char *bufdata = NULL;
    int buflen = -1;
    int isKeyFrame;
    //LOGI("/**********************PostOriginalSlice************************%d",datalen);
    _x264Encoder->StartEncoder((uint8_t *) pdata, *&bufdata, *&buflen, *&isKeyFrame);
    if (buflen != -1) {
        if (NULL != h264callbackFunc) {
            h264callbackFunc(bufdata, buflen);
        }
        if (bufdata) {
            delete[] bufdata;
        }
    }
}

void ReleaseX264Encode() {
    if (_x264Encoder) {
        _x264Encoder->ReleaseEncoder();
        delete _x264Encoder;
        _x264Encoder = NULL;
    }
}