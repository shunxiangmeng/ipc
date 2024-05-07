#include <map>
#include <queue>
#include "platform/media/mediaPlatform.h"
#include "infra/include/thread/Thread.h"
#include "common/mediafiles/mp4/MP4Reader.h"

namespace media {

class MediaPlatformImpl : public IMediaPlatform, public infra::Thread {
    MediaPlatformImpl();
    ~MediaPlatformImpl();
public:

    static MediaPlatformImpl& instance();

    virtual bool initial(const int config);

    virtual bool deInitial();

    virtual bool startVideoEncoder(int32_t channel, int32_t sub_channel);
    virtual bool stopVideoEncoder(int32_t channel, int32_t sub_channel);
    virtual bool setVideoEncoderParams(int32_t channel, int32_t sub_channel, const VideoEncodeParams& params);
    virtual bool getVideoEncoderParams(int32_t channel, int32_t sub_channel, VideoEncodeParams& params);
    virtual bool requestIFrame(int32_t channel, int32_t sub_channel);

    virtual bool startVideoStream(int32_t channel, int32_t sub_channel, MediaStreamProc proc) override;
    virtual bool stopVideoStream(int32_t channel, int32_t sub_channel, MediaStreamProc proc) override;

    virtual bool startAudioEncoder();
    virtual bool stopAudioEncoder();

    virtual bool setAudioEncoderParams(int32_t channel, const AudioEncodeParams& params) override;
    virtual bool getAudioEncoderParams(int32_t channel, AudioEncodeParams& params) override;

    virtual bool startAudioStream(MediaStreamProc proc) override;
    virtual bool stopAudioStream(MediaStreamProc proc) override;

private:
    virtual void run() override;
private:
    bool init_;
    struct CodecChannel {
        int32_t channel;
        int32_t sub_channel;
        bool operator< (const CodecChannel& other) const {
            return (this->channel * 10 + this->sub_channel) < (other.channel * 10 + other.sub_channel);
        }
    };
    std::map<CodecChannel, std::shared_ptr<MediaStreamSignal>> video_callback_signals_;
    MediaStreamSignal audio_callback_signal_;  // 音频只有一个通道,不考虑多 mic 的情况

    MP4Reader mp4_reader_;
    std::queue<MediaFrame> video_frame_queue_;
    std::queue<MediaFrame> audio_frame_queue_;
};

}
