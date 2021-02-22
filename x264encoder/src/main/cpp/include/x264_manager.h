#include "Macro.h"

typedef void (CALLBACK *H264DataCallBack)(void *pdata, int datalen);

extern "C"
{
dllexport void InitX264Encode(H264DataCallBack h264callback);

dllexport void SetParameter(int width, int height, int fps, int bite);

dllexport void EncoderH264(void *pdata, unsigned int datalen, long long time);

dllexport void ReleaseX264Encode();
}