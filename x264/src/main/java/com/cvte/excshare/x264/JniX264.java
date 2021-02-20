package com.cvte.excshare.x264;

import java.nio.ByteBuffer;

/**
 * @author chenxiuning
 * @brief
 * @since 2021/1/4 11:16
 */
public class JniX264 {
    private listener mListener;

    public JniX264(listener listener) {
        mListener = listener;
    }

    static {
        System.loadLibrary("x264-lib");
    }

    private ByteBuffer mVideobuffer;

    public native void initX264Encode();

    public native void setParameter(int width, int height, int fps, int bite);

    public native void encoderH264(byte[] srcData, int length, long time);

    public native void CloseX264Encode();

    private void H264DataCallBackFunc(byte[] buffer, int length) {
        mListener.h264Data(buffer, length);
    }

    public interface listener {
        void h264Data(byte[] buffer, int length);
    }
}
