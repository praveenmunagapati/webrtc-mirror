/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "webrtc/api/audio_codecs/g711/audio_decoder_g711.h"

#include <memory>
#include <vector>

#include "webrtc/common_types.h"
#include "webrtc/modules/audio_coding/codecs/g711/audio_decoder_pcm.h"
#include "webrtc/rtc_base/ptr_util.h"
#include "webrtc/rtc_base/safe_conversions.h"

namespace webrtc {

rtc::Optional<AudioDecoderG711::Config> AudioDecoderG711::SdpToConfig(
    const SdpAudioFormat& format) {
  const bool is_pcmu = STR_CASE_CMP(format.name.c_str(), "PCMU") == 0;
  const bool is_pcma = STR_CASE_CMP(format.name.c_str(), "PCMA") == 0;
  if (format.clockrate_hz == 8000 && format.num_channels >= 1 &&
      (is_pcmu || is_pcma)) {
    Config config;
    config.type = is_pcmu ? Config::Type::kPcmU : Config::Type::kPcmA;
    config.num_channels = rtc::dchecked_cast<int>(format.num_channels);
    RTC_DCHECK(config.IsOk());
    return rtc::Optional<Config>(config);
  } else {
    return rtc::Optional<Config>();
  }
}

void AudioDecoderG711::AppendSupportedDecoders(
    std::vector<AudioCodecSpec>* specs) {
  for (const char* type : {"PCMU", "PCMA"}) {
    specs->push_back({{type, 8000, 1}, {8000, 1, 64000}});
  }
}

std::unique_ptr<AudioDecoder> AudioDecoderG711::MakeAudioDecoder(
    const Config& config) {
  RTC_DCHECK(config.IsOk());
  switch (config.type) {
    case Config::Type::kPcmU:
      return rtc::MakeUnique<AudioDecoderPcmU>(config.num_channels);
    case Config::Type::kPcmA:
      return rtc::MakeUnique<AudioDecoderPcmA>(config.num_channels);
    default:
      return nullptr;
  }
}

}  // namespace webrtc
