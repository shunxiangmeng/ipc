#pragma once
#include <stdint.h>

namespace media {

class IVideo {
public:

    static IVideo& instance();

    virtual bool initial(const int config) = 0;
    virtual bool deInitial() = 0;

    /*
    channel: sensor 通道号，目前只支持一个sensor，只能是0
    stream_type: 0-主码流， 1-子码流，2-3码流
    */
    virtual bool createVideoEncoder(int32_t channel, int32_t stream_type) = 0;
    virtual bool deleteVideoEncoder(int32_t channel, int32_t stream_type) = 0;
    virtual bool startVideoEncoder(int32_t channel, int32_t stream_type) = 0;
    virtual bool stopVideoEncoder(int32_t channel, int32_t stream_type) = 0;
    virtual bool setEncodeParams(int32_t channel, int32_t stream_type) = 0;
    virtual bool getEncodeParams(int32_t channel, int32_t stream_type) = 0;
    // 强制 I 帧
    virtual bool requestIFrame(int32_t channel, int32_t stream_type) = 0;

    virtual bool createVideoDecoder() = 0;
    virtual bool deleteVideoDecoder() = 0;
    virtual bool startVideoDecoder() = 0;
    virtual bool stopVideoDecoder() = 0;
    virtual bool sendDecoderPacket() = 0;

};

}