#include "hal/Video.h"
#include "Media.h"
#include "infra/include/Timestamp.h"

void startMedia(MediaStartProc start_proc, MediaStopProc stop_proc) {
    //media::IMedia::instance().initial(0);
    //media::IMedia::instance().video().initial(0);
    //media::IMediaPlatform::instance().initial(0);
    //media::IMediaPlatform::instance().startVideoEncoder(0, 0);
}

bool AppMedia::start() {

    std::vector<hal::VideoEncodeParams> video_encode_params;

    hal::IVideo::instance()->initial(0, video_encode_params);
    //media::IMediaPlatform::instance().startVideoStream(0, 0, Func(&AppMedia::onVideoFrame, this));
    //media::IMediaPlatform::instance().startAudioStream(Func(&AppMedia::onAudioFrame, this));

    //MediaStopProc proc = Func(&AppMedia::ProcTest, this);

    //MediaStopProcV2 proc2 = Func0(&AppMedia::ProcTest, this);

    //test_signal_.attachV2(MediaStopSignal::TProc(&AppMedia::ProcTest, this));

    return true;
}

void AppMedia::onVideoFrame(MediaFrame &frame) {
    int64_t dts = frame.dts();
    int64_t now = infra::getCurrentTimeMs();
    //warnf("onVideo seq:%d, size:%4d, pts:%lld, now:%lld, diff:%lld\n", frame.sequence(), frame.size(), dts, now, now - dts);
}

void AppMedia::onAudioFrame(MediaFrame &frame) {
    int64_t dts = frame.dts();
    int64_t now = infra::getCurrentTimeMs();
    //warnf("onAudio seq:%d, size:%4d, pts:%lld, now:%lld, diff:%lld\n", frame.sequence(), frame.size(), dts, now, now - dts);
}

int32_t AppMedia::onVideoFrame2(int32_t, MediaFrame &frame) {
    return 0;
}

void AppMedia::ProcTest(std::string& name) {
}