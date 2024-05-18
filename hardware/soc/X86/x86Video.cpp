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

namespace hal {

IVideo* IVideo::instance() {
    return x86Video::instance();
}

IVideo* x86Video::instance() {
    static x86Video s_video;
    return &s_video;
}

x86Video::x86Video() : init_(false) {
}

bool x86Video::initial(int32_t channel, std::vector<VideoEncodeParams> &video_encode_params) {
    if (!init_) {
        init_ = true;
        std::string filename = "F:\\mp4\\The.Teacher.2022.HD1080P.X264.AAC.Malayalam.CHS.BDYS.mp4";
        //std::string filename = "/home/shawn/test.mp4";
        //std::string filename = "F:\\mp4\\HWZ.2022.EP01.HD1080P.X264.AAC.Mandarin.CHS.BDYS.mp4";
        //std::string filename = "E:\\sample.mp4";
        mp4_reader_.open(filename);
        Thread::start();
    }
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

bool x86Video::startVideoStream(int32_t channel, int32_t sub_channel, MediaStreamProc proc) {
    CodecChannel codec_channel{channel, sub_channel};
    auto it = video_callback_signals_.find(codec_channel);
    if (it == video_callback_signals_.end()) {
        std::shared_ptr<MediaStreamSignal> video_signal = std::make_shared<MediaStreamSignal>();
        video_signal->attach(proc);
        video_callback_signals_[codec_channel] = video_signal;
    } else if (it != video_callback_signals_.end()) {
        return it->second->attach(proc) > 0 ? true : false;
    }
    return false;
}

bool x86Video::stopVideoStream(int32_t channel, int32_t sub_channel, MediaStreamProc proc) {
    CodecChannel codec_channel{ channel, sub_channel };
    auto it = video_callback_signals_.find(codec_channel);
    if (it != video_callback_signals_.end()) {
        return it->second->detach(proc) >= 0 ? true : false;
    }
    return false;
}

bool x86Video::getEncodeParams(int32_t channel, int32_t sub_channel, VideoEncodeParams& params) {
    VideoFrameInfo info;
    mp4_reader_.getVideoInfo(info);
    params.codec = info.codec;
    params.bitrate = 90000;
    params.width = info.width;
    params.height = info.height;
    return true;
}

bool x86Video::setEncodeParams(int32_t channel, int32_t sub_channel, VideoEncodeParams &encode_params) {
    return false;
}

void x86Video::run() {
    infof("MediaPlatformImpl thread start\n");
    MediaFrame frame;
    while (running()) {
        int64_t now = infra::getCurrentTimeMs();
        if (!video_frame_queue_.empty()) {
            frame = video_frame_queue_.front();
            if (frame.dts() <= now) {
                auto it = video_callback_signals_.begin();
                if (it != video_callback_signals_.end()) {
                    (*it->second)(0, 0, frame);
                }
                video_frame_queue_.pop();
            }
        }
        if (!audio_frame_queue_.empty()) {
            frame = audio_frame_queue_.front();
            if (frame.dts() <= now) {
                audio_callback_signal_(0,0,frame);
                audio_frame_queue_.pop();
            }
        }
        
        if (video_frame_queue_.size() > 100 || audio_frame_queue_.size() > 100) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        frame = mp4_reader_.getFrame();
        if (!frame.empty()) {
            if (frame.getMediaFrameType() == Video) {
                video_frame_queue_.push(frame);
                //infof("video queue size:%d\n", video_frame_queue_.size());
            } else {
                audio_frame_queue_.push(frame);
                //infof("audio queue size:%d\n", audio_frame_queue_.size());
            }
        } else {
            int64_t timestamp = 0;
            //mp4_reader_.seek(&timestamp);
            //warnf("seek to %lld\n", timestamp);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    warnf("MediaPlatformImpl thread exit\n");
}

}