/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  X86Video.h
 * Author      :  mengshunxiang 
 * Data        :  2024-05-14 23:11:43
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include "hal/Video.h"
#include "infra/include/thread/Thread.h"
#include "common/mediafiles/mp4/MP4Reader.h"

class x86Video : public IVideo, public infra::Thread {
    x86Video();
    virtual ~x86Video() = default;
public:
    static IVideo* instance();

    virtual bool initial(const int config) override;
    virtual bool deInitial() override;


private:
    virtual void run() override;
private:
    bool init_;
    struct CodecChannel {
        int32_t channel;
        int32_t sub_channel;
        bool operator< (const CodecChannel& other) const {
            return (this->channel * 10 + this->sub_channel) < (other.channel * 10 + other.sub_channel);
        }
    };
    std::map<CodecChannel, std::shared_ptr<MediaStreamSignal>> video_callback_signals_;
    MediaStreamSignal audio_callback_signal_;  // 音频只有一个通道,不考虑多 mic 的情况

    MP4Reader mp4_reader_;
    std::queue<MediaFrame> video_frame_queue_;
    std::queue<MediaFrame> audio_frame_queue_;
};