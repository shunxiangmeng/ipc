/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  x86Image.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-07-27 15:30:44
 * Description :  None
 * Note        : 
 ************************************************************************/
#include "x86Image.h"

namespace hal {

IImage* IImage::instance() {
    return x86Image::instance();
}
x86Image* x86Image::instance() {
    static x86Image s_image;
    return &s_image;
}  

bool x86Image::setInputFramerate(int32_t channel, uint32_t fps) {
    return false;
}

int x86Image::getInputFramerate(int32_t channel) {
    return 0;
}

}