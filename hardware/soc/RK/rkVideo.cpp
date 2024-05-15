/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  rkVideo.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-05-15 20:29:40
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "rkVideo.h"

static void media_video_callback(MEDIA_BUFFER mb);

IVideo* rkVideo::instance() {
    static rkVideo s_video;
    return &s_video;
}

bool rkVideo::initial(int32_t channel, std::vector<VideoEncodeParams> &video_encode_params) {
    int cam_id = channel;
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

    // 创建VI
    ret = rk_mpi_vi_create_chn(cam_id, 0, 1920, 1080);
    if (ret) {
        errorf("create vi %d error\n", 0);
        return false;
    }

    for (int i = 0; i < video_encode_params.size(); i++) {
        // 创建VENC
        ret = rk_mpi_venc_create_chn(i, video_encode_params[i], media_video_callback);
        if (ret) {
            errorf("create venc %d error\n", i);
            continue;
        }

        // 绑定VI VENC
        ret = rk_mpi_vi_venc_bind(cam_id, 0, i);
        if (ret) {
            errorf("bind vi %d venc %d failed\n", 0, i);
            continue;
        }
    }

    infof("rk_mpi_isp_init succ\n");
    return true;
}

bool rkVideo::deInitial() {
    return false;
}

bool rkVideo::setEncodeParams(int32_t channel, int32_t sub_channel, VideoEncodeParams &params) {
    return false;
}

bool rkVideo::getEncodeParams(int32_t channel, int32_t sub_channel, VideoEncodeParams& params) {
    params.codec = H264;
    params.bitrate = 90000;
    params.width = 1920;
    params.height = 1080;
    return false;
}

bool rkVideo::startVideoStream(int32_t channel, int32_t sub_channel, MediaStreamProc proc) {
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

bool rkVideo::stopVideoStream(int32_t channel, int32_t sub_channel, MediaStreamProc proc) {
    CodecChannel codec_channel{channel, sub_channel};
    auto it = video_callback_signals_.find(codec_channel);
    if (it != video_callback_signals_.end()) {
        return it->second->detach(proc) >= 0 ? true : false;
    }
    return false;
}

void rkVideo::distributeVideoFrame(int32_t channel, int32_t sub_channel, MediaFrame &frame) {
    //tracef("distributeFrame sub_channel:%d\n", sub_channel);
    CodecChannel codec_channel{channel, sub_channel};
    auto it = video_callback_signals_.find(codec_channel);
    if (it != video_callback_signals_.end()) {
        (*it->second)(frame);
    }
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

bool rkVideo::initAudio() {
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