#include "media/mediaPlatform.h"
#include "rkMedia.h"

namespace media {

class MediaPlatformImpl : public IMediaPlatform {
    MediaPlatformImpl();
    ~MediaPlatformImpl();
public:

    static MediaPlatformImpl& instance();

    virtual bool initial(const int config);

    virtual bool deInitial();

    virtual bool startVideoEncoder(int32_t channel, int32_t sub_channel) override;
    virtual bool stopVideoEncoder(int32_t channel, int32_t sub_channel) override;
    virtual bool setVideoEncoderParams(int32_t channel, int32_t sub_channel, const VideoEncodeParams& params) override;
    virtual bool getVideoEncoderParams(int32_t channel, int32_t sub_channel, VideoEncodeParams& params) override;
    virtual bool requestIFrame(int32_t channel, int32_t sub_channel) override;

    virtual bool startVideoStream(int32_t channel, int32_t sub_channel, MediaStreamProc proc) override;
    virtual bool stopVideoStream(int32_t channel, int32_t sub_channel, MediaStreamProc proc) override;

    virtual bool startAudioEncoder();
    virtual bool stopAudioEncoder();

    virtual bool setAudioEncoderParams(int32_t channel, const AudioEncodeParams& params) override;
    virtual bool getAudioEncoderParams(int32_t channel, AudioEncodeParams& params) override;

    virtual bool startAudioStream(MediaStreamProc proc) override;
    virtual bool stopAudioStream(MediaStreamProc proc) override;

    void distributeVideoFrame(int32_t channel, int32_t sub_channel, MediaFrame &frame);
    void distributeAudioFrame(MediaFrame &frame);

private:
    bool initVideo();
    bool initAudio();

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
    MediaStreamSignal audio_callback_signal_; 
};

}
