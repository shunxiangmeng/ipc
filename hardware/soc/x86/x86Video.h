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
#include "common/mediafiles/mp4/MP4Reader.h"

namespace hal {

class x86Video : public IVideo {
    x86Video();
    virtual ~x86Video() = default;
public:
    static IVideo* instance();

    virtual bool initial(int32_t channel, std::vector<VideoEncodeParams> &video_encode_params) override;
    virtual bool deInitial(int32_t channel = 0) override;

    virtual bool setEncodeParams(int32_t channel, int32_t sub_channel, VideoEncodeParams &encode_params) override;
    virtual bool getEncodeParams(int32_t channel, int32_t sub_channel, VideoEncodeParams &encode_params) override;

    virtual bool requestIFrame(int32_t channel, int32_t sub_channel) override;

    virtual bool startStream(int32_t channel, int32_t sub_channel, VideoStreamProc proc) override;
    virtual bool stopStream(int32_t channel, int32_t sub_channel, VideoStreamProc proc) override;

    virtual bool getViImage(int32_t channel, int32_t sub_channel, VideoImage &image, int32_t timeout = -1) override;

private:
    struct CodecChannel {
        int32_t channel;
        int32_t sub_channel;
        bool operator< (const CodecChannel& other) const {
            return (this->channel * 10 + this->sub_channel) < (other.channel * 10 + other.sub_channel);
        }
    };
    std::map<CodecChannel, std::shared_ptr<VideoStreamSignal>> video_callback_signals_;
};

}