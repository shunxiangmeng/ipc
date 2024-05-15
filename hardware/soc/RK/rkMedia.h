#pragma once
#include <stdint.h>
#include <functional>
#include "soc/RK/include/rkmedia/rkmedia_api.h"
#include "hal/Video.h"

namespace media {

#define RK_VI_CHN_0     0
#define RK_VI_CHN_1     1
#define RK_VI_CHN_2     2
#define RK_VI_CHN_3     3
#define RK_VI_CHN_4     4
#define RK_VI_CHN_5     5
#define RK_VI_CHN_6     6
#define RK_VI_CHN_7     7

#define RKISPP_CHN0     "rkispp_m_bypass"
#define RKISPP_CHN1     "rkispp_scale0"
#define RKISPP_CHN2     "rkispp_scale1"
#define RKISPP_CHN3     "rkispp_scale2"

#define RK_ALSA_PATH    "default"

int rk_mpi_isp_init(int camera_channel);
int rk_mpi_isp_deinit(int camera_channel);
int rk_mpi_system_init();
int rk_mpi_vi_create_chn(int32_t channel, int32_t stream_type, uint32_t width, uint32_t height);
int rk_mpi_venc_create_chn(VENC_CHN s32VencChnId, OutCbFunc cbVenc);
int rk_mpi_venc_create_chn(VENC_CHN s32VencChnId, VideoEncodeParams &params, OutCbFunc cbVenc);
int rk_mpi_vi_venc_bind(RK_S32 s32ViPipe, RK_S32 s32ViChnId, RK_S32 s32VencChnId);

int media_audio_init(void);
int media_audio_deinit(void);
}
