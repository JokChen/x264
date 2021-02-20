package com.jokchen.x264.encoder;

public interface EncodeListener {
    void onEncode(byte[] buffer, int length);
}
