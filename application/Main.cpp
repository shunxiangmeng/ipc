#include <chrono>
#include <thread>
#include "Media.h"
#include "configManager/include/IConfigManager.h"
#include "rtsp/include/IRtspService.h"
#include "private/include/IPrivServer.h"
#include "http/include/IHttpServer.h"
#include "infra/include/Logger.h"
#include "infra/include/network/Pipe.h"
#include "infra/include/network/Network.h"
#include "infra/include/Timestamp.h"
#include "infra/include/network/NetworkThreadPool.h"
#include "infra/include/thread/WorkThreadPool.h"
#include "infra/include/Command.h"
#include "infra/include/RedirServer.h"
#include "api/api.h"
#include "oac/include/OacServer.h"
#include "OacClientTest.h"
#include "platform/include/IPlatform.h"
#include "ulucu/include/IUlucu.h"

int main(int argc, char* argv[]) {

    std::shared_ptr<infra::LogChannel> console_log = std::make_shared<infra::ConsoleLogChannel>();
    infra::Logger::instance().addLogChannel(console_log);

    std::shared_ptr<infra::LogChannel> file_log = std::make_shared<infra::FileLogChannel>("./log");
    infra::Logger::instance().addLogChannel(file_log);

    std::string config_path = "";
    std::string default_config_path = "";
    IConfigManager::instance()->init(config_path.data(), default_config_path.data());

    // 开启重定向功能
    infra::RedirServer::instance("/tmp/broncoRedir")->start();

    infof("bronco start............\n");

    infra::network_init();
    infra::NetworkThreadPool::instance()->init(4);
    infra::WorkThreadPool::instance()->init(4);

    //IHttpServer::Config config;
    //config.www_root_path = "E:/ulucu/code/mpegts.js/demo";
    //config.default_index_file = "arib.html";
    //IHttpServer::instance()->config(config);
    //IHttpServer::instance()->start(18080);

    //apiInit();

    AppMedia media;
    media.start();

    IRtspService::instance()->start(8554);
    IRtspService::instance()->setAuthority(false);
    IPrivServer::instance()->start();
    //std::this_thread::sleep_for(std::chrono::milliseconds(40));
    //RtspService::instance()->stop();

    oac::IOacServer::instance()->start();

    //auto platform = platform::IPlatform::create();
    //platform::DeviceAttribute attribute;
    //platform->init(&attribute);

    //ulucu::IUlucu::instance()->init();

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        //tracef("buffer used memory %lld\n", infra::BufferMemoryStatistic::instance()->used());
        //tracef("ObjectStatistic Buffer %d\n", infra::ObjectStatistic<infra::Buffer>::count());
        //tracef("ObjectStatistic MediaFrame %d\n", infra::ObjectStatistic<MediaFrame>::count());
    }
    return 0;
}