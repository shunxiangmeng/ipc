#include "hal/Video.h"
#include "Media.h"
#include "infra/include/Timestamp.h"
#include "configManager/include/IConfigManager.h"
#include "jsoncpp/include/value.h"
#include "infra/include/Logger.h"

bool AppMedia::start() {

    std::vector<hal::VideoEncodeParams> video_encode_params;

    Json::Value video;
    if (IConfigManager::instance()->getConfig("video", video)) {
        tracef("videoconfig:%s\n", video.toStyledString().data());
        if (video.isArray()) {
            for (auto it : video) {
                hal::VideoEncodeParams params;
                if (it.isMember("codec") && it["codec"].isString()) {
                    std::string codec = it["codec"].asString();
                    if (codec == "H264" || codec == "h264") {
                        params.codec = H264;
                    } else if (codec == "H265" || codec == "h265") {
                        params.codec = H265;
                    } else {
                        errorf("unknown codec %s\n", codec.data());
                    }
                }

                if (it.isMember("width") && it["width"].isIntegral()) {
                    params.width = it["width"].asInt();
                }
                if (it.isMember("height") && it["height"].isIntegral()) {
                    params.height = it["height"].asInt();
                }
                if (it.isMember("gop") && it["gop"].isIntegral()) {
                    params.gop = it["gop"].asInt();
                }
                if (it.isMember("bitrate") && it["bitrate"].isIntegral()) {
                    params.bitrate = it["bitrate"].asInt();
                }
                if (it.isMember("fps") && it["fps"].isDouble()) {
                    params.fps = it["fps"].asFloat();
                }

                video_encode_params.push_back(params);
            }
        }
    }


    hal::IVideo::instance()->initial(0, video_encode_params);


    return true;
}
