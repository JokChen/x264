#include "include/Buffer.h"
#include "include/PUX264Encoder.h"
#include "include/Macro.h"
#include "x264/x264.h"
#include <stdio.h>
#include <sys/time.h>

static int32_t getCurrentTime()      //直接调用这个函数就行了，返回值最好是int64_t，long long应该也可以
{
    struct timeval tv;
    gettimeofday(&tv, NULL);    //该函数在sys/time.h头文件中
    return tv.tv_usec;
}


x264Encode::x264Encode() {
    _x264_encoder = NULL;
    _x264_param = new x264_param_t;
    int result = x264_param_default_preset(_x264_param, "ultrafast", "zerolatency");
    _in_pic = new x264_picture_t;
    _out_pic = new x264_picture_t;
    LOGD("****************result = %d", result);
}

void x264Encode::setParameter(int width, int height, int fps, int bite) {
    xfps = fps;
    xheight = height;
    xwidth = width;

    // 单位转化
    bite = bite / (1024 * 8);
    if (bite == 0) {
        bite = 1;
    }
    if (bite > 0 && bite <= 64) {
        bitratelevel = BIT_LOW_LEVEL;
    } else if (bite > 64 && bite <= 128) {
        bitratelevel = BIT_MEDIUM_LEVEL;
    } else if (bite > 128 && bite <= 256) {
        bitratelevel = BIT_STANDARD_LEVEL;
    } else if (bite > 256 && bite <= 512) {
        bitratelevel = BIT_HIGH_LEVEL;
    } else if (bite > 512 && bite <= 1024) {
        bitratelevel = BIT_VERY_HIGH_LEVEL;
    } else {
        bitratelevel = BIT_STANDARD_LEVEL;
    }
    if (bitratelevel == BIT_LOW_LEVEL) {
        _x264_param->rc.f_rf_constant = 32;
    } else if (bitratelevel == BIT_MEDIUM_LEVEL) {
        _x264_param->rc.f_rf_constant = 29;
    } else if (bitratelevel == BIT_STANDARD_LEVEL) {
        _x264_param->rc.f_rf_constant = 26;
    } else if (bitratelevel == BIT_HIGH_LEVEL) {
        _x264_param->rc.f_rf_constant = 24;
    } else if (bitratelevel == BIT_VERY_HIGH_LEVEL) {
        _x264_param->rc.f_rf_constant = 22;
    } else {
        _x264_param->rc.f_rf_constant = 26;
    }

    _x264_param->rc.f_rf_constant_max = 45;
    _x264_param->b_repeat_headers = 1; // 重复SPS/PPS 放到关键帧前面
    _x264_param->rc.i_rc_method = X264_RC_CRF; //参数i_rc_method表示码率控制，CQP(恒定质量)，CRF(恒定码率)，ABR(平均码率)

    _x264_param->i_width = width;
    _x264_param->i_height = height;
    _x264_param->i_frame_total = 0; //* 编码总帧数.不知道用0.

    // 10秒刷一次I帧
    _x264_param->i_fps_num = fps; //* 帧率分子
    _x264_param->i_fps_den = 1; //* 帧率分母
    _x264_param->i_keyint_max = fps * 10;

    _x264_param->i_timebase_den = _x264_param->i_fps_num;
    _x264_param->i_timebase_num = _x264_param->i_fps_den;

    _x264_param->i_cqm_preset = X264_CQM_FLAT;

    _x264_param->analyse.i_me_method = X264_ME_HEX;
    _x264_param->analyse.i_subpel_refine = 2;
    _x264_param->i_frame_reference = 1;
    _x264_param->analyse.b_mixed_references = 0;
    _x264_param->analyse.i_trellis = 0;
    _x264_param->b_sliced_threads = 0;
    _x264_param->i_threads = 4;
    //i_threads = N并行编码的时候如果b_sliced_threads=1那么是并行slice编码，
    //如果b_sliced_threads=0，那么是并行frame编码。并行slice无延时，并行frame有延时

    _x264_param->analyse.b_transform_8x8 = 0;
    _x264_param->b_cabac = 0;
    _x264_param->b_deblocking_filter = 1;
    _x264_param->psz_cqm_file = NULL;
    _x264_param->analyse.i_weighted_pred = X264_WEIGHTP_NONE;
    _x264_param->rc.i_lookahead = 0;
    _x264_param->i_bframe = 0;

    if (_x264_encoder != NULL) {
        x264_encoder_close(_x264_encoder);
        _x264_encoder = NULL;
    }
    _x264_encoder = x264_encoder_open(_x264_param);
    if (_x264_encoder == NULL) {
//            NSLog(@"encoder error");
    }
//    int iMaxFrames = x264_encoder_maximum_delayed_frames(_x264_encoder);

//        NSLog(@"缓存最大帧数：%d",iMaxFrames);

    x264_picture_init(_out_pic);
    x264_picture_alloc(_in_pic, X264_CSP_I420, _x264_param->i_width, _x264_param->i_height);
    _in_pic->img.i_csp = X264_CSP_I420;
    _in_pic->img.i_plane = 3;
}

void x264Encode::startEncoder(uint8_t *dataptr, char *&bufdata, int &buflen, int &isKeyFrame) {

    int width = xheight;
    int height = xwidth;

    picture_buf = dataptr;
#ifdef __ANDROID__
#else

    UInt8 *pUV = picture_buf + width * height;
    UInt8 *tmp = (UInt8 *) malloc(width * height / 2);
    memcpy(tmp, pUV, width * height / 2);

    int fsize = width * height / 4;
    UInt8 *pU = pUV;
    UInt8 *pV = pUV + fsize;
    for (int i = 0; i < fsize; i++) {
        *pU = tmp[i << 1];
        *pV = tmp[(i << 1) + 1];
        pV++;
        pU++;
    }
    free(tmp);
#endif
    _in_pic->img.plane[0] = picture_buf;
    _in_pic->img.plane[1] = picture_buf + width * height;
    _in_pic->img.plane[2] = picture_buf + width * height * 5 / 4;

    _in_pic->i_type = X264_TYPE_AUTO;

    _in_pic->i_qpplus1 = 0;
    _in_pic->param = _x264_param;
    _in_pic->i_pts = 0;
    x264_nal_t *nal;
    int i_nal = 0;
    int Result;

    int32_t ms1;
    int32_t ms2;
    int32_t ms3;
    if (_x264_encoder != NULL) {
//        ms1 = getCurrentTime();
//        LOGD("###################ms1 = %d", ms1);
        Result = x264_encoder_encode(_x264_encoder, &nal, &i_nal, _in_pic, _out_pic);
//        ms2 = getCurrentTime();
//        LOGD("###################ms2 = %d, interval = %d", ms2, ms2 - ms1);
        isKeyFrame = _out_pic->b_keyframe;
        _in_pic->i_pts++;
    }


    if (Result < 0) {
        LOGI("/**********************encoder faild************************");
    } else if (Result == 0) {
//        LOGI("/**********************编码成功,但被缓存了************************");
    } else {
        int bufsize = 0;
        for (int i = 0; i < i_nal; i++) {
            bufsize += nal[i].i_payload;
        }

        char *tempdata = new char[bufsize];
        memset(tempdata, 0, bufsize);
        bufdata = tempdata;
        for (int i = 0; i < i_nal; i++) {
            if (nal[i].p_payload != NULL) {
                memcpy(tempdata, nal[i].p_payload, nal[i].i_payload);
                tempdata += nal[i].i_payload;
            }
        }

//        LOGI("/**********************编码成功,长度为：%d************************", bufsize);
        buflen = bufsize;
//        ms3 = getCurrentTime();
//        LOGD("###################ms3 = %d, interval = %d", ms3, ms3 - ms2);
    }
}


void x264Encode::Flush() {
    x264_picture_t pic_out;
    x264_nal_t *nal;
    int i_nal;
    if (x264_encoder_encode(_x264_encoder, &nal, &i_nal, NULL, &pic_out) < 0) {
    }
}

void x264Encode::releaseEncoder() {
    this->Flush();

//    x264_picture_clean(_in_pic);
//    x264_picture_clean(_out_pic);

    //* 关闭编码器句柄
    if (_x264_encoder != NULL) {
        x264_encoder_close(_x264_encoder);
        _x264_encoder = NULL;
    }

    if (_in_pic != NULL) {
        delete _in_pic;
        _in_pic = NULL;
    }

    if (_out_pic != NULL) {
        delete _out_pic;
        _out_pic = NULL;
    }

    if (_x264_param != NULL) {
        delete _x264_param;
        _x264_param = NULL;
    }
}
    
