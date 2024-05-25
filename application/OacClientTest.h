#include "oac/include/OacClient.h"
#include "infra/include/thread/Thread.h"

class OacClientTest : public infra::Thread {
public:
    OacClientTest();
    ~OacClientTest() = default;

    bool init();

    bool initAlg();

private:
    virtual void run() override;

private:
    oac::IOacClient *oac_client_;
};
