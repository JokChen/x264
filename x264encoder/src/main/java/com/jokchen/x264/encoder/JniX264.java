package com.jokchen.x264.encoder;

import java.nio.ByteBuffer;

/**
 * @author chenxiuning
 * @brief
 * @since 2021/1/4 11:16
 */
public class JniX264 {
    private EncodeListener mListener;

    public JniX264(EncodeListener listener) {
        mListener = listener;
    }

    static {
        System.loadLibrary("x264-lib");
    }

    public native void initX264Encode();

    public native void setParameter(int width, int height, int fps, int bite);

    public native void encoderH264(byte[] srcData, int length, long time);

    public native void encoderH264(ByteBuffer yun420p, int length, long time);

    public native void closeX264Encode();

    private void H264DataCallBackFunc(byte[] buffer, int length) {
        mListener.onEncode(buffer, length);
    }
}
