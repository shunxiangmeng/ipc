/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  mediaPlatform.h
 * Author      :  mengshunxiang 
 * Data        :  2024-02-25 15:41:36
 * Description :  对接platform media的抽象层,对下统一接口
 * Note        : 
 ************************************************************************/
#pragma once
#include <stdint.h>
#include "common/mediaframe/MediaDefine.h"
#include "common/mediaframe/MediaFrame.h"
#include "infra/include/Optional.h"
#include "infra/include/Signal.h"

namespace media {

typedef struct {
    infra::optional<VideoCodecType> codec;  // 编码格式
    infra::optional<int32_t> width;
    infra::optional<int32_t> height;
    infra::optional<int32_t> gop;           // I帧间隔
    infra::optional<int32_t> bitrate;       // 码率
} VideoEncodeParams;

// ISP相关参数 
typedef struct {
    infra::optional<int32_t> brightness;    // 亮度
} ImageParams;

typedef struct {
    infra::optional<AudioCodecType> codec;  // 编码格式
    infra::optional<int32_t> channel_count; // 通道数量
    infra::optional<int32_t> sample_rate;
    infra::optional<int32_t> bit_per_sample;
} AudioEncodeParams;

class IMediaPlatform {
public:

    typedef infra::TSignal<void, MediaFrame&> MediaStreamSignal;
    typedef MediaStreamSignal::Proc MediaStreamProc;

    static IMediaPlatform& instance();

    /**
     * @brief 初始化media系统
     * @param[in] config
     * @return 
     */
    virtual bool initial(const int config) = 0;

    virtual bool deInitial() = 0;

    /**
     * @brief 开启 video 编码
     * @param[in] channle 通道号，对应 sensor 编号，只有一个sensor的产品，只能是0
     * @param[in] sub_channel 0-主码流 1-子码流 2-第三码流 3-第四码流
     * @return 
     */ 
    virtual bool startVideoEncoder(int32_t channel, int32_t sub_channel) = 0;
    virtual bool stopVideoEncoder(int32_t channel, int32_t sub_channel) = 0;
    virtual bool setVideoEncoderParams(int32_t channel, int32_t sub_channel, const VideoEncodeParams& params) = 0;
    virtual bool getVideoEncoderParams(int32_t channel, int32_t sub_channel, VideoEncodeParams& params) = 0;
    // 强制 I 帧
    virtual bool requestIFrame(int32_t channel, int32_t sub_channel) = 0;

    /**
     * @brief 绑定指定流的回调函数，开始获取数据，注意：需要调用 startVideoEncoder 才会开始回调数据
     * @param[in] channle 通道号，对应 sensor 编号，只有一个sensor的产品，只能是0
     * @param[in] sub_channel 0-主码流 1-子码流 2-第三码流 3-第四码流
     * @param[in/out] data3
     * @return 
     * Node   上层不直接使用这个接口，此接口一般由 stream 模块调用  
     */
    virtual bool startVideoStream(int32_t channel, int32_t sub_channel, MediaStreamProc proc) = 0;
    virtual bool stopVideoStream(int32_t channel, int32_t sub_channel, MediaStreamProc proc) = 0;

    virtual bool startAudioEncoder() = 0;
    virtual bool stopAudioEncoder() = 0;

    virtual bool setAudioEncoderParams(int32_t channel, const AudioEncodeParams& params) = 0;
    virtual bool getAudioEncoderParams(int32_t channel, AudioEncodeParams& params) = 0;

    virtual bool startAudioStream(MediaStreamProc proc) = 0;
    virtual bool stopAudioStream(MediaStreamProc proc) = 0;

};

}