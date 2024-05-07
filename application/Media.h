#pragma once
#include <string>
#include "media/media.h"
#include "infra/include/Signal.h"
#include "common/mediaframe/MediaFrame.h"

typedef infra::TSignal<void, std::string&> MediaStartSignal;
typedef MediaStartSignal::Proc MediaStartProc;


typedef infra::TSignal<void, std::string&> MediaStopSignal;
typedef MediaStopSignal::Proc MediaStopProc;

void startMedia(MediaStartProc start_proc, MediaStopProc stop_proc);

class AppMedia {
public:
    AppMedia() = default;
    ~AppMedia() = default;
    bool start();

private:
    void onVideoFrame(MediaFrame &frame);
    void onAudioFrame(MediaFrame &frame);

    void ProcTest(std::string& name);

    int32_t onVideoFrame2(int32_t, MediaFrame &frame);

    MediaStopSignal test_signal_;
};
