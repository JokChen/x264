#include "Macro.h"

typedef void (CALLBACK *H264DataCallBack)(void* pdata,int datalen);

extern "C" 
{
	dllexport void initX264Encode(H264DataCallBack h264callback);

	dllexport void setParameter(int width, int height, int fps, int bite);

	dllexport void encoderH264(void* pdata,unsigned int datalen,long long time);

    dllexport void releaseX264Encode();
}