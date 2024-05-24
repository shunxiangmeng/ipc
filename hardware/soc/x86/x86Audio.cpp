/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  x86Audio.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-05-18 20:33:45
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "x86Audio.h"
#include "x86FileMediaSource.h"

namespace hal {

IAudio* IAudio::instance() {
    return x86Audio::instance();
}
IAudio* x86Audio::instance() {
    static x86Audio s_audio;
    return &s_audio;
}

bool x86Audio::initial(AudioEncodeParams &encode_params) {
    x86FileMediaSource::instance().initial(nullptr);
    x86FileMediaSource::instance().startAudio([&](MediaFrame& frame) {
        audio_callback_signal_(frame);
    });
    return true;
}

bool x86Audio::deInitial() {
    return false;
}

bool x86Audio::startStream(AudioStreamProc proc) {
    return audio_callback_signal_.attach(proc) > 0 ? true : false;
}

bool x86Audio::stopStream(AudioStreamProc proc) {
    return audio_callback_signal_.detach(proc) >= 0 ? true : false;
}

bool x86Audio::setEncodeParams(AudioEncodeParams &params) {
    return false;
}

bool x86Audio::getEncodeParams(AudioEncodeParams &params) {
    AudioFrameInfo info;
    x86FileMediaSource::instance().getAudioInfo(info);
    params.codec = info.codec;
    params.channel_count = info.channel_count;
    params.sample_rate = info.sample_rate;
    params.bit_per_sample = info.bit_per_sample;
    return true;
}

}