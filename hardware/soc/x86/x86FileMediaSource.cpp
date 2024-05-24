/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  x86FileMediaSource.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-05-18 20:35:45
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "x86FileMediaSource.h"
#include "infra/include/Timestamp.h"
#include "infra/include/Logger.h"

namespace hal {
    
x86FileMediaSource& x86FileMediaSource::instance() {
    static x86FileMediaSource s_filesource;
    return s_filesource;
}

x86FileMediaSource::x86FileMediaSource() {
}

x86FileMediaSource::~x86FileMediaSource() { 
}

bool x86FileMediaSource::initial(const char* file_name) {
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

bool x86FileMediaSource::deInitial() {
    return false;
}

bool x86FileMediaSource::startVideo(VideoCallback cb) {
    video_callback_ = cb;
    return true;
}

bool x86FileMediaSource::startAudio(AudioCallback cb) {
    audio_callback_ = cb;
    return true;
}

void x86FileMediaSource::getVideoInfo(VideoFrameInfo &videoinfo) {
    mp4_reader_.getVideoInfo(videoinfo);
}

void x86FileMediaSource::getAudioInfo(AudioFrameInfo &audioinfo) {
    mp4_reader_.getAudioInfo(audioinfo);
}

void x86FileMediaSource::run() {
    infof("MediaPlatformImpl thread start\n");
    MediaFrame frame;
    while (running()) {
        int64_t now = infra::getCurrentTimeMs();
        if (!video_frame_queue_.empty()) {
            frame = video_frame_queue_.front();
            if (frame.dts() <= now) {
                if (video_callback_) {
                    video_callback_(0, 0, frame);
                }
                video_frame_queue_.pop();
            }
        }
        if (!audio_frame_queue_.empty()) {
            frame = audio_frame_queue_.front();
            if (frame.dts() <= now) {
                if (audio_callback_) {
                    audio_callback_(frame);
                }
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