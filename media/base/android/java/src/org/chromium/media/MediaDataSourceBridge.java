package org.chromium.media;

import android.annotation.TargetApi;
import android.media.MediaDataSource;

import java.io.IOException;
import java.nio.ByteBuffer;

import org.chromium.base.annotations.CalledByNative;
import org.chromium.base.annotations.JNINamespace;
import org.chromium.base.Log;

/**
* A wrapper around android.media.MediaDataSource that allows the native code to use it.
* See media/base/android/media_data_source_bridge.cc for the corresponding native code.
*/
@JNINamespace("media")
@TargetApi(23)
public class MediaDataSourceBridge extends MediaDataSource {
    private static final String TAG = "cr.media";
    private long mNativeMediaDataSourceBridge;

    @CalledByNative
    private static MediaDataSourceBridge create(long nativeMediaDataSourceBridge) {
        return new MediaDataSourceBridge(nativeMediaDataSourceBridge);
    }

    protected MediaDataSourceBridge(long nativeMediaDataSourceBridge) {
        mNativeMediaDataSourceBridge = nativeMediaDataSourceBridge;
    }

    @CalledByNative
    protected void destroy() {
        mNativeMediaDataSourceBridge = 0;
    }

    @Override
    public synchronized int readAt(long position, byte[] buffer, int offset, int size) throws IOException {
        return nativeReadAt(mNativeMediaDataSourceBridge, position, buffer, offset, size);
    }

    @Override
    public synchronized long getSize() throws IOException {
        return nativeGetSize(mNativeMediaDataSourceBridge);
    }

    @Override
    public synchronized void close() throws IOException {
        nativeClose(mNativeMediaDataSourceBridge);
    }

    private native long nativeGetSize(long nativeMediaDataSourceBridge);
    private native void nativeClose(long nativeMediaDataSourceBridge);
    private native int nativeReadAt(long nativeMediaDataSourceBridge, long position, byte[] buffer, int offset, int size);
}
