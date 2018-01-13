#ifndef MEDIA_FORMATS_COMMON_ANDROID_STREAM_PARSER_H_
#define MEDIA_FORMATS_COMMON_ANDROID_STREAM_PARSER_H_

#include <stdint.h>

#include <memory>

#include "base/callback_forward.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "media/base/audio_decoder_config.h"
#include "media/base/stream_parser.h"
#include "media/base/video_codecs.h"

namespace media {

class MEDIA_EXPORT AndroidStreamParser : public StreamParser {
 public:
  AndroidStreamParser(const std::string& codec);
  ~AndroidStreamParser() override;

  // StreamParser implementation.
  void Init(const InitCB& init_cb,
            const NewConfigCB& config_cb,
            const NewBuffersCB& new_buffers_cb,
            bool ignore_text_tracks,
            const EncryptedMediaInitDataCB& encrypted_media_init_data_cb,
            const NewMediaSegmentCB& new_segment_cb,
            const EndMediaSegmentCB& end_of_segment_cb,
            MediaLog* media_log) override;
  void Flush() override;
  bool Parse(const uint8_t* buf, int size) override;

 private:
  InitCB init_cb_;
  NewConfigCB config_cb_;
  NewBuffersCB new_buffers_cb_;
  bool ignore_text_tracks_;
  EncryptedMediaInitDataCB encrypted_media_init_data_cb_;
  NewMediaSegmentCB new_segment_cb_;
  EndMediaSegmentCB end_of_segment_cb_;
  MediaLog* media_log_;

  bool is_first_parse_call_;
  bool is_video_stream_;
  bool is_audio_stream_;
  std::string codec_;

  DISALLOW_COPY_AND_ASSIGN(AndroidStreamParser);
};

}  // namespace media

#endif  // MEDIA_FORMATS_COMMON_ANDROID_STREAM_PARSER_H_
