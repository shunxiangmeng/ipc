/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  rkAudio.h
 * Author      :  mengshunxiang 
 * Data        :  2024-05-20 21:31:36
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include "hal/Audio.h"
#include "rkMedia.h"

namespace hal {
class rkAudio : public IAudio {
    rkAudio() = default;
    virtual ~rkAudio() = default;
public:
    static rkAudio* instance();

    virtual bool initial(AudioEncodeParams &encode_params) override;
    virtual bool deInitial() override;

    virtual bool setEncodeParams(AudioEncodeParams &encode_params) override;
    virtual bool getEncodeParams(AudioEncodeParams &encode_params) override;

    virtual bool startStream(AudioStreamProc proc) override;
    virtual bool stopStream(AudioStreamProc proc) override;

    void distributeAudioFrame(MediaFrame &frame);
private:
    bool initAudio();

private:
    bool init_;
    AudioStreamSignal audio_callback_signal_;  // 音频只有一个通道,不考虑多 mic 的情况
};
}