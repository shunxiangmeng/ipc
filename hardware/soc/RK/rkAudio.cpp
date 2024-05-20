/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  rkAudio.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-05-20 21:31:43
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "rkAudio.h"
#include <thread>
#include <unistd.h>

namespace hal {

IAudio* IAudio::instance() {
    return rkAudio::instance();
}
rkAudio* rkAudio::instance() {
    static rkAudio s_audio;
    return &s_audio;
}

bool rkAudio::initial(AudioEncodeParams &encode_params) {
    return initAudio();
}

bool rkAudio::deInitial() {
    return false;
}

bool rkAudio::startStream(AudioStreamProc proc) {
    return audio_callback_signal_.attach(proc) > 0 ? true : false;
}

bool rkAudio::stopStream(AudioStreamProc proc) {
    return audio_callback_signal_.detach(proc) >= 0 ? true : false;
}

bool rkAudio::setEncodeParams(AudioEncodeParams &params) {
    return false;
}

bool rkAudio::getEncodeParams(AudioEncodeParams &params) {
    /*AudioFrameInfo info;
    params.codec = info.codec;
    params.channel_count = info.channel_count;
    params.sample_rate = info.sample_rate;
    params.bit_per_sample = info.bit_per_sample;
    */

    AudioFrameInfo info;
    params.codec = G711a;
    params.channel_count = 1;
    params.sample_rate = 8000;
    params.bit_per_sample = 16;
    return true;
}

void rkAudio::distributeAudioFrame(MediaFrame &frame) {
    audio_callback_signal_(frame);
}

bool rkAudio::initAudio() {
    if (media_audio_init() == 0) {
        // 单独使用一个线程接收音频数据
        std::thread([]() {
            AudioFrameInfo info;
            info.codec = G711a;
            info.channel = 1;
            info.channel_count = 1;
            info.bit_per_sample = 16;
            info.sample_rate = 8000;
            while (true) {
                MEDIA_BUFFER mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_AENC, 0, -1);
                //mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_AI, 0, -1);
                if (mb == nullptr) {
                    usleep(1000);
                    continue;
                }

                uint64_t pts = RK_MPI_MB_GetTimestamp(mb);
                pts = pts / 1000; //to ms
                size_t size = RK_MPI_MB_GetSize(mb);
                void *buffer = RK_MPI_MB_GetPtr(mb);

                MediaFrame frame;
                frame.ensureCapacity(int32_t(size));
                frame.putData((const char*)buffer, int32_t(size));
                frame.setSize(int32_t(size));
                frame.setAudioFrameInfo(info);
                frame.setMediaFrameType(Audio);
                frame.setPts(pts).setDts(pts);

                //tracef("Get Frame:ptr:%p, size:%zu, mode:%d, channel:%d, timestamp:%lld\n",
                //    RK_MPI_MB_GetPtr(mb), RK_MPI_MB_GetSize(mb),
                //    RK_MPI_MB_GetModeID(mb), RK_MPI_MB_GetChannelID(mb),
                //    RK_MPI_MB_GetTimestamp(mb));

                //uint8_t *data = (uint8_t*)buffer;
                //tracef("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
                //    data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], 
                //    data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

                RK_MPI_MB_ReleaseBuffer(mb);

                rkAudio::instance()->distributeAudioFrame(frame);
            }
        }).detach();
    }
    return true;
}

}