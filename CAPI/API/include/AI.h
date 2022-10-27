#pragma once
#ifndef AI_H
#define AI_H

#include "API.h"

// 暂定版本：Human和Butcher全部继承AI类，
class IAI
{
public:
    IAI()
    {
    }
    virtual void play(IAPI& api) = 0;
};

class AI : public IAI
{
public:
    AI() :
        IAI()
    {
    }
    void play(IAPI& api) override;
};

#endif