#include <stdio.h>


extern "C" {
#include "x264.h"
#include "x264_config.h"
}

enum bitrate_lev {
    BIT_VERY_HIGH_LEVEL = 0,
    BIT_HIGH_LEVEL = 1,
    BIT_STANDARD_LEVEL = 2,
    BIT_MEDIUM_LEVEL = 3,
    BIT_LOW_LEVEL = 4,
};


class X264Encode {

public :
    x264_param_t *_x264_param;
    x264_t *_x264_encoder;
    x264_picture_t *_in_pic;

    x264_picture_t *_out_pic;

    uint8_t *picture_buf;

    unsigned int bitratelevel;
private :
    int xwidth;
    int xheight;
    int xfps;

public :
    X264Encode();

    void SetParameter(int width, int height, int fps, int bite);

    void StartEncoder(uint8_t *dataptr, char *&bufdata, int &buflen, int &isKeyFrame);

    void ReleaseEncoder();

private :
    void Flush();

};