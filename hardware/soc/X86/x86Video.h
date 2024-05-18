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
#include <map>
#include <queue>
#include "hal/Video.h"
#include "infra/include/thread/Thread.h"
#include "common/mediafiles/mp4/MP4Reader.h"

namespace hal {

class x86Video : public IVideo, public infra::Thread {
    x86Video();
    virtual ~x86Video() = default;
public:
    static IVideo* instance();

    virtual bool initial(int32_t channel, std::vector<VideoEncodeParams> &video_encode_params) override;
    virtual bool deInitial(int32_t channel = 0) override;

    virtual bool setEncodeParams(int32_t channel, int32_t sub_channel, VideoEncodeParams &encode_params) override;
    virtual bool getEncodeParams(int32_t channel, int32_t sub_channel, VideoEncodeParams &encode_params) override;

    virtual bool requestIFrame(int32_t channel, int32_t sub_channel) override;

    virtual bool startVideoStream(int32_t channel, int32_t sub_channel, MediaStreamProc proc) override;
    virtual bool stopVideoStream(int32_t channel, int32_t sub_channel, MediaStreamProc proc) override;

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

}