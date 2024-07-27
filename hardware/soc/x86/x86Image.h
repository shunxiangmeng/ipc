/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  x86Image.h
 * Author      :  mengshunxiang 
 * Data        :  2024-07-27 15:30:37
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include "hal/Image.h"

namespace hal {
class x86Image : public IImage {
    x86Image() = default;
    virtual ~x86Image() = default;
public:
    static x86Image* instance();

    virtual bool setInputFramerate(int32_t channel, uint32_t fps) override;
    virtual int getInputFramerate(int32_t channel)  override;
};
}