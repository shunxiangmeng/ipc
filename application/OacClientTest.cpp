#include "OacClientTest.h"
#include "infra/include/Logger.h"

OacClientTest::OacClientTest() : oac_client_(oac::IOacClient::instance()) {
}

bool OacClientTest::init() {
    if (!oac_client_->start()) {
        return false;
    }

    initAlg();

    return start();
}

bool OacClientTest::initAlg() {

    return true;
}

void OacClientTest::run() {
    infof("start oac client test thread\n");
    while (running()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        oac::ImageFrame image;
        oac_client_->getImageFrame(image);
        infof("oac client use image index:%d, pts:%lld\n", image.index, image.timestamp);

        /*ulu_face::SULUImage cv_img;
        cv_img.pixel_format = ulu_face::EPIX_FMT_RGB888;
        cv_img.width = image.width;
        cv_img.height = image.height;
        cv_img.vir_addrs[0]= (ulu_face::ulu_uchat_t*)image.data;
        cv_img.data_sizes[0] = image.size;

        std::vector<ulu_face::SPersonInfo> out_current_info;

        tracef("alg update+++\n");
        person_->Update(cv_img, out_current_info);
        tracef("alg update---\n");
        */
        
        //std::this_thread::sleep_for(std::chrono::milliseconds(20));

        oac_client_->releaseImageFrame(image);

    }

}