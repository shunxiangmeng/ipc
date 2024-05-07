#include "mediaImpl.h"

namespace media {

IMedia& IMedia::instance() {
    return MediaImpl::instance();
}

MediaImpl& MediaImpl::instance() {
    static MediaImpl s_media;
    return s_media;
}

MediaImpl::MediaImpl() {
}

MediaImpl::~MediaImpl() {
}

bool MediaImpl::initial(const int config) {
    return false;
}

bool MediaImpl::deInitial() {
    return false;
}

bool MediaImpl::start() {
    return false;
}

bool MediaImpl::stop() {
    return false;
}

/*
IAudio& MediaImpl::audio() {

}

IVideo& MediaImpl::video() {

}

IImage& MediaImpl::image() {

}
*/

}