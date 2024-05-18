/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  x86Audio.h
 * Author      :  mengshunxiang 
 * Data        :  2024-05-18 20:33:14
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include "hal/Audio.h"

namespace hal {
class x86Audio : public IAudio {
    x86Audio() = default;
    virtual ~x86Audio() = default;
public:
    static IAudio* instance();

    virtual bool initial(AudioEncodeParams &encode_params) override;
    virtual bool deInitial() override;

    virtual bool setEncodeParams(AudioEncodeParams &encode_params) override;
    virtual bool getEncodeParams(AudioEncodeParams &encode_params) override;

    virtual bool startStream(AudioStreamProc proc) override;
    virtual bool stopStream(AudioStreamProc proc) override;

private:
    bool init_;
    AudioStreamSignal audio_callback_signal_;  // 音频只有一个通道,不考虑多 mic 的情况
};
}