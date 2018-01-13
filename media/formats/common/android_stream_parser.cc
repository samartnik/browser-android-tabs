#include "media/formats/common/android_stream_parser.h"

#include <memory>
#include <string>

#include "base/callback.h"
#include "base/callback_helpers.h"
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "media/base/media_track.h"
#include "media/base/media_tracks.h"
#include "media/base/media_util.h"
#include "media/base/timestamp_constants.h"
#include "media/formats/webm/webm_cluster_parser.h"
#include "media/formats/webm/webm_constants.h"
#include "media/formats/webm/webm_content_encodings.h"
#include "media/formats/webm/webm_info_parser.h"
#include "media/formats/webm/webm_tracks_parser.h"

namespace media {

AndroidStreamParser::AndroidStreamParser(const std::string& codec):
is_first_parse_call_(true),
codec_(codec) {
}

AndroidStreamParser::~AndroidStreamParser() {
}

void AndroidStreamParser::Init(
    const InitCB& init_cb,
    const NewConfigCB& config_cb,
    const NewBuffersCB& new_buffers_cb,
    bool ignore_text_tracks,
    const EncryptedMediaInitDataCB& encrypted_media_init_data_cb,
    const NewMediaSegmentCB& new_segment_cb,
    const EndMediaSegmentCB& end_of_segment_cb,
    MediaLog* media_log) {
  DCHECK(init_cb_.is_null());
  DCHECK(!init_cb.is_null());
  DCHECK(!config_cb.is_null());
  DCHECK(!new_buffers_cb.is_null());
  DCHECK(!encrypted_media_init_data_cb.is_null());
  DCHECK(!new_segment_cb.is_null());
  DCHECK(!end_of_segment_cb.is_null());

  init_cb_ = init_cb;
  config_cb_ = config_cb;
  new_buffers_cb_ = new_buffers_cb;
  ignore_text_tracks_ = ignore_text_tracks;
  encrypted_media_init_data_cb_ = encrypted_media_init_data_cb;
  new_segment_cb_ = new_segment_cb;
  end_of_segment_cb_ = end_of_segment_cb;
  media_log_ = media_log;
}

void AndroidStreamParser::Flush() {
  LOG(WARNING) << "SAM: AndroidStreamParser::Flush";
}

bool AndroidStreamParser::Parse(const uint8_t* buf, int size) {
  if (is_first_parse_call_) {
      is_first_parse_call_ = false;
      VideoCodec video_codec;
      AudioCodec audio_codec;
      video_codec = StringToVideoCodec(codec_);
      audio_codec = StringToAudioCodec(codec_);
      is_video_stream_ = (video_codec != kUnknownVideoCodec);
      is_audio_stream_ = (audio_codec != kUnknownAudioCodec);
      std::unique_ptr<MediaTracks> media_tracks = base::MakeUnique<MediaTracks>();
      if (is_video_stream_) {
          VideoDecoderConfig config(video_codec, ::media::VIDEO_CODEC_PROFILE_UNKNOWN,
          ::media::PIXEL_FORMAT_UNKNOWN, ::media::COLOR_SPACE_UNSPECIFIED, VIDEO_ROTATION_0,
          gfx::Size(0, 0), gfx::Rect(0, 0, 0, 0), gfx::Size(0, 0),
          ::media::EmptyExtraData(), ::media::EncryptionScheme(), true);
          media_tracks->AddVideoTrack(config, DemuxerStream::VIDEO, "main", "", "");
      } else if (is_audio_stream_) {
          AudioDecoderConfig config(audio_codec, kUnknownSampleFormat,
                                    CHANNEL_LAYOUT_NONE, 1000, ::media::EmptyExtraData(),
                                    ::media::EncryptionScheme(), true);
          media_tracks->AddAudioTrack(config, DemuxerStream::AUDIO, "main", "", "");
      } else {
          LOG(ERROR) << "SAM: Unknown codec";
          NOTREACHED();
      }

      CHECK(media_tracks.get());
      TextTrackConfigMap text_tracks;
      if (!config_cb_.Run(std::move(media_tracks), text_tracks)) {
        LOG(ERROR) << "SAM: New config data isn't allowed.";
        return false;
      }
      if (!init_cb_.is_null()) {
        InitParameters params(kInfiniteDuration);
        /*params.liveness = DemuxerStream::LIVENESS_LIVE;*/
        params.auto_update_timestamp_offset = true;
        base::ResetAndReturn(&init_cb_).Run(params);
      }
  }
  new_segment_cb_.Run();
  BufferQueueMap buffer_queue_map;
  BufferQueue queue;
  queue.push_back(StreamParserBuffer::CopyFrom(buf, size, true, is_video_stream_ ? DemuxerStream::VIDEO : DemuxerStream::AUDIO, is_video_stream_ ? DemuxerStream::VIDEO : DemuxerStream::AUDIO));
  queue[0]->set_duration(base::TimeDelta::FromMilliseconds(0));
  queue[0]->set_is_key_frame(true);
  buffer_queue_map.insert(std::pair<TrackId, BufferQueue>(is_video_stream_ ? DemuxerStream::VIDEO : DemuxerStream::AUDIO, queue));
  if (!buffer_queue_map.empty() && !new_buffers_cb_.Run(buffer_queue_map)) {
    LOG(ERROR) << "SAM: New buffer callback failed.";
    return false;
  }
  end_of_segment_cb_.Run();
  return true;
}

}  // namespace media
