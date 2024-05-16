#include "rkMedia.h"
#include "soc/RK/include/rkaiq/common/rk_aiq_types.h"
#include "soc/RK/include/rkaiq/common/rk_aiq_comm.h"
#include "soc/RK/include/rkaiq/uAPI/rk_aiq_user_api_af.h"
#include "soc/RK/include/rkaiq/uAPI/rk_aiq_user_api_sysctl.h"
#include "soc/RK/include/rkaiq/uAPI/rk_aiq_user_api_imgproc.h"
#include "infra/include/Logger.h"

namespace hal {

#define VI_BUFFER_COUNT 3
#define MAX_AIQ_CTX 4
static rk_aiq_sys_ctx_t *g_aiq_ctx[MAX_AIQ_CTX];
static rk_aiq_working_mode_t g_WDRMode[MAX_AIQ_CTX];

static RK_S32 RK_ISP_Init(RK_S32 CamId, rk_aiq_working_mode_t WDRMode, RK_BOOL MultiCam, const char *iq_file_dir) {
    if (CamId >= MAX_AIQ_CTX) {
        errorf("CamId is over 3\n");
        return -1;
    }

    if (iq_file_dir == NULL) {
        errorf("SAMPLE_COMM_ISP_Init : not start.\n");
        g_aiq_ctx[CamId] = NULL;
        return -1;
    }

    // must set HDR_MODE, before init
    g_WDRMode[CamId] = WDRMode;
    char hdr_str[16];
    snprintf(hdr_str, sizeof(hdr_str), "%d", (int)WDRMode);
    setenv("HDR_MODE", hdr_str, 1);

    rk_aiq_sys_ctx_t *aiq_ctx;
    rk_aiq_static_info_t aiq_static_info;
    XCamReturn ret = rk_aiq_uapi_sysctl_enumStaticMetas(CamId, &aiq_static_info);
    if (ret < 0) {
        errorf("isp init error, camreturn %d\n", ret);
        return -1;
    }

    infof("CamId:%d, sensor_name:%s, iqfiles:%s\n", CamId, aiq_static_info.sensor_info.sensor_name, iq_file_dir);

    aiq_ctx = rk_aiq_uapi_sysctl_init(aiq_static_info.sensor_info.sensor_name, iq_file_dir, NULL, NULL);
    if (aiq_ctx == nullptr) {
        errorf("rk_aiq_uapi_sysctl_init error\n");
        return -1;
    }
    
    if (MultiCam) {
        rk_aiq_uapi_sysctl_setMulCamConc(aiq_ctx, true);
    }
    g_aiq_ctx[CamId] = aiq_ctx;
    return 0;
}

RK_S32 RK_ISP_Run(RK_S32 CamId) {
    if (CamId >= MAX_AIQ_CTX || !g_aiq_ctx[CamId]) {
        errorf("CamId is over 3 or not init\n");
        return -1;
    }
    if (rk_aiq_uapi_sysctl_prepare(g_aiq_ctx[CamId], 0, 0, g_WDRMode[CamId])) {
        errorf("rkaiq engine prepare failed !\n");
        g_aiq_ctx[CamId] = NULL;
        return -1;
    }
    infof("rk_aiq_uapi_sysctl_init/prepare succeed\n");
    if (rk_aiq_uapi_sysctl_start(g_aiq_ctx[CamId])) {
        errorf("rk_aiq_uapi_sysctl_start  failed\n");
        return -1;
    }
    infof("rk_aiq_uapi_sysctl_start succeed\n");
    return 0;
}

RK_S32 RK_ISP_SetFrameRate(RK_S32 CamId, RK_U32 uFps) {
    if (CamId >= MAX_AIQ_CTX || !g_aiq_ctx[CamId]) {
        errorf("%s : CamId is over 3 or not init\n");
        return -1;
    }
    infof("SAMPLE_COMM_ISP_SetFrameRate start %d\n", uFps);

    frameRateInfo_t info;
    info.mode = OP_MANUAL;
    info.fps = uFps;
    RK_S32 ret = rk_aiq_uapi_setFrameRate(g_aiq_ctx[CamId], info);

    infof("SAMPLE_COMM_ISP_SetFrameRate %d succ\n", uFps);
    return ret;
}

#define RK_ISP_AIQ_PATH "/app/config/iqfiles"

int rk_mpi_isp_init(int channel) {
    rk_aiq_working_mode_t hdr_mode = RK_AIQ_WORKING_MODE_NORMAL;

    int fps = 25;

    // if (wdr_is_on) hdr_mode = RK_AIQ_WORKING_MODE_ISP_HDR2;

    char iqfilePath[128] = {0};
    snprintf(iqfilePath, sizeof(iqfilePath), "%s", RK_ISP_AIQ_PATH);

    if (RK_ISP_Init(channel, hdr_mode, RK_FALSE, iqfilePath) < 0) {
        errorf("RK_ISP_Init error\n");
        return -1;
    }
    tracef("RK_ISP_Init succeed\n");

    RK_ISP_Run(channel);
    RK_ISP_SetFrameRate(channel, fps);

    return 0;
}

int rk_mpi_isp_deinit(int channel) {
    if (channel >= MAX_AIQ_CTX || !g_aiq_ctx[channel]) {
        errorf("%s : CamId is over 3 or not init\n");
        return -1;
    }
    tracef("rk_aiq_uapi_sysctl_stop enter\n");
    rk_aiq_uapi_sysctl_stop(g_aiq_ctx[channel], false);
    tracef("rk_aiq_uapi_sysctl_deinit enter\n");
    rk_aiq_uapi_sysctl_deinit(g_aiq_ctx[channel]);
    tracef("rk_aiq_uapi_sysctl_deinit exit\n");
    g_aiq_ctx[channel] = NULL;
    return 0;
}

int rk_mpi_system_init() {
    int ret = RK_MPI_SYS_Init();
    if (ret) {
        errorf("RK_MPI_SYS_Init error %d\n", ret);
    } else {
        infof("RK_MPI_SYS_Init success\n");
        //RK_MPI_SYS_DumpChn(RK_ID_VB);
        //RK_MPI_SYS_DumpChn(RK_ID_SYS);
        //RK_MPI_SYS_DumpChn(RK_ID_VI);
        //RK_MPI_SYS_DumpChn(RK_ID_VENC);
        //RK_MPI_SYS_DumpChn(RK_ID_H264E);
        //RK_MPI_SYS_DumpChn(RK_ID_H265E);
        //RK_MPI_SYS_DumpChn(RK_ID_AENC);
        //RK_MPI_SYS_DumpChn(RK_ID_ADEC);
    }
    return ret;
}

int rk_mpi_vi_create_chn(RK_S32 s32ViPipe, VI_CHN s32ViChnId, uint32_t width, uint32_t height) {
    VI_CHN_ATTR_S vi_chn_attr;
    switch(s32ViChnId) {
        case 0: vi_chn_attr.pcVideoNode = RKISPP_CHN0; break;
        case 1: vi_chn_attr.pcVideoNode = RKISPP_CHN1; break;
        case 2: vi_chn_attr.pcVideoNode = RKISPP_CHN2; break;
        case 3: vi_chn_attr.pcVideoNode = RKISPP_CHN3; break;
        default : break;
    }
    vi_chn_attr.u32BufCnt = VI_BUFFER_COUNT;
    vi_chn_attr.u32Width = width;
    vi_chn_attr.u32Height = height;
    vi_chn_attr.enPixFmt = IMAGE_TYPE_NV12;
    vi_chn_attr.enBufType = VI_CHN_BUF_TYPE_MMAP;
    vi_chn_attr.enWorkMode = VI_WORK_MODE_NORMAL;
    
    int ret = RK_MPI_VI_SetChnAttr(s32ViPipe, s32ViChnId, &vi_chn_attr);
    if (ret) {
        errorf("error: set vi[%d] attr error! code:%d\n", s32ViChnId, ret);
        return ret;
    }

    ret = RK_MPI_VI_EnableChn(s32ViPipe, s32ViChnId);
    if (ret) {
        errorf("error: create vi[%d] error! code:%d\n", s32ViChnId, ret);
        return ret;
    }
    return ret;
}

int rk_mpi_venc_create_chn(VENC_CHN s32VencChnId, OutCbFunc cbVenc) {
    if (s32VencChnId >= VENC_MAX_CHN_NUM) {
        errorf("venc channle %d > VENC_MAX_CHN_NUM\n", s32VencChnId);
        return -1;
    }
    VENC_CHN_ATTR_S venc_chn_attr;
    memset(&venc_chn_attr, 0, sizeof(venc_chn_attr));
    
    venc_chn_attr.stVencAttr.enType = RK_CODEC_TYPE_H264;
    venc_chn_attr.stVencAttr.imageType = IMAGE_TYPE_NV12;
    venc_chn_attr.stVencAttr.enRotation = VENC_ROTATION_0;
    venc_chn_attr.stVencAttr.u32PicWidth = 1920;
    venc_chn_attr.stVencAttr.u32PicHeight = 1080;
    venc_chn_attr.stVencAttr.u32VirWidth = 1920;
    venc_chn_attr.stVencAttr.u32VirHeight = 1080;
    venc_chn_attr.stVencAttr.u32Profile = 66; //66: baseline; 77:MP; 100:HP;

    venc_chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
    venc_chn_attr.stRcAttr.stH264Cbr.u32Gop = 50;
    venc_chn_attr.stRcAttr.stH264Cbr.u32BitRate = 2048;
    venc_chn_attr.stRcAttr.stH264Cbr.fr32DstFrameRateDen = 1;
    venc_chn_attr.stRcAttr.stH264Cbr.fr32DstFrameRateNum = 25;
    venc_chn_attr.stRcAttr.stH264Cbr.u32SrcFrameRateDen = 1;
    venc_chn_attr.stRcAttr.stH264Cbr.u32SrcFrameRateNum = 25;

    int ret = RK_MPI_VENC_CreateChn(s32VencChnId, &venc_chn_attr);
    if (ret) {
        errorf("create venc[%d] error! code:%d\n", s32VencChnId, ret);
        return ret;
    }
    
    MPP_CHN_S stEncChn;
    stEncChn.enModId = RK_ID_VENC;
    stEncChn.s32DevId = 0;
    stEncChn.s32ChnId = s32VencChnId;
    ret = RK_MPI_SYS_RegisterOutCb(&stEncChn, cbVenc);
    if (ret) {
        errorf("register cb for venc[%d] error! code:%d\n", s32VencChnId, ret);
    }
    return ret;
}

int rk_mpi_venc_create_chn(VENC_CHN s32VencChnId, VideoEncodeParams &params, OutCbFunc cbVenc) {
    if (s32VencChnId >= VENC_MAX_CHN_NUM) {
        errorf("venc channle %d > VENC_MAX_CHN_NUM\n", s32VencChnId);
        return -1;
    }
    VENC_CHN_ATTR_S venc_chn_attr;
    memset(&venc_chn_attr, 0, sizeof(venc_chn_attr));
    
    venc_chn_attr.stVencAttr.enType = RK_CODEC_TYPE_H264;
    venc_chn_attr.stVencAttr.imageType = IMAGE_TYPE_NV12;
    venc_chn_attr.stVencAttr.enRotation = VENC_ROTATION_0;
    venc_chn_attr.stVencAttr.u32PicWidth = 1920;
    venc_chn_attr.stVencAttr.u32PicHeight = 1080;
    venc_chn_attr.stVencAttr.u32VirWidth = 1920;
    venc_chn_attr.stVencAttr.u32VirHeight = 1080;
    venc_chn_attr.stVencAttr.u32Profile = 0; //66: baseline; 77:MP; 100:HP;

    venc_chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
    venc_chn_attr.stRcAttr.stH264Cbr.u32Gop = 50;
    venc_chn_attr.stRcAttr.stH264Cbr.u32BitRate = 2048;
    venc_chn_attr.stRcAttr.stH264Cbr.u32SrcFrameRateDen = 1;
    venc_chn_attr.stRcAttr.stH264Cbr.u32SrcFrameRateNum = 25;
    venc_chn_attr.stRcAttr.stH264Cbr.fr32DstFrameRateDen = 1;
    venc_chn_attr.stRcAttr.stH264Cbr.fr32DstFrameRateNum = 25;

    int32_t gop = 50, bitrate = 1024, fps = 25;
    if (params.gop.has_value()) {
        gop = *params.gop;
    }
    if (params.bitrate.has_value()) {
        bitrate = *params.bitrate;
    }
    if (params.fps.has_value()) {
        fps = (int32_t)*params.fps;
    }

    if (params.codec.has_value()) {
        switch (*params.codec) {
            case H264:
                venc_chn_attr.stVencAttr.enType = RK_CODEC_TYPE_H264;
                venc_chn_attr.stVencAttr.u32Profile = 66; //66: baseline; 77:MP; 100:HP;
                venc_chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;

                venc_chn_attr.stRcAttr.stH264Cbr.u32Gop = gop;
                venc_chn_attr.stRcAttr.stH264Cbr.u32BitRate = bitrate;
                venc_chn_attr.stRcAttr.stH264Cbr.u32SrcFrameRateDen = 1;
                venc_chn_attr.stRcAttr.stH264Cbr.u32SrcFrameRateNum = 25;
                venc_chn_attr.stRcAttr.stH264Cbr.fr32DstFrameRateDen = 1;
                venc_chn_attr.stRcAttr.stH264Cbr.fr32DstFrameRateNum = fps;
                break;

            case H265:
                venc_chn_attr.stVencAttr.enType = RK_CODEC_TYPE_H265;
                venc_chn_attr.stVencAttr.u32Profile = 0;  //todo
                venc_chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
                break;
        }
    }

    if (params.width.has_value()) {
        venc_chn_attr.stVencAttr.u32PicWidth = *params.width;
        venc_chn_attr.stVencAttr.u32VirWidth = *params.width;
    }
    if (params.height.has_value()) {
        venc_chn_attr.stVencAttr.u32PicHeight = *params.height;
        venc_chn_attr.stVencAttr.u32VirHeight = *params.height;
    }

    int ret = RK_MPI_VENC_CreateChn(s32VencChnId, &venc_chn_attr);
    if (ret) {
        errorf("create venc[%d] error! code:%d\n", s32VencChnId, ret);
        return ret;
    }
    
    MPP_CHN_S stEncChn;
    stEncChn.enModId = RK_ID_VENC;
    stEncChn.s32DevId = 0;
    stEncChn.s32ChnId = s32VencChnId;
    ret = RK_MPI_SYS_RegisterOutCb(&stEncChn, cbVenc);
    if (ret) {
        errorf("register cb for venc[%d] error! code:%d\n", s32VencChnId, ret);
    }
    return ret;
}

int rk_mpi_vi_venc_bind(RK_S32 s32ViPipe, RK_S32 s32ViChnId, RK_S32 s32VencChnId) {
    MPP_CHN_S stSrcChn;
    stSrcChn.enModId = RK_ID_VI;
    stSrcChn.s32DevId = s32ViPipe;
    stSrcChn.s32ChnId = s32ViChnId;
    
    MPP_CHN_S stDestChn;
    stDestChn.enModId = RK_ID_VENC;
    stDestChn.s32DevId = 0;
    stDestChn.s32ChnId = s32VencChnId;
    
    int ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (ret) {
        errorf("error: bind vi[0] to venc[0] error! code:%d\n", ret);
        return -1;
    }
    return ret;
}

static int rk_mpi_ai_aenc_init(void) {
    AI_TALKVQE_CONFIG_S stAiVqeTalkAttr;
    AI_RECORDVQE_CONFIG_S stAiVqeRecordAttr;

    MPP_CHN_S mpp_chn_ai;
    mpp_chn_ai.enModId = RK_ID_AI;
    mpp_chn_ai.s32ChnId = 0;

    AI_CHN_ATTR_S ai_attr;
    ai_attr.pcAudioNode = (char*)"default";
    ai_attr.enSampleFormat = RK_SAMPLE_FMT_S16;
    ai_attr.u32NbSamples = 320;
    ai_attr.u32SampleRate = 8000;
    ai_attr.u32Channels = 1;
    ai_attr.enAiLayout = AI_LAYOUT_NORMAL; // chanel layout: [ref:mic]; remove ref, output mic mono

    AENC_CHN_ATTR_S aenc_attr;
    aenc_attr.enCodecType = RK_CODEC_TYPE_G711A;
    aenc_attr.u32Bitrate = 8000;
    aenc_attr.u32Quality = 1;

    switch (aenc_attr.enCodecType)
    {
       case RK_CODEC_TYPE_MP3:
            aenc_attr.stAencMP3.u32Channels = 1;
            aenc_attr.stAencMP3.u32SampleRate = 8000;
            break;
       case RK_CODEC_TYPE_MP2:
            aenc_attr.stAencMP2.u32Channels = 1;
            aenc_attr.stAencMP2.u32SampleRate = 8000;
            break;
       case RK_CODEC_TYPE_G711A:
            aenc_attr.stAencG711A.u32Channels = 1;
            aenc_attr.stAencG711A.u32SampleRate = 8000;
            aenc_attr.stAencG711A.u32NbSample = 320;
            break;
       case RK_CODEC_TYPE_G711U:
            aenc_attr.stAencG711U.u32Channels = 1;
            aenc_attr.stAencG711U.u32SampleRate = 8000;
            aenc_attr.stAencG711U.u32NbSample = 320;
            break;
       case RK_CODEC_TYPE_G726:
            aenc_attr.stAencG726.u32Channels = 1;
            aenc_attr.stAencG726.u32SampleRate = 8000;
            break;
       default: 
            errorf("audio codec type error.\n"); 
            break;
    }

    /* 1. create ai */
    int ret = RK_MPI_AI_SetChnAttr(mpp_chn_ai.s32ChnId, &ai_attr);
    if (ret) {
        errorf("Create AI Attr [0] failed! ret=%d\n", ret);
        return ret;
    }
    ret = RK_MPI_AI_EnableChn(mpp_chn_ai.s32ChnId);
    if (ret) {
        errorf("Create AI[0] failed! ret=%d\n", ret);
        return ret;
    }

    /* set volume */
    int uiAiSetVolume = 0;
    if (uiAiSetVolume) {
        ret = RK_MPI_AI_SetVolume(mpp_chn_ai.s32ChnId, uiAiSetVolume);
        if (ret) {
            errorf("Set Volume failed! ret=%d\n", ret);
            RK_MPI_AI_DisableChn(mpp_chn_ai.s32ChnId);
            return ret;
        }    
    }

    /* 2. create aenc */
    MPP_CHN_S mpp_chn_aenc;
    mpp_chn_aenc.enModId = RK_ID_AENC;
    mpp_chn_aenc.s32ChnId = 0;
    ret = RK_MPI_AENC_CreateChn(mpp_chn_aenc.s32ChnId, &aenc_attr);
    if (ret) {
        errorf("Create AENC[0] failed! ret=%d\n", ret);
        RK_MPI_AI_DisableChn(mpp_chn_ai.s32ChnId);
        return ret;
    }

    /* 3. bind ai - aenc */
    ret = RK_MPI_SYS_Bind(&mpp_chn_ai, &mpp_chn_aenc);
    if (ret) {
        errorf("ERROR: Bind ai[0] to aenc[0] error! code:%d\n", ret);
        RK_MPI_AENC_DestroyChn(mpp_chn_aenc.s32ChnId);
        RK_MPI_AI_DisableChn(mpp_chn_ai.s32ChnId);
        return ret;
    }

    /* start ai stream */
    ret = RK_MPI_AI_StartStream(mpp_chn_ai.s32ChnId);
    if (ret) {
        errorf("Start AI Stream failed! s32Ret=%d\n", ret);
        RK_MPI_SYS_UnBind(&mpp_chn_ai, &mpp_chn_aenc);
        RK_MPI_AENC_DestroyChn(mpp_chn_aenc.s32ChnId);
        RK_MPI_AI_DisableChn(mpp_chn_ai.s32ChnId);
        return ret;
    }
    return ret;
}

int media_audio_init(void) {
    return rk_mpi_ai_aenc_init();
}

static int rk_mpi_ai_aenc_deinit(void) {
    MPP_CHN_S mpp_chn_ai;
    MPP_CHN_S mpp_chn_aenc;
    mpp_chn_ai.enModId = RK_ID_AI;
    mpp_chn_ai.s32ChnId = 0;
    mpp_chn_aenc.enModId = RK_ID_AENC;
    mpp_chn_aenc.s32ChnId = 0;
    
    RK_MPI_SYS_UnBind(&mpp_chn_ai, &mpp_chn_aenc);
    RK_MPI_AI_DisableChn(mpp_chn_ai.s32ChnId);
    RK_MPI_AENC_DestroyChn(mpp_chn_aenc.s32ChnId);
    return 0;
}

int media_audio_deinit(void) {
    return rk_mpi_ai_aenc_deinit();
}

}
