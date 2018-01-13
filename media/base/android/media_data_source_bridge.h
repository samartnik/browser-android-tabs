#ifndef MEDIA_BASE_ANDROID_MEDIA_DATA_SOURCE_BRIDGE_H_
#define MEDIA_BASE_ANDROID_MEDIA_DATA_SOURCE_BRIDGE_H_

#include "base/android/scoped_java_ref.h"
#include "media/base/decoder_buffer.h"
#include "media/base/demuxer_stream.h"

namespace media {

class MediaResource;

// Implementation of MediaDataSource for Android media player
class MediaDataSourceBridge {
public:
    MediaDataSourceBridge(MediaResource* media_resource);
    virtual ~MediaDataSourceBridge();
    base::android::ScopedJavaGlobalRef<jobject> GetJavaMediaDataSourceBridge();

    // Implementation of MediaDataSource interface
    int ReadAt(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj,
        long position, jbyteArray buffer, int offset, int size);
    long GetSize(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj);
    void Close(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj);
protected:
    // Create the corresponding Java class instance.
    virtual void CreateJavaMediaDataSourceBridge();
private:
    void OnVideoBufferReady(DemuxerStream::Status status,
                                const scoped_refptr<DecoderBuffer>& buffer);
    void OnAudioBufferReady(DemuxerStream::Status status,
                                const scoped_refptr<DecoderBuffer>& buffer);
    // Java MediaDataSourceBridge instance.
    base::android::ScopedJavaGlobalRef<jobject> j_media_data_source_bridge_;

    // Media resource to get data from
    MediaResource* media_resource_;

    std::vector<char> media_data_;
    bool data_loaded_;
    int current_data_requests_;
    mutable base::Lock lock_;
    DemuxerStream* video_stream_;
    DemuxerStream* audio_stream_;
};

}  // namespace media

#endif  // MEDIA_BASE_ANDROID_MEDIA_DATA_SOURCE_BRIDGE_H_
