/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  X86Video.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-05-14 23:12:58
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "x86Video.h"

IVideo* x86Video::instance() {
    static x86Video s_video;
    return &s_video;
}

x86Video::x86Video() : init_(false) {
}

bool x86Video::initial(const int config) {
    if (!init_) {
        init_ = true;
        //std::string filename = "F:\\mp4\\The.Teacher.2022.HD1080P.X264.AAC.Malayalam.CHS.BDYS.mp4";
        std::string filename = "/home/shawn/test.mp4";
        //std::string filename = "F:\\mp4\\HWZ.2022.EP01.HD1080P.X264.AAC.Mandarin.CHS.BDYS.mp4";
        //std::string filename = "E:\\sample.mp4";
        mp4_reader_.open(filename);
        Thread::start();
    }
    return true;
}

bool x86Video::deInitial() {
    return false;
}

