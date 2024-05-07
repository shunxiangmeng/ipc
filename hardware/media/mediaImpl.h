#pragma once
#include "media.h"

namespace media {

class MediaImpl : public IMedia {
    MediaImpl();
    ~MediaImpl();
public:

    static MediaImpl& instance();

    virtual bool initial(const int config);
    virtual bool deInitial();
    virtual bool start();
    virtual bool stop();
    /*virtual IAudio& audio();
    virtual IVideo& video();
    virtual IImage& image();
    */
};
}