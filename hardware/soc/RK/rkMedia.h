#pragma once
#include <stdint.h>
#include <functional>
#include "soc/RK/include/rkmedia/rkmedia_api.h"
#include "hal/Video.h"

namespace hal {

#define RK_VI_CHN_0     0
#define RK_VI_CHN_1     1
#define RK_VI_CHN_2     2
#define RK_VI_CHN_3     3
#define RK_VI_CHN_4     4
#define RK_VI_CHN_5     5
#define RK_VI_CHN_6     6
#define RK_VI_CHN_7     7

#define RKISPP_CHN0     "rkispp_scale0"
#define RKISPP_CHN1     "rkispp_scale1"
#define RKISPP_CHN2     "rkispp_scale2"
#define RKISPP_CHN3     "rkispp_m_bypass"

#define RK_ALSA_PATH    "default"

int rk_mpi_isp_init(int camera_channel, int fps = 25);
int rk_mpi_isp_deinit(int camera_channel);
int rk_mpi_isp_set_framerate(int camera_channel, uint32_t fps);
int rk_mpi_isp_get_framerate(int camera_channel);
int rk_mpi_isp_get_sensor_capability(int &width, int &height, int &fps);
int rk_mpi_system_init();
int rk_mpi_vi_create_chn(int32_t channel, int32_t stream_type, uint32_t width, uint32_t height);
int rk_mpi_venc_create_chn(VENC_CHN s32VencChnId, VideoEncodeParams &params, OutCbFunc cbVenc, int32_t src_fps = 25, bool virtual_addr_align = false);
int rk_mpi_vi_venc_bind(RK_S32 s32ViPipe, RK_S32 s32ViChnId, RK_S32 s32VencChnId);
int rk_mpi_vi_venc_unbind(RK_S32 s32ViPipe, RK_S32 s32ViChnId, RK_S32 s32VencChnId);
int rk_mpi_vdec_venc_bind(RK_S32 s32ViPipe, RK_S32 s32VdecChnId, RK_S32 s32VencChnId);

int rk_mpi_get_venc_attr(VENC_CHN s32VencChnId, VideoEncodeParams &params);

int media_audio_init(void);
int media_audio_deinit(void);
}
