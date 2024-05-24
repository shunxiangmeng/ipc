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

std::shared_ptr<unsigned char> read_model_2_buff(const char *file_path, size_t& model_size) {
    std::shared_ptr<unsigned char> p_buff;
    do {
        std::ifstream ifs_model(file_path, std::ios::binary);
        if (!ifs_model.good()) {
            break;
        }
        ifs_model.seekg(0, ifs_model.end);
        model_size = ifs_model.tellg();
        p_buff = std::shared_ptr<unsigned char>((unsigned char*)::malloc(model_size), ::free);
        ifs_model.seekg(0, ifs_model.beg);
        ifs_model.read((char*)(p_buff.get()), model_size);
        ifs_model.close();
    } while (0);
    return p_buff;
}

bool OacClientTest::initAlg() {
    char alg_sdk_version[16] = {0};
    ulu_face::GetVersion(alg_sdk_version);
    infof("alg version: %s\n", alg_sdk_version);

    if (ulu_face::SDKInit("") != ulu_face::E_OK) {
        errorf("alg sdk init failed\n");
        return false;
    }
    infof("000\n");
    ulu_face::SetDefaultPixelFormat(ulu_face::EPIX_FMT_RGB888);
    ulu_face::SetTrackMaxAge(7);
    ulu_face::SetTrackTentativeAge(3);
    ulu_face::SetFaceDetectThreshold(0.5); // default = 0.5
    ulu_face::SetFacePedestrianThreshold(0.5); // default = 0.4
    
    ulu_face::EReturn_Code ret = CreatePerson(&person_);
    if (ret != ulu_face::E_OK) {
        errorf("CreatePerson failed, ret:%d\n", ret);
        return false;
    }

    person_->SetEScene(ulu_face::ES_GE60DegreeObliqueStore, true);
    person_->SetPersonDetectAlgorithm(ulu_face::E_PDA_1); 

    std::string near_detect_model_full_path = "/app/fs/models/detect/FaceDetect_shuyi.rknn";
    size_t near_detect_model_size = 0;
    std::shared_ptr<unsigned char> model_data = read_model_2_buff(near_detect_model_full_path.c_str(), near_detect_model_size);
    tracef("detect_model_size = %d\n", near_detect_model_size);

    ret = person_->Init(ulu_face::E_MT_FaceDetect, model_data.get(), near_detect_model_size);
    if (ret != ulu_face::E_OK) {
        errorf("ulu_fs Init face detect fail, ret = %d\n", ret);
        DestroyPerson(&person_);
    }
    person_->SetModule(ulu_face::EPF_PedestrianDetect, true);

    infof("init alg version: %s succeed\n", alg_sdk_version);
    return true;
}

void OacClientTest::run() {
    infof("start oac client test thread\n");
    while (running()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        oac::ImageFrame image;
        oac_client_->getImageFrame(image);
        infof("oac client use image index:%d, pts:%lld\n", image.index, image.timestamp);

        ulu_face::SULUImage cv_img;
        cv_img.pixel_format = ulu_face::EPIX_FMT_RGB888;
        cv_img.width = image.width;
        cv_img.height = image.height;
        cv_img.vir_addrs[0]= (ulu_face::ulu_uchat_t*)image.data;
        cv_img.data_sizes[0] = image.size;

        std::vector<ulu_face::SPersonInfo> out_current_info;

        tracef("alg update+++\n");
        person_->Update(cv_img, out_current_info);
        tracef("alg update---\n");
        
        //std::this_thread::sleep_for(std::chrono::milliseconds(20));

        oac_client_->releaseImageFrame(image);

    }

}