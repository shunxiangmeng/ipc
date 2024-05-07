#include "videoImpl.h"

namespace media {

IVideo& IVideo::instance() {
    return VideoImpl::instance();
}

VideoImpl& VideoImpl::instance() {
    static VideoImpl s_video;
    return s_video;
}

VideoImpl::VideoImpl() {
}

VideoImpl::~VideoImpl() {
}

bool VideoImpl::initial(const int config) {
    return false;
}

bool VideoImpl::deInitial() {
    return false;
}

bool VideoImpl::createVideoEncoder(int32_t channel, int32_t stream_type) {
    return false;
}

bool VideoImpl::deleteVideoEncoder(int32_t channel, int32_t stream_type) {
    return false;
}

bool VideoImpl::startVideoEncoder(int32_t channel, int32_t stream_type) {
    return false;
}

bool VideoImpl::stopVideoEncoder(int32_t channel, int32_t stream_type) {
    return false;
}

bool VideoImpl::setEncodeParams(int32_t channel, int32_t stream_type) {
    return false;
}

bool VideoImpl::getEncodeParams(int32_t channel, int32_t stream_type) {
    return false;
}

bool VideoImpl::requestIFrame(int32_t channel, int32_t stream_type) {
    return false;
}


bool VideoImpl::createVideoDecoder() {
    return false;
}

bool VideoImpl::deleteVideoDecoder() {
    return false;
}

bool VideoImpl::startVideoDecoder() {
    return false;
}

bool VideoImpl::stopVideoDecoder() {
    return false;
}

bool VideoImpl::sendDecoderPacket() {
    return false;
}

bool VideoImpl::setDecodeParams() {
    return false;
}

bool VideoImpl::getDecodeParams() {
    return false;
}

}