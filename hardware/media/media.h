#pragma once
#include "video.h"
#include "audio.h"
#include "image.h"

namespace media {

class IMedia {
public:
    static IMedia& instance();

    virtual bool initial(const int config) = 0;

    virtual bool deInitial() = 0;

    virtual bool start() = 0;

    virtual bool stop() = 0;

    /*virtual IAudio& audio() = 0;

    virtual IVideo& video() = 0;

    virtual IImage& image() = 0;
    */
};

}
