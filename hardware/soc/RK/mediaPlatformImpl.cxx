#include <unistd.h>
#include "soc/RK/mediaPlatformImpl.h"
#include "infra/include/Logger.h"

namespace media {

static void media_video_callback(MEDIA_BUFFER mb);

IMediaPlatform& IMediaPlatform::instance() {
    return MediaPlatformImpl::instance();
}

MediaPlatformImpl& MediaPlatformImpl::instance() {
    static MediaPlatformImpl s_media_platform;
    return s_media_platform;
}

MediaPlatformImpl::MediaPlatformImpl() {
}

MediaPlatformImpl::~MediaPlatformImpl() {
}

bool MediaPlatformImpl::initial(const int config) {
    infof("rk mediaplatform initial\n");
    int cam_id = 0;
    int ret = rk_mpi_isp_init(cam_id);
    if (ret < 0) {
        errorf("rk_mpi_isp_init error\n");
        return false;
    }
    
    ret = rk_mpi_system_init();
    if (ret) {
        errorf("rk_mpi_system_init failed\n");
        rk_mpi_isp_deinit(cam_id);
        return false;
    }

    int stream_channel = 1;
    for (int i = 0; i < stream_channel; i++) {
        // 创建VI
        ret = rk_mpi_vi_create_chn(cam_id, i);
        if (ret) {
            errorf("create vi %d error\n", i);
            continue;
        }

        // 创建VENC
        ret = rk_mpi_venc_create_chn(i, media_video_callback);
        if (ret) {
            errorf("create venc %d error\n", i);
            continue;
        }

        // 绑定VI VENC
        ret = rk_mpi_vi_venc_bind(cam_id, i, i);
        if (ret) {
            errorf("bind vi %d venc %d failed\n", i, i);
            continue;
        }
    }
    infof("rk_mpi_isp_init succ\n");

    if (initAudio() == false) {
        errorf("initaudio failed\n");
    }
    return true;
}

bool MediaPlatformImpl::deInitial() {
    return false;
}



bool MediaPlatformImpl::startVideoEncoder(int32_t channel, int32_t sub_channel) {
    //int ret = rk_mpi_vi_create_chn(channel, sub_channel);
    //if (ret) {
    //    return false;
    //}
    //ret = rk_mpi_venc_create_chn(sub_channel, nullptr/*media_video_callback*/);
    //ret = rk_mpi_vi_venc_bind(channel, sub_channel, sub_channel);
    return true;
}

bool MediaPlatformImpl::stopVideoEncoder(int32_t channel, int32_t sub_channel) {
    return false;
}

bool MediaPlatformImpl::setVideoEncoderParams(int32_t channel, int32_t sub_channel, const VideoEncodeParams& params) {
    return false;
}

bool MediaPlatformImpl::getVideoEncoderParams(int32_t channel, int32_t sub_channel, VideoEncodeParams& params) {
    params.codec = H264;
    params.bitrate = 90000;
    params.width = 1920;
    params.height = 1080;
    return false;
}

bool MediaPlatformImpl::requestIFrame(int32_t channel, int32_t sub_channel) {
    return true;
}

bool MediaPlatformImpl::startVideoStream(int32_t channel, int32_t sub_channel, MediaStreamProc proc) {
    infof("startVideoStream channel:%d, sub_channel:%d\n", channel, sub_channel);
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
    CodecChannel codec_channel{channel, sub_channel};
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

bool MediaPlatformImpl::setAudioEncoderParams(int32_t channel, const AudioEncodeParams& params) {
    return false;
}

bool MediaPlatformImpl::getAudioEncoderParams(int32_t channel, AudioEncodeParams& params) {
    AudioFrameInfo info;
    params.codec = G711a;
    params.channel_count = 1;
    params.sample_rate = 8000;
    params.bit_per_sample = 16;
    return true;
}

bool MediaPlatformImpl::startAudioStream(MediaStreamProc proc) {
    return audio_callback_signal_.attach(proc) > 0 ? true : false;
}

bool MediaPlatformImpl::stopAudioStream(MediaStreamProc proc) {
    return audio_callback_signal_.detach(proc) >= 0 ? true : false;
}

void MediaPlatformImpl::distributeVideoFrame(int32_t channel, int32_t sub_channel, MediaFrame &frame) {
    //tracef("distributeFrame sub_channel:%d\n", sub_channel);
    CodecChannel codec_channel{channel, sub_channel};
    auto it = video_callback_signals_.find(codec_channel);
    if (it != video_callback_signals_.end()) {
        (*it->second)(frame);
    }
}

void MediaPlatformImpl::distributeAudioFrame(MediaFrame &frame) {
    audio_callback_signal_(frame);
}

static void media_video_callback(MEDIA_BUFFER mb) {
    int sub_channel = RK_MPI_MB_GetChannelID(mb);
    uint64_t pts = RK_MPI_MB_GetTimestamp(mb);
    pts = pts / 1000; //to ms
    size_t size = RK_MPI_MB_GetSize(mb);
    void *buffer = RK_MPI_MB_GetPtr(mb);

    VideoFrameInfo info;

    int flag = RK_MPI_MB_GetFlag(mb);
    switch (flag) {
        case VENC_NALU_IDRSLICE: info.type = VideoFrame_I; break;
        case VENC_NALU_PSLICE: ; info.type = VideoFrame_P; break;
        default: errorf("RK_MPI_MB_GetFlag %d error!\n", flag); break;
    }

    info.codec = H264;
    info.width = 1920;
    info.height = 1080;

    MB_IMAGE_INFO_S image_info = {0};
    RK_MPI_MB_GetImageInfo(mb, &image_info);

    //infof("chn:%d, flag:%d, pts:%llu, size:%d\n", sub_channel, flag, pts, int32_t(size));
    
    MediaFrame frame;
    frame.ensureCapacity(int32_t(size));
    frame.putData((const char*)buffer, int32_t(size));
    frame.setSize(int32_t(size));

    frame.setVideoFrameInfo(info);
    frame.setPlacementType(Annexb);
    frame.setMediaFrameType(Video);
    frame.setPts(pts).setDts(pts);

    RK_MPI_MB_ReleaseBuffer(mb);

    MediaPlatformImpl::instance().distributeVideoFrame(0, sub_channel, frame);
}

bool MediaPlatformImpl::initAudio() {
    if (media_audio_init() == 0) {
        // 单独使用一个线程接收音频数据
        std::thread([]() {
            AudioFrameInfo info;
            info.codec = G711a;
            info.channel = 1;
            info.channel_count = 1;
            info.bit_per_sample = 16;
            info.sample_rate = 8000;
            while (true) {
                MEDIA_BUFFER mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_AENC, 0, -1);
                //mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_AI, 0, -1);
                if (mb == nullptr) {
                    usleep(1000);
                    continue;
                }

                uint64_t pts = RK_MPI_MB_GetTimestamp(mb);
                pts = pts / 1000; //to ms
                size_t size = RK_MPI_MB_GetSize(mb);
                void *buffer = RK_MPI_MB_GetPtr(mb);

                MediaFrame frame;
                frame.ensureCapacity(int32_t(size));
                frame.putData((const char*)buffer, int32_t(size));
                frame.setSize(int32_t(size));
                frame.setAudioFrameInfo(info);
                frame.setMediaFrameType(Audio);
                frame.setPts(pts).setDts(pts);

                //tracef("Get Frame:ptr:%p, size:%zu, mode:%d, channel:%d, timestamp:%lld\n",
                //    RK_MPI_MB_GetPtr(mb), RK_MPI_MB_GetSize(mb),
                //    RK_MPI_MB_GetModeID(mb), RK_MPI_MB_GetChannelID(mb),
                //    RK_MPI_MB_GetTimestamp(mb));

                //uint8_t *data = (uint8_t*)buffer;
                //tracef("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
                //    data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], 
                //    data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

                RK_MPI_MB_ReleaseBuffer(mb);

                MediaPlatformImpl::instance().distributeAudioFrame(frame);
            }
        }).detach();
    }
    return true;
}

}
