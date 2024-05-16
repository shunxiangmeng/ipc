#pragma once
#include <string>
#include "media/media.h"
#include "infra/include/Signal.h"
#include "common/mediaframe/MediaFrame.h"

class AppMedia {
public:
    AppMedia() = default;
    ~AppMedia() = default;
    bool start();
};
