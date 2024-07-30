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

int add(int a, int b) {
    return  a+ b;
}

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

    infra::Pipe pipe;

    infra::optional<bool> is_start = {true};

    infra::optional<infra::Timestamp> t1 = { infra::Timestamp::now() };
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    infra::optional<infra::Timestamp> t2 = { infra::Timestamp::now() };

    if (t1.has_value() && t2.has_value()) {
        infra::TimeDelta delta = *t2 - *t1;
        //tracef("delta micros %lld\n", delta.micros());
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    infra::Timestamp t3 = infra::Timestamp::now();
    
    infra::TimeDelta delta2 = t3 - *t2;
    //tracef("delta2 micros %lld\n", delta2.micros());

    infra::Buffer frame1;
    {
        infra::Buffer frame0(512);

        std::string hello = "this is a test message!";
        frame0.putData(hello.c_str(), (int32_t)hello.length());

        frame1 = frame0;

        //infof("ObjectStatistic Buffer %d\n", infra::ObjectStatistic<infra::Buffer>::count());
    }

    //infof("ObjectStatistic Buffer %d\n", infra::ObjectStatistic<infra::Buffer>::count());
    //infof("frame1 size:%d, content:%s\n", frame1.size(), frame1.data());
    

    infra::NetworkThreadPool::instance()->init(4);
    infra::WorkThreadPool::instance()->init(4);
    
#if 0

    int32_t task_exec_count = 0;

    auto task = [&]() {
        infof("test task %d\n", task_exec_count++);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    };

    auto delay_task = [&]() {
        warnf("test delay task\n");
    };

    infra::Timer timer("test");
    timer.start(100, [&]() {
        static int count = 0;
        errorf("timer %d\n", count++);
        if (count >= 2) {
            timer.stop();
        }
        return true;
    });
#endif

    IHttpServer::Config config;
    config.www_root_path = "E:/ulucu/code/mpegts.js/demo";
    config.default_index_file = "arib.html";
    IHttpServer::instance()->config(config);
    IHttpServer::instance()->start(18080);

    apiInit();

    AppMedia media;
    media.start();

    IRtspService::instance()->start(8554);
    IRtspService::instance()->setAuthority(false);
    IPrivServer::instance()->start();
    //std::this_thread::sleep_for(std::chrono::milliseconds(40));
    //RtspService::instance()->stop();

    //oac::IOacServer::instance()->start();

    auto platform = platform::IPlatform::create();


    platform::DeviceAttribute attribute;
    platform->init(&attribute);

    //ulucu::IUlucu::instance()->init();

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        //infra::WorkThreadPool::instance()->postTask(task);
        //infra::WorkThreadPool::instance()->postDelayedTask(delay_task, 100);
        {

            /*MediaFrame frame(1024);
            infof("0buffer used memory %lld\n", infra::BufferMemoryStatistic::instance()->used());
            MediaFrame frame2 = frame;
            infof("1buffer used memory %lld\n", infra::BufferMemoryStatistic::instance()->used());
            MediaFrame frame3 = std::move(frame);
            infof("2buffer used memory %lld\n", infra::BufferMemoryStatistic::instance()->used());
            */
        }

        //infof("2ObjectStatistic Buffer %d\n", infra::ObjectStatistic<infra::Buffer>::count());
        //infof("2ObjectStatistic MediaFrame %d\n", infra::ObjectStatistic<MediaFrame>::count());
        //infof("3buffer used memory %lld\n", infra::BufferMemoryStatistic::instance()->used());
    }
    return 0;
}