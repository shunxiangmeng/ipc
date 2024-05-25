/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  alg.h
 * Author      :  mengshunxiang 
 * Data        :  2024-05-25 16:37:37
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include "Uluai.h"

#include "ulucu/include/IULUPose.h"
#include "ulucu/include/IULUPerson.h"
#include "ulucu/include/IULUBaseType.h"
#include "ulucu/include/IULUFace.h"

class NearPassengerFlow : public IUluai {
public:
    NearPassengerFlow();
    virtual ~NearPassengerFlow();

    virtual bool init() override;

private:
    ulu_face::IULUPerson *person_ = nullptr;
};