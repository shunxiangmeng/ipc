#include "oac/include/OacClient.h"
#include "infra/include/thread/Thread.h"

#include "ulucu/include/IULUPose.h"
#include "ulucu/include/IULUPerson.h"
#include "ulucu/include/IULUBaseType.h"
#include "ulucu/include/IULUFace.h"

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

    ulu_face::IULUPerson *person_ = nullptr;

};
