#include "hal/Video.h"
#include "hal/Image.h"
#include "hal/Audio.h"
#include "Media.h"
#include "infra/include/Timestamp.h"
#include "jsoncpp/include/value.h"
#include "infra/include/Logger.h"

bool AppMedia::start() {

    std::vector<hal::VideoEncodeParams> video_encode_params;

    Json::Value video;
    if (IConfigManager::instance()->getConfig("video", video)) {
        Json::Value &config = video["config"];
        tracef("videoconfig:%s\n", video.toStyledString().data());
        if (config.isArray()) {
            for (auto it : config) {
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
                if (it.isMember("bitrate_type") && it["bitrate_type"].isString()) {
                    std::string bitrate_type = it["bitrate_type"].asString();
                    if (bitrate_type == "vbr") {
                        params.rc_mode = VideoVBR;
                    } else if (bitrate_type == "cbr") {
                        params.rc_mode = VideoCBR;
                    }
                }
                if (it.isMember("fps") && it["fps"].isDouble()) {
                    params.fps = it["fps"].asFloat();
                }

                video_encode_params.push_back(params);
            }
        }
    }


    hal::IVideo::instance()->initial(0, video_encode_params);

    if (video.isMember("format") && video["format"].isString()) {
        std::string video_format = video["format"].asString();
        if (video_format == "ntsc") {
            hal::IImage::instance()->setInputFramerate(0, 30);
        } else {
            hal::IImage::instance()->setInputFramerate(0, 25);
        }
    }

    hal::AudioEncodeParams audio_encode_params;
    hal::IAudio::instance()->initial(audio_encode_params);

    IConfigManager::instance()->attachVerify("video", IConfigManager::ConfigProc(&AppMedia::onVideoConfigVerify, this));
    IConfigManager::instance()->attachApply("video", IConfigManager::ConfigProc(&AppMedia::onVideoConfigApply, this));

    return true;
}

void AppMedia::onVideoConfigVerify(const char*, const Json::Value& config, IConfigManager::ApplyResults& result) {

}

void AppMedia::onVideoConfigApply(const char*, const Json::Value& config, IConfigManager::ApplyResults& result) {

}