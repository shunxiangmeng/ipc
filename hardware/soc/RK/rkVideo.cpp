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
#include "rkMedia.h"
#include "infra/include/Logger.h"
#include "hardware/soc/RK/include/rga/rga.h"
#include "hardware/soc/RK/include/rga/im2d.h"

namespace hal {

static void media_video_callback(MEDIA_BUFFER mb);

IVideo* IVideo::instance() {
    return rkVideo::instance();
}

rkVideo* rkVideo::instance() {
    static rkVideo s_video;
    return &s_video;
}

void rkVideo::initLogLevel(int level) {
    LOG_LEVEL_CONF_S log_level;
    for (int i = RK_ID_VB; i < RK_ID_BUTT; i++) {
        log_level.enModId = (MOD_ID_E)i;
        log_level.s32Level = level;
        int ret = RK_MPI_LOG_SetLevelConf(&log_level);
        if (ret != 0) {
            errorf("RK_MPI_LOG_SetLevelConf mod %d ret = %d\n", i, ret);
            continue;
        }
    }
    infof("set rk log_level %d\n", level);

    for (int i = RK_ID_VB; i < RK_ID_BUTT; i++) {
        log_level.enModId = (MOD_ID_E)i;
        int ret = RK_MPI_LOG_GetLevelConf(&log_level);
        if (ret != 0) {
            errorf("RK_MPI_LOG_SetLevelConf mod %d ret = %d\n", i, ret);
            continue;
        } else {
            //tracef("rk_log_level mod:%d level:%d\n", i, log_level.s32Level);
        }
    }
}

bool rkVideo::setVsdpMode() {
    vdsp_mode_ = true;
    warnf("turn on vdsp mode\n");
    return true;
}

bool rkVideo::initial(int32_t channel, std::vector<VideoEncodeParams> &video_encode_params, int32_t fps) {
    initLogLevel(1);
    int cam_id = channel;
    int ret = rk_mpi_isp_init(cam_id, fps);
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

    //int sensor_width, sensor_height, sensor_fps;
    //rk_mpi_isp_get_sensor_capability(sensor_width, sensor_height, sensor_fps);

    for (int i = 0; i < video_encode_params.size(); i++) {
        initVideo(channel, i, video_encode_params[i]);
        // 创建VI
        #if 0
        int32_t width = 1920, height = 1080;
        if (video_encode_params[i].width.has_value()) {
            width = *video_encode_params[i].width;
        }
        if (video_encode_params[i].height.has_value()) {
            height = *video_encode_params[i].height;
        }

        ret = rk_mpi_vi_create_chn(cam_id, i, width, height);
        if (ret) {
            errorf("create vi %d error\n", i);
            return false;
        }
        // 创建VENC
        ret = rk_mpi_venc_create_chn(i, video_encode_params[i], media_video_callback);
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
        #endif
    }

    encode_params_ = video_encode_params;
    infof("rk_mpi_isp_init succ\n");
    return true;
}

bool rkVideo::deInitial(int32_t channel) {
    return false;
}

bool rkVideo::initVideo(int32_t channel, int32_t sub_channel, VideoEncodeParams &video_encode_params, int32_t fps) {
    if (sub_channel_status_[sub_channel]) {
        rk_mpi_vi_venc_unbind(channel, sub_channel, sub_channel);
        RK_MPI_VENC_DestroyChn(sub_channel);
        RK_MPI_VI_DisableChn(channel, sub_channel);
        sub_channel_status_[sub_channel] = false;
    }

    // 创建VI
    int32_t width = video_encode_params.width;
    int32_t height = video_encode_params.height;

    int ret = rk_mpi_vi_create_chn(channel, sub_channel, width, height);
    if (ret) {
        errorf("create vi %d error\n", sub_channel);
        return false;
    }
    // 创建VENC
    ret = rk_mpi_venc_create_chn(sub_channel, video_encode_params, media_video_callback, fps);
    if (ret) {
        errorf("create venc %d error\n", sub_channel);
        return false;
    }
    ret = rk_mpi_vi_venc_bind(channel, sub_channel, sub_channel);
    if (ret) {
        errorf("bind vi %d venc %d failed\n", sub_channel, sub_channel);
        return false;
    }
    sub_channel_status_[sub_channel] = true;
    return true;
}

bool rkVideo::setEncodeParams(int32_t channel, int32_t sub_channel, VideoEncodeParams &params) {
    if (params == encode_params_[sub_channel]) {
        return true;
    }

    if (params.codec != encode_params_[sub_channel].codec 
        || params.width != encode_params_[sub_channel].width || params.height != encode_params_[sub_channel].height) {
        // 编码格式或者分辨率改变，需要重新创建编码器
        int32_t src_fps = rk_mpi_isp_get_framerate(channel);
        if (!initVideo(channel, sub_channel, params, src_fps)) {
            return false;
        }
        encode_params_[sub_channel] = params;
        return true;
    }

    if (params.gop != encode_params_[sub_channel].gop) {
        int ret = RK_MPI_VENC_SetGop(sub_channel, params.gop);
        if (ret != 0) {
            errorf("venc %d set gop:%d error, ret = %d\n", sub_channel, params.gop, ret);
            return false;
        }
    }
    if (params.fps != encode_params_[sub_channel].fps) {
        int32_t src_fps = rk_mpi_isp_get_framerate(channel);
        int ret = RK_MPI_VENC_SetFps(sub_channel, params.fps, 1, src_fps, 1);
        if (ret != 0) {
            errorf("venc %d set fps:%d error, ret = %d\n", sub_channel, params.fps, ret);
            return false;
        }
    }

    if (params.bitrate != encode_params_[sub_channel].bitrate) {
        int ret = RK_MPI_VENC_SetBitrate(sub_channel, params.bitrate * 1024, 32 * 1024, params.bitrate * 1024);
        if (ret != 0) {
            errorf("venc %d set bitrate:%d error, ret = %d\n", sub_channel, params.bitrate, ret);
            return false;
        }
    }

    encode_params_[sub_channel] = params;
    return true;
}

bool rkVideo::getEncodeParams(int32_t channel, int32_t sub_channel, VideoEncodeParams& params) {
    return rk_mpi_get_venc_attr(sub_channel, params) == 0;
}

bool rkVideo::requestIFrame(int32_t channel, int32_t sub_channel) {
    return RK_MPI_VENC_RequestIDR(sub_channel, RK_TRUE) == 0;
}

bool rkVideo::setResolution(int32_t channel, int32_t sub_channel, int32_t width, int32_t height) {
    VI_CHN_ATTR_S vi_chn_attr;
    RK_MPI_VI_GetChnAttr(channel, sub_channel, &vi_chn_attr);
    if (width == vi_chn_attr.u32Width && height == vi_chn_attr.u32Height) {
        tracef("new resolution == old resolution, no need set\n");
        return true;
    }

    VideoEncodeParams encode_param = encode_params_[sub_channel];
    encode_param.width = width;
    encode_param.height = height;

    if (!initVideo(channel, sub_channel, encode_param)) {
        return false;
    }
    encode_params_[sub_channel] = encode_param;
    return true;
}

bool rkVideo::startStream(int32_t channel, int32_t sub_channel, VideoStreamProc proc) {
    infof("startVideoStream channel:%d, sub_channel:%d\n", channel, sub_channel);
    CodecChannel codec_channel{channel, sub_channel};
    auto it = video_callback_signals_.find(codec_channel);
    if (it == video_callback_signals_.end()) {
        std::shared_ptr<VideoStreamSignal> video_signal = std::make_shared<VideoStreamSignal>();
        video_signal->attach(proc);
        video_callback_signals_[codec_channel] = video_signal;
    } else if (it != video_callback_signals_.end()) {
        return it->second->attach(proc) > 0 ? true : false;
    }
    infof("create new stream channel:%d, sub_channel:%d\n", channel, sub_channel);
    return true;
}

bool rkVideo::stopStream(int32_t channel, int32_t sub_channel, VideoStreamProc proc) {
    CodecChannel codec_channel{channel, sub_channel};
    auto it = video_callback_signals_.find(codec_channel);
    if (it != video_callback_signals_.end()) {
        return it->second->detach(proc) >= 0 ? true : false;
    }
    return false;
}

bool rkVideo::streamIsStarted(int32_t channel, int32_t sub_channel) {
    return sub_channel_status_[sub_channel];
}

void rkVideo::distributeVideoFrame(int32_t channel, int32_t sub_channel, MediaFrame &frame) {
    //tracef("distributeFrame sub_channel:%d\n", sub_channel);
    CodecChannel codec_channel{channel, sub_channel};
    auto it = video_callback_signals_.find(codec_channel);
    if (it != video_callback_signals_.end()) {
        (*it->second)(channel, sub_channel, frame);
    }
}

void rkVideo::getEncodeTypeWxH(int32_t sub_channel, VideoCodecType &codec, int32_t &width, int32_t &height) {
    if (encode_params_.size() >= sub_channel + 1) {
        codec = encode_params_[sub_channel].codec;
        width = encode_params_[sub_channel].width;
        height = encode_params_[sub_channel].height;
    }
}

static void media_video_callback(MEDIA_BUFFER mb) {
    static VideoFrameInfo info[4];
    static int32_t s_sequence[4] = {0};

    int sub_channel = RK_MPI_MB_GetChannelID(mb);
    int flag = RK_MPI_MB_GetFlag(mb);
    if (flag == VENC_NALU_IDRSLICE) {
        info[sub_channel].type = VideoFrame_I;
        rkVideo::instance()->getEncodeTypeWxH(sub_channel, info[sub_channel].codec, info[sub_channel].width, info[sub_channel].height);
    } else {
        info[sub_channel].type = VideoFrame_P;
    }

    uint64_t pts = RK_MPI_MB_GetTimestamp(mb);
    pts = pts / 1000; //to ms
    size_t size = RK_MPI_MB_GetSize(mb);
    void *buffer = RK_MPI_MB_GetPtr(mb);

    MB_IMAGE_INFO_S image_info = {0};
    RK_MPI_MB_GetImageInfo(mb, &image_info);

    //if (sub_channel == 0)
    //infof("chn:%d, flag:%d, pts:%llu, codec:%d, wxh:%dx%d, size:%d\n", sub_channel, flag, pts, (int32_t)info[sub_channel].codec, info[sub_channel].width, info[sub_channel].height, int32_t(size));
    
    MediaFrame frame;
    frame.ensureCapacity(int32_t(size));
    frame.putData((const char*)buffer, int32_t(size));
    frame.setSize(int32_t(size));

    frame.setVideoFrameInfo(info[sub_channel]);
    frame.setPlacementType(Annexb);
    frame.setMediaFrameType(Video);
    frame.setPts(pts).setDts(pts);
    frame.setSequence(s_sequence[sub_channel]++);

    RK_MPI_MB_ReleaseBuffer(mb);

    rkVideo::instance()->distributeVideoFrame(0, sub_channel, frame);
}

bool rkVideo::getViImage(int32_t channel, int32_t sub_channel, VideoImage &image, int32_t timeout) {
    MEDIA_BUFFER buf = nullptr;
    if (vdsp_mode_) {
        buf = RK_MPI_SYS_GetMediaBuffer(RK_ID_VDEC, 0, timeout);
    } else {
        buf = RK_MPI_SYS_GetMediaBuffer(RK_ID_VI, sub_channel, timeout);
    }

    if (buf == nullptr) {
        errorf("GetMediaBuffer chn [%d] wait %dms null...!\n", sub_channel, timeout);
        return false;
    }

    MB_IMAGE_INFO_S stImageInfo;
    RK_MPI_MB_GetImageInfo(buf, &stImageInfo);
    uint64_t pts = RK_MPI_MB_GetTimestamp(buf);
    tracef("viimage pts:%lld\n", pts);

    int width = stImageInfo.u32Width, height = stImageInfo.u32Height;
    int src_format = RK_FORMAT_YCbCr_420_SP; // NV12
    int dst_format = RK_FORMAT_RGB_888;      // 送算法需要rgb888

    char *src_buf = (char*)RK_MPI_MB_GetPtr(buf);
    char *dst_buf = (char*)image.data;

    rga_buffer_t src = wrapbuffer_virtualaddr(src_buf, width, height, src_format);
    rga_buffer_t dst = wrapbuffer_virtualaddr(dst_buf, width, height, dst_format);
    IM_STATUS status = imcvtcolor(src, dst, src.format, dst.format);
    if (status != IM_STATUS_SUCCESS) {
        errorf("RGA imcvtcolor failed, status = %d\n", status);
        RK_MPI_MB_ReleaseBuffer(buf);
        return false;
    }

    static uint32_t frame_number = 0; 

    image.width = width;
    image.height = height;
    image.stride - width;
    image.timestamp = pts;
    image.frame_number = frame_number++;
    image.data_size = width * height * 3;

    RK_MPI_MB_ReleaseBuffer(buf);
    return true;
}


bool rkVideo::initialVdsp(std::vector<VideoEncodeParams> &video_encode_params) {
    initLogLevel(1);
    int ret = rk_mpi_system_init();
    if (ret) {
        errorf("rk_mpi_system_init failed\n");
        return false;
    }
    VDEC_CHN_ATTR_S stVdecAttr{};
    stVdecAttr.enCodecType = RK_CODEC_TYPE_H264;
    stVdecAttr.enMode = VIDEO_MODE_FRAME;
    if (stVdecAttr.enCodecType == RK_CODEC_TYPE_JPEG) {
        stVdecAttr.enMode = VIDEO_MODE_FRAME;
    } else {
        stVdecAttr.enMode = VIDEO_MODE_STREAM;
    }
    stVdecAttr.enDecodecMode = VIDEO_DECODEC_HADRWARE;

    //stVdecAttr.enMode = VIDEO_MODE_FRAME;

    int vdec_channel = 0;
    ret = RK_MPI_VDEC_CreateChn(vdec_channel, &stVdecAttr);
    if (ret) {
        errorf("Create Vdec[%d] failed! ret=%d\n", vdec_channel, ret);
        return false;
    }

    // 创建VENC
    for (int sub_channel = 0; sub_channel < video_encode_params.size(); sub_channel++) {
    int32_t fps = 25;
    ret = rk_mpi_venc_create_chn(sub_channel, video_encode_params[0], media_video_callback, fps);
        if (ret) {
            errorf("create venc %d error\n", sub_channel);
            continue;
        }
        ret = rk_mpi_vdec_venc_bind(0/*pipe*/, vdec_channel, sub_channel);
        if (ret) {
            errorf("bind vdec %d venc %d failed\n", vdec_channel, sub_channel);
            continue;
        } else {
            infof("bind vdec %d venc %d succ\n", vdec_channel, sub_channel);
        }
        sub_channel_status_[sub_channel] = true;
    }
    encode_params_ = video_encode_params;
    return true; 

    std::thread([]() {
        tracef("start vdsp thread\n");
        while(true) {
            MEDIA_BUFFER mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_VDEC, 0, 1000);
            if (!mb) {
                errorf("RK_MPI_SYS_GetMediaBuffer get null buffer in 5s...\n");
                continue;
            }
            MB_IMAGE_INFO_S stImageInfo = {0};
            int ret = RK_MPI_MB_GetImageInfo(mb, &stImageInfo);
            if (ret) {
                errorf("Get image info failed! ret = %d\n", ret);
                RK_MPI_MB_ReleaseBuffer(mb);
                continue;
            }

            tracef("Get Frame:ptr:%p, fd:%d, size:%zu, mode:%d, channel:%d, timestamp:%lld, ImgInfo: %dx%d, fmt 0x%x\n",
                    RK_MPI_MB_GetPtr(mb), RK_MPI_MB_GetFD(mb), RK_MPI_MB_GetSize(mb),
                    RK_MPI_MB_GetModeID(mb), RK_MPI_MB_GetChannelID(mb),
                    RK_MPI_MB_GetTimestamp(mb), stImageInfo.u32Width,
                    stImageInfo.u32Height, stImageInfo.enImgType);
            RK_MPI_MB_ReleaseBuffer(mb);
        }
        warnf("exit vdsp thread\n");
    }).detach();

    return true; 
};

bool rkVideo::sendFrameVdsp(MediaFrame &frame) {
    MEDIA_BUFFER mb = RK_MPI_MB_CreateBuffer(frame.size(), RK_TRUE, 0);
    memcpy(RK_MPI_MB_GetPtr(mb), frame.data(), frame.size());
    RK_MPI_MB_SetSize(mb, frame.size());
    RK_MPI_MB_SetTimestamp(mb, frame.dts() * 1000);
    int ret = RK_MPI_SYS_SendMediaBuffer(RK_ID_VDEC, 0, mb);
    if (ret != 0) {
        errorf("RK_MPI_SYS_SendMediaBuffer failed ret = %d\n", ret);
    } else {
        //tracef("vdec send buffer succ size:%d, pts:%lld\n", frame.size(), frame.pts());
    }
    RK_MPI_MB_ReleaseBuffer(mb);
    return ret == 0; 
};


}