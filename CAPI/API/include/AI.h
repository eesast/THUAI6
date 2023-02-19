#pragma once
#ifndef AI_H
#define AI_H

#include "API.h"

class IAI
{
public:
    virtual ~IAI() = default;
    IAI() = default;
    virtual void play(IHumanAPI& api) = 0;
    virtual void play(IButcherAPI& api) = 0;
};

using CreateAIFunc = std::unique_ptr<IAI> (*)();

class AI : public IAI
{
public:
    AI() :
        IAI()
    {
    }
    void play(IHumanAPI& api) override;
    void play(IButcherAPI& api) override;
};

#endif
