#pragma once
#include <string>
#include "infra/include/Signal.h"
#include "common/mediaframe/MediaFrame.h"
#include "configManager/include/IConfigManager.h"

class AppMedia {
public:
    AppMedia() = default;
    ~AppMedia() = default;
    bool start();
private:
    void onVideoConfigVerify(const char*, const Json::Value& config, IConfigManager::ApplyResults& result);
    void onVideoConfigApply(const char*, const Json::Value& config, IConfigManager::ApplyResults& result);
};
