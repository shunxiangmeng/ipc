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

    for (int i = 0; i < video_encode_params.size(); i++) {
        // 创建VI
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
    }

    encode_params_ = video_encode_params;
    infof("rk_mpi_isp_init succ\n");
    return true;
}

bool rkVideo::deInitial(int32_t channel) {
    return false;
}

bool rkVideo::setEncodeParams(int32_t channel, int32_t sub_channel, VideoEncodeParams &params) {
    return false;
}

bool rkVideo::getEncodeParams(int32_t channel, int32_t sub_channel, VideoEncodeParams& params) {
    return rk_mpi_get_venc_attr(sub_channel, params) == 0;
}

bool rkVideo::requestIFrame(int32_t channel, int32_t sub_channel) {
    return false;
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
        if (encode_params_[sub_channel].codec.has_value()) {
            codec = *encode_params_[sub_channel].codec;
        }
        if (encode_params_[sub_channel].width.has_value()) {
            width = *encode_params_[sub_channel].width;
        }
        if (encode_params_[sub_channel].height.has_value()) {
            height = *encode_params_[sub_channel].height;
        }
    }
}

static void media_video_callback(MEDIA_BUFFER mb) {
    static VideoFrameInfo info[4];

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

    RK_MPI_MB_ReleaseBuffer(mb);

    rkVideo::instance()->distributeVideoFrame(0, sub_channel, frame);
}

bool rkVideo::getViImage(int32_t channel, int32_t sub_channel, VideoImage &image, int32_t timeout) {
    MEDIA_BUFFER buf = RK_MPI_SYS_GetMediaBuffer(RK_ID_VI, sub_channel, timeout);
    if (buf == nullptr) {
        errorf("GetMediaBuffer chn [%d] wait %dms null...!\n", sub_channel, timeout);
        return false;
    }
    MB_IMAGE_INFO_S stImageInfo;
    RK_MPI_MB_GetImageInfo(buf, &stImageInfo);
    uint64_t pts = RK_MPI_MB_GetTimestamp(buf);
    //tracef("viimage pts:%lld\n", pts);

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

}