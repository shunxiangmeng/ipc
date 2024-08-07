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

namespace hal {

class rkVideo : public IVideo {
    rkVideo() = default;
    virtual ~rkVideo();
public:
    static rkVideo* instance();

    virtual bool setVsdpMode() override;
    virtual bool initialVdsp(std::vector<VideoEncodeParams> &video_encode_params) override;
    virtual bool sendFrameVdsp(MediaFrame &frame) override;

    virtual bool initial(int32_t channel, std::vector<VideoEncodeParams> &video_encode_params, int32_t fps = 25) override;
    virtual bool deInitial(int32_t channel = 0) override;

    virtual bool setEncodeParams(int32_t channel, int32_t sub_channel, VideoEncodeParams &encode_params) override;
    virtual bool getEncodeParams(int32_t channel, int32_t sub_channel, VideoEncodeParams &encode_params) override;

    virtual bool requestIFrame(int32_t channel, int32_t sub_channel) override;

    virtual bool setResolution(int32_t channel, int32_t sub_channel, int32_t width, int32_t height) override;

    virtual bool startStream(int32_t channel, int32_t sub_channel, VideoStreamProc proc) override;
    virtual bool stopStream(int32_t channel, int32_t sub_channel, VideoStreamProc proc) override;
    virtual bool streamIsStarted(int32_t channel, int32_t sub_channel) override;

    //virtual bool waitViImage(int32_t channel, int32_t sub_channel, int32_t timeout = -1) override;
    virtual bool getViImage(int32_t channel, int32_t sub_channel, VideoImage &image, int32_t timeout = -1) override;


    void distributeVideoFrame(int32_t channel, int32_t sub_channel, MediaFrame &frame);
    void distributeAudioFrame(MediaFrame &frame);

    void getEncodeTypeWxH(int32_t sub_channel, VideoCodecType &codec, int32_t &width, int32_t &height);

private:
    void initLogLevel(int level);
    bool initVideo(int32_t channel, int32_t sub_channel, VideoEncodeParams &video_encode_params, int32_t fps = 25);

private:
    bool vdsp_mode_ = false; 
    bool init_;
    bool sub_channel_status_[8] = {0};
    std::vector<VideoEncodeParams> encode_params_;
    struct CodecChannel {
        int32_t channel;
        int32_t sub_channel;
        bool operator< (const CodecChannel& other) const {
            return (this->channel * 10 + this->sub_channel) < (other.channel * 10 + other.sub_channel);
        }
    };
    std::map<CodecChannel, std::shared_ptr<VideoStreamSignal>> video_callback_signals_;
    VideoStreamSignal audio_callback_signal_;

    char* alg_resize_buffer_ = nullptr;
};

}