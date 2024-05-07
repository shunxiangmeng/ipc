#pragma once
#include "video.h"

namespace media {

class VideoImpl : public IVideo {
    VideoImpl();
    ~VideoImpl();
public:

    static VideoImpl& instance();

    virtual bool initial(const int config);

    virtual bool deInitial();

    virtual bool createVideoEncoder(int32_t channel, int32_t stream_type);
    virtual bool deleteVideoEncoder(int32_t channel, int32_t stream_type);
    virtual bool startVideoEncoder(int32_t channel, int32_t stream_type);
    virtual bool stopVideoEncoder(int32_t channel, int32_t stream_type);
    virtual bool setEncodeParams(int32_t channel, int32_t stream_type);
    virtual bool getEncodeParams(int32_t channel, int32_t stream_type);
    virtual bool requestIFrame(int32_t channel, int32_t stream_type);

    virtual bool createVideoDecoder();
    virtual bool deleteVideoDecoder();
    virtual bool startVideoDecoder();
    virtual bool stopVideoDecoder();
    virtual bool sendDecoderPacket();
    virtual bool setDecodeParams();
    virtual bool getDecodeParams();
};

}