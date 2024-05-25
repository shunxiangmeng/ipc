/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  NearPassengerFlow.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-05-25 16:42:15
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "NearPassengerFlow.h"
#include "infra/include/Logger.h"

NearPassengerFlow::NearPassengerFlow() {
}

NearPassengerFlow::~NearPassengerFlow() {
}

bool NearPassengerFlow::init() {
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
}