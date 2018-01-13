// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/callback_helpers.h"
#include "base/files/file_util.h"
#include "base/path_service.h"
#include "media/base/android/media_data_source_bridge.h"
#include "media/base/media_resource.h"
#include "jni/MediaDataSourceBridge_jni.h"

namespace media {

void MediaDataSourceBridge::OnVideoBufferReady(DemuxerStream::Status status,
                              const scoped_refptr<DecoderBuffer>& buffer) {
  base::AutoLock auto_lock(lock_);
  if (status == DemuxerStream::kOk && !buffer->end_of_stream() && buffer->data_size() > 0) {
    int pos(media_data_.size());
    media_data_.resize(pos + buffer->data_size());
    memcpy(&media_data_.front() + pos, buffer->data(), buffer->data_size());
    if (nullptr != video_stream_) {
      // Request next video buffer
      current_data_requests_++;
      video_stream_->Read(base::Bind(&MediaDataSourceBridge::OnVideoBufferReady, base::Unretained(this)));
    }
  } else {
    data_loaded_ = true;
    LOG(ERROR) << "SAM: This is the end of video stream!";
  }
  current_data_requests_--;
  /*base::FilePath buffer_file;
  LOG(INFO) << "SAM: OnBufferReady: attempting to get app data directory";
  PathService::Get(base::DIR_ANDROID_APP_DATA, &buffer_file);
  LOG(INFO) << "SAM: OnBufferReady: app data directory is " << buffer_file.value();
  if (!base::PathExists(buffer_file)) {
    base::File::Error error;
    if (!base::CreateDirectoryAndGetError(buffer_file, &error)) {
        LOG(ERROR) << "SAM: OnBufferReady failed to create directory " << buffer_file.value() << ". Error: " << base::File::ErrorToString(error);
        DCHECK(false);
    }
  }
  buffer_file = buffer_file.Append("test.webm");
  base::DeleteFile(buffer_file, false);
  LOG(INFO) << "SAM: OnBufferReady: attempting to write to file " << buffer_file.value();
  if (base::WriteFile(buffer_file, (const char*)buffer->data(), buffer->data_size()) != (int)buffer->data_size()) {
      LOG(ERROR) << "SAM: OnBufferReady (failed)";
      DCHECK(false);
  }*/
}

void MediaDataSourceBridge::OnAudioBufferReady(DemuxerStream::Status status,
                              const scoped_refptr<DecoderBuffer>& buffer) {
  base::AutoLock auto_lock(lock_);
  if (status == DemuxerStream::kOk && !buffer->end_of_stream() && buffer->data_size() > 0) {
    int pos(media_data_.size());
    media_data_.resize(pos + buffer->data_size());
    memcpy(&media_data_.front() + pos, buffer->data(), buffer->data_size());
    if (nullptr != audio_stream_) {
      // Request next audio buffer
      current_data_requests_++;
      audio_stream_->Read(base::Bind(&MediaDataSourceBridge::OnAudioBufferReady, base::Unretained(this)));
    }
  } else {
    //data_loaded_ = true;
    if (nullptr != video_stream_) {
        // Start reading data from video stream
        current_data_requests_++;
        video_stream_->Read(base::Bind(&MediaDataSourceBridge::OnVideoBufferReady, base::Unretained(this)));
    }
    LOG(ERROR) << "SAM: This is the end of audio stream!";
  }
  current_data_requests_--;
}

MediaDataSourceBridge::MediaDataSourceBridge(MediaResource* media_resource) :
media_resource_(media_resource),
data_loaded_(false),
current_data_requests_(0),
video_stream_(nullptr),
audio_stream_(nullptr) {
  DCHECK(media_resource_);
  CreateJavaMediaDataSourceBridge();
  std::vector<DemuxerStream*> streams = media_resource_->GetAllStreams();
  for (auto* stream : streams) {
    if (DemuxerStream::VIDEO == stream->type()) {
      LOG(INFO) << "SAM: video stream found";
      video_stream_ = stream;
    } else if (DemuxerStream::AUDIO == stream->type()) {
      LOG(INFO) << "SAM: audio stream found";
      audio_stream_ = stream;
    } else {
      LOG(ERROR) << "SAM: Unknown stream type: " << stream->type();
      NOTREACHED();
    }
  }
  if (nullptr != video_stream_) {
    // Start reading data from video stream
    current_data_requests_++;
    video_stream_->Read(base::Bind(&MediaDataSourceBridge::OnVideoBufferReady, base::Unretained(this)));
  }
  /*if (nullptr != audio_stream_) {
    // Start reading data from audio stream
    current_data_requests_++;
    audio_stream_->Read(base::Bind(&MediaDataSourceBridge::OnAudioBufferReady, base::Unretained(this)));
}*/
}

MediaDataSourceBridge::~MediaDataSourceBridge() {
  LOG(INFO) << "SAM: MediaDataSourceBridge::~MediaDataSourceBridge";
  if (!j_media_data_source_bridge_.is_null()) {
    JNIEnv* env = base::android::AttachCurrentThread();
    CHECK(env);
    Java_MediaDataSourceBridge_destroy(env, j_media_data_source_bridge_);
  }
}

base::android::ScopedJavaGlobalRef<jobject> MediaDataSourceBridge::GetJavaMediaDataSourceBridge() {
  return j_media_data_source_bridge_;
}

void MediaDataSourceBridge::CreateJavaMediaDataSourceBridge() {
  JNIEnv* env = base::android::AttachCurrentThread();
  CHECK(env);

  j_media_data_source_bridge_.Reset(Java_MediaDataSourceBridge_create(
      env, reinterpret_cast<intptr_t>(this)));
}

int MediaDataSourceBridge::ReadAt(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj,
    long position, jbyteArray buffer, int offset, int size) {
  //LOG(INFO) << "SAM: MediaDataSourceBridge::ReadAt " << position << ":" << offset << ":" << size << ":" << media_data_.size();
  while (true) {
    {
      base::AutoLock auto_lock(lock_);
      if (size + position <= (long)media_data_.size()) {
        break;
      }
    }
    LOG(WARNING) << "SAM: Waiting for data: " << media_data_.size() << ", " << current_data_requests_;
    sleep(1);
    {
      base::AutoLock auto_lock(lock_);
      if (data_loaded_ && current_data_requests_ <= 0) {
        LOG(WARNING) << "SAM: Data is loaded: " << current_data_requests_;
        break;
      }
    }
  }
  jbyte* data = env->GetByteArrayElements(buffer, NULL);
  if (!data) {
    LOG(ERROR) << "SAM: MediaDataSourceBridge::ReadAt: failed to GetByteArrayElements";
    return 0;
  }
  // todo: fill the data
  {
    base::AutoLock auto_lock(lock_);
    if (size + position > (long)media_data_.size()) {
      LOG(WARNING) << "SAM: MediaDataSourceBridge::ReadAt: no more data to read";
      return -1;
    } else {
     memcpy(data + offset, &media_data_.front() + position, size);
    }
  }
  env->ReleaseByteArrayElements(buffer, data, 0);
  return size;
}

long MediaDataSourceBridge::GetSize(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj) {
  LOG(INFO) << "SAM: MediaDataSourceBridge::GetSize";
  return -1;
}

void MediaDataSourceBridge::Close(JNIEnv* env, const base::android::JavaParamRef<jobject>& obj) {
  LOG(INFO) << "SAM: MediaDataSourceBridge::Close";
  base::AutoLock auto_lock(lock_);
  media_data_.clear();
}

}  // namespace media
