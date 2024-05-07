#include "platform/soc/FH885XV200/mediaPlatformImpl.h"

namespace media {

IMediaPlatform& IMediaPlatform::instance() {
    return MediaPlatformImpl::instance();
}

MediaPlatformImpl& MediaPlatformImpl::instance() {
    static MediaPlatformImpl s_media_platform;
    return s_media_platform;
}

MediaPlatformImpl::MediaPlatformImpl() {
}

MediaPlatformImpl::~MediaPlatformImpl() {
}

bool MediaPlatformImpl::initial(const int config) {
    return false;
}

bool MediaPlatformImpl::deInitial() {
    return false;
}

bool MediaPlatformImpl::startVideoEncoder(int32_t channel, int32_t sub_channel) {
    return false;
}

bool MediaPlatformImpl::stopVideoEncoder(int32_t channel, int32_t sub_channel) {
    return false;
}

bool MediaPlatformImpl::setVideoEncoderParams(int32_t channel, int32_t sub_channel, const VideoEncodeParams& params) {
    return false;
}

bool MediaPlatformImpl::getVideoEncoderParams(int32_t channel, int32_t sub_channel, VideoEncodeParams& params) {
    return false;
}

bool MediaPlatformImpl::requestIFrame(int32_t channel, int32_t sub_channel) {
    return true;
}


bool MediaPlatformImpl::startAudioEncoder() {
    return false;
}

bool MediaPlatformImpl::stopAudioEncoder() {
    return false;
}

}
