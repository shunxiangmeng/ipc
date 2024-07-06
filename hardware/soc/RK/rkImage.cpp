/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  rkImage.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-07-06 10:44:56
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "rkImage.h"

namespace hal {

IImage* IImage::instance() {
    return rkImage::instance();
}
rkImage* rkImage::instance() {
    static rkImage s_image;
    return &s_image;
}  

bool rkImage::setInputFramerate(int32_t channel, uint32_t fps) {
    return rk_mpi_isp_set_framerate(channel, fps);
}

int rkImage::getInputFramerate(int32_t channel) {
    return rk_mpi_isp_get_framerate(channel);
}

}