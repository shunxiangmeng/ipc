/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  rkImage.h
 * Author      :  mengshunxiang 
 * Data        :  2024-07-06 10:45:01
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include "hal/Image.h"
#include "rkMedia.h"

namespace hal {
class rkImage : public IImage {
    rkImage() = default;
    virtual ~rkImage() = default;
public:
    static rkImage* instance();

    virtual bool setInputFramerate(int32_t channel, uint32_t fps) override;
    virtual int getInputFramerate(int32_t channel) override;

};
}