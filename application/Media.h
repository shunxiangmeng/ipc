#pragma once
#include <string>
#include "infra/include/Signal.h"
#include "common/mediaframe/MediaFrame.h"

class AppMedia {
public:
    AppMedia() = default;
    ~AppMedia() = default;
    bool start();
};
