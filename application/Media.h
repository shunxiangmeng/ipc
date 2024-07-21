#pragma once
#include <string>
#include "infra/include/Signal.h"
#include "common/mediaframe/MediaFrame.h"
#include "configManager/include/IConfigManager.h"
#include "hal/Video.h"
#include "vdsp/include/vsdp.h"

class AppMedia {
public:
    AppMedia() = default;
    ~AppMedia() = default;
    bool start();
private:
    void onVideoConfigVerify(const char*, const Json::Value& config, IConfigManager::ApplyResults& result);
    void onVideoConfigApply(const char*, const Json::Value& config, IConfigManager::ApplyResults& result);

    void configToEncodeParams(const Json::Value &video, std::vector<hal::VideoEncodeParams> &video_encode_params);
private:
    VirtualDSP vdsp;
};
