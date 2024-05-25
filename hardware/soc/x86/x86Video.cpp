/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  X86Video.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-05-14 23:12:58
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "x86Video.h"
#include "infra/include/Timestamp.h"
#include "x86FileMediaSource.h"

namespace hal {

IVideo* IVideo::instance() {
    return x86Video::instance();
}

IVideo* x86Video::instance() {
    static x86Video s_video;
    return &s_video;
}

x86Video::x86Video() {
}

bool x86Video::initial(int32_t channel, std::vector<VideoEncodeParams> &video_encode_params) {
    x86FileMediaSource::instance().initial(nullptr);
    x86FileMediaSource::instance().startVideo([&](int32_t channel, int32_t sub_channel, MediaFrame& frame) {
        auto it = video_callback_signals_.begin();
        if (it != video_callback_signals_.end()) {
            (*it->second)(channel, sub_channel, frame);
        }
    });
    return true;
}

bool x86Video::deInitial(int32_t channel) {
    return false;
}

bool x86Video::requestIFrame(int32_t channel, int32_t sub_channel) {
    /*int64_t timestamp = 5;
    warnf("requestIFrame, seek to %lld\n", timestamp);
    mp4_reader_.seek(&timestamp);
    warnf("requestIFrame, seeked %lld\n", timestamp);
    */
    return true;
}

bool x86Video::startStream(int32_t channel, int32_t sub_channel, VideoStreamProc proc) {
    CodecChannel codec_channel{channel, sub_channel};
    auto it = video_callback_signals_.find(codec_channel);
    if (it == video_callback_signals_.end()) {
        std::shared_ptr<VideoStreamSignal> video_signal = std::make_shared<VideoStreamSignal>();
        int32_t ret = video_signal->attach(proc);
        if (ret < 0) {
            errorf("attach video_signal failed ret:%d\n", ret);
            return false;
        }
        video_callback_signals_[codec_channel] = video_signal;
    } else if (it != video_callback_signals_.end()) {
        return it->second->attach(proc) > 0 ? true : false;
    }
    return true;
}

bool x86Video::stopStream(int32_t channel, int32_t sub_channel, VideoStreamProc proc) {
    CodecChannel codec_channel{ channel, sub_channel };
    auto it = video_callback_signals_.find(codec_channel);
    if (it != video_callback_signals_.end()) {
        return it->second->detach(proc) >= 0 ? true : false;
    }
    return false;
}

bool x86Video::getEncodeParams(int32_t channel, int32_t sub_channel, VideoEncodeParams& params) {
    VideoFrameInfo info;
    x86FileMediaSource::instance().getVideoInfo(info);
    params.codec = info.codec;
    params.bitrate = 90000;
    params.width = info.width;
    params.height = info.height;
    return true;
}

bool x86Video::setEncodeParams(int32_t channel, int32_t sub_channel, VideoEncodeParams &encode_params) {
    return false;
}

bool x86Video::getViImage(int32_t channel, int32_t sub_channel, VideoImage &image, int32_t timeout) {
    return false;
};

}