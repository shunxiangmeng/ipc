/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  x86FileMediaSource.h
 * Author      :  mengshunxiang 
 * Data        :  2024-05-18 20:35:31
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include <map>
#include <queue>
#include <functional>
#include "infra/include/thread/Thread.h"
#include "common/mediafiles/mp4/MP4Reader.h"

namespace hal {
class x86FileMediaSource : public infra::Thread {
    x86FileMediaSource();
    ~x86FileMediaSource();
public:
    static x86FileMediaSource& instance();
    bool initial(const char* file_name);
    bool deInitial();

    typedef std::function<void(int32_t, int32_t, MediaFrame&)> VideoCallback;
    typedef std::function<void(MediaFrame&)> AudioCallback;

    bool startVideo(VideoCallback cb);
    bool startAudio(AudioCallback cb);

    void getVideoInfo(VideoFrameInfo &videoinfo);
    void getAudioInfo(AudioFrameInfo &audioinfo);

private:
    virtual void run() override;
private:
    bool init_;
    MP4Reader mp4_reader_;
    std::queue<MediaFrame> video_frame_queue_;
    std::queue<MediaFrame> audio_frame_queue_;

    VideoCallback video_callback_;
    AudioCallback audio_callback_;
};
}