#include "platform/media/mediaPlatform.h"

namespace media {

class MediaPlatformImpl : public IMediaPlatform {
    MediaPlatformImpl();
    ~MediaPlatformImpl();
public:

    static MediaPlatformImpl& instance();

    virtual bool initial(const int config);

    virtual bool deInitial();

    virtual bool startVideoEncoder(int32_t channel, int32_t sub_channel);
    virtual bool stopVideoEncoder(int32_t channel, int32_t sub_channel);
    virtual bool setVideoEncoderParams(int32_t channel, int32_t sub_channel, const VideoEncodeParams& params);
    virtual bool getVideoEncoderParams(int32_t channel, int32_t sub_channel, VideoEncodeParams& params);
    virtual bool requestIFrame(int32_t channel, int32_t sub_channel);

    virtual bool startAudioEncoder();
    virtual bool stopAudioEncoder();
};

}
