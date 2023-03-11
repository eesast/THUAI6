#pragma once
#ifndef AI_H
#define AI_H

#include "API.h"

class IAI
{
public:
    virtual ~IAI() = default;
    IAI() = default;
    virtual void play(IStudentAPI& api) = 0;
    virtual void play(ITrickerAPI& api) = 0;
};

using CreateAIFunc = std::unique_ptr<IAI> (*)();

class AI : public IAI
{
public:
    AI() :
        IAI()
    {
    }
    void play(IStudentAPI& api) override;
    void play(ITrickerAPI& api) override;
};

#endif
