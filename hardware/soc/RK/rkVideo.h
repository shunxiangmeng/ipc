/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  RKVideo.h
 * Author      :  mengshunxiang 
 * Data        :  2024-05-15 20:27:05
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include "hal/Video.h"

class rkVideo : public IVideo, public infra::Thread {
    rkVideo() = default;
    virtual ~rkVideo() = default;
public:
    static IVideo* instance();

    virtual bool initial(int32_t channel, std::vector<VideoEncodeParams> &video_encode_params) override;
    virtual bool deInitial() override;

    virtual bool setEncodeParams(int32_t channel, int32_t sub_channel, VideoEncodeParams &encode_params) override;
    virtual bool getEncodeParams(int32_t channel, int32_t sub_channel, VideoEncodeParams &encode_params) override;

    void distributeVideoFrame(int32_t channel, int32_t sub_channel, MediaFrame &frame);
    void distributeAudioFrame(MediaFrame &frame);

private:
    bool initVideo();
    bool initAudio();

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
    MediaStreamSignal audio_callback_signal_; 
};
