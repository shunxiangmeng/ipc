#include "hardware/soc/win32/mediaPlatformImpl.h"
#include "infra/include/Timestamp.h"

namespace media {

IMediaPlatform& IMediaPlatform::instance() {
    return MediaPlatformImpl::instance();
}

MediaPlatformImpl& MediaPlatformImpl::instance() {
    static MediaPlatformImpl s_media_platform;
    return s_media_platform;
}

MediaPlatformImpl::MediaPlatformImpl() : init_(false) {
}

MediaPlatformImpl::~MediaPlatformImpl() {
}

bool MediaPlatformImpl::initial(const int config) {
    if (!init_) {
        init_ = true;
        //std::string filename = "F:\\mp4\\The.Teacher.2022.HD1080P.X264.AAC.Malayalam.CHS.BDYS.mp4";
        std::string filename = "F:\\mp4\\Last.Film.Show.2021.HD1080P.X264.AAC.Hindi.CHS.BDYS.mp4";
        //std::string filename = "F:\\mp4\\HWZ.2022.EP01.HD1080P.X264.AAC.Mandarin.CHS.BDYS.mp4";
        //std::string filename = "E:\\sample.mp4";
        mp4_reader_.open(filename);
        Thread::start();
    }
    return true;
}

bool MediaPlatformImpl::deInitial() {
    return false;
}

bool MediaPlatformImpl::startVideoEncoder(int32_t channel, int32_t sub_channel) {
    CodecChannel codec_channel{channel, sub_channel};
    return false;
}

bool MediaPlatformImpl::stopVideoEncoder(int32_t channel, int32_t sub_channel) {
    return false;
}

bool MediaPlatformImpl::setVideoEncoderParams(int32_t channel, int32_t sub_channel, const VideoEncodeParams& params) {
    return false;
}

bool MediaPlatformImpl::getVideoEncoderParams(int32_t channel, int32_t sub_channel, VideoEncodeParams& params) {
    VideoFrameInfo info;
    mp4_reader_.getVideoInfo(info);
    params.codec = info.codec;
    params.bitrate = 90000;
    params.width = info.width;
    params.height = info.height;
    return true;
}

bool MediaPlatformImpl::setAudioEncoderParams(int32_t channel, const AudioEncodeParams& params) {
    return false;
}

bool MediaPlatformImpl::getAudioEncoderParams(int32_t channel, AudioEncodeParams& params) {
    AudioFrameInfo info;
    mp4_reader_.getAudioInfo(info);
    params.codec = info.codec;
    params.channel_count = info.channel_count;
    params.sample_rate = info.sample_rate;
    params.bit_per_sample = info.bit_per_sample;
    return true;
}

bool MediaPlatformImpl::requestIFrame(int32_t channel, int32_t sub_channel) {
    /*int64_t timestamp = 5;
    warnf("requestIFrame, seek to %lld\n", timestamp);
    mp4_reader_.seek(&timestamp);
    warnf("requestIFrame, seeked %lld\n", timestamp);
    */
    return true;
}

bool MediaPlatformImpl::startVideoStream(int32_t channel, int32_t sub_channel, MediaStreamProc proc) {
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

bool MediaPlatformImpl::stopVideoStream(int32_t channel, int32_t sub_channel, MediaStreamProc proc) {
    CodecChannel codec_channel{ channel, sub_channel };
    auto it = video_callback_signals_.find(codec_channel);
    if (it != video_callback_signals_.end()) {
        return it->second->detach(proc) >= 0 ? true : false;
    }
    return false;
}


bool MediaPlatformImpl::startAudioEncoder() {
    return false;
}

bool MediaPlatformImpl::stopAudioEncoder() {
    return false;
}

bool MediaPlatformImpl::startAudioStream(MediaStreamProc proc) {
    return audio_callback_signal_.attach(proc) > 0 ? true : false;
}

bool MediaPlatformImpl::stopAudioStream(MediaStreamProc proc) {
    return audio_callback_signal_.detach(proc) >= 0 ? true : false;
}

void MediaPlatformImpl::run() {
    infof("MediaPlatformImpl thread start\n");
    MediaFrame frame;
    while (running()) {
        int64_t now = infra::getCurrentTimeMs();
        if (!video_frame_queue_.empty()) {
            frame = video_frame_queue_.front();
            if (frame.dts() <= now) {
                auto it = video_callback_signals_.begin();
                if (it != video_callback_signals_.end()) {
                    (*it->second)(frame);
                }
                video_frame_queue_.pop();
            }
        }
        if (!audio_frame_queue_.empty()) {
            frame = audio_frame_queue_.front();
            if (frame.dts() <= now) {
                audio_callback_signal_(frame);
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
