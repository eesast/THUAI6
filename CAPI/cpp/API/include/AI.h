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

using CreateAIFunc = std::unique_ptr<IAI> (*)(int64_t playerID);

class AI : public IAI
{
public:
    AI(int64_t pID) :
        IAI(),
        playerID(pID)
    {
    }
    void play(IStudentAPI& api) override;
    void play(ITrickerAPI& api) override;

private:
    int64_t playerID;
};

#endif
