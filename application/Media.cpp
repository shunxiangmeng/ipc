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
    IConfigManager::instance()->getConfig("video", video);
    int32_t video_sample_fps = 25;
    if (video.isMember("format") && video["format"].isString()) {
        std::string video_format = video["format"].asString();
        if (video_format == "ntsc") {
            video_sample_fps = 30;
        } else {
            video_sample_fps = 25;
        }
    }

    configToEncodeParams(video, video_encode_params);

    //hal::IVideo::instance()->initial(0, video_encode_params, video_sample_fps);

    hal::IVideo::instance()->initialVdsp(video_encode_params);
    vdsp.initial("./test.mp4");

    hal::AudioEncodeParams audio_encode_params;
    hal::IAudio::instance()->initial(audio_encode_params);

    IConfigManager::instance()->attachVerify("video", IConfigManager::ConfigProc(&AppMedia::onVideoConfigVerify, this));
    IConfigManager::instance()->attachApply("video", IConfigManager::ConfigProc(&AppMedia::onVideoConfigApply, this));


    return true;
}

void AppMedia::configToEncodeParams(const Json::Value &video, std::vector<hal::VideoEncodeParams> &video_encode_params) {
    const Json::Value &config = video["config"];
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

void AppMedia::onVideoConfigVerify(const char* name, const Json::Value& config, IConfigManager::ApplyResults& result) {
    tracef("onVideoConfigVerify %s\n", name);
}

void AppMedia::onVideoConfigApply(const char* name, const Json::Value& config, IConfigManager::ApplyResults& result) {
    tracef("onVideoConfigApply:%s\n", config.toStyledString().data());
    Json::Value old_config;
    //IConfigManager::instance()->getConfig(name, old_config);
    std::vector<hal::VideoEncodeParams> video_encode_params;
    configToEncodeParams(config, video_encode_params);

    for (int i = 0; i < video_encode_params.size(); i++) {
        if (!hal::IVideo::instance()->setEncodeParams(0, i, video_encode_params[i])) {
            result = IConfigManager::applyFailed;
        }
    }

    /*for (int i = 0; i < config["config"].size(); i++) {
        auto& it = config["config"][i];
        tracef("%d, %s\n", i, it.toStyledString().data());

        int32_t width = it["width"].asInt();
        int32_t height = it["height"].asInt();
        if (!hal::IVideo::instance()->setResolution(0, i, width, height)) {
            errorf("set resolution %dx%d error\n", width, height);
        } else {
            infof("set resolution %dx%d succ\n", width, height);
        }
    }*/

    //result = IConfigManager::applySuccess;
    tracef("onVideoConfigApply end, size:%d\n", config["config"].size());
}