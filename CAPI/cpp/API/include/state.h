#pragma once
#ifndef STATE_H
#define STATE_H

#include <vector>
#include <array>
#include <map>
#include <memory>

#include "structures.h"

// 存储场上的状态
struct State
{
    int32_t teamScore;

    // 自身信息，根据playerType的不同，可以调用的值也不同。
    std::shared_ptr<THUAI6::Student> studentSelf;
    std::shared_ptr<THUAI6::Tricker> trickerSelf;

    std::vector<std::shared_ptr<THUAI6::Student>> students;
    std::vector<std::shared_ptr<THUAI6::Tricker>> trickers;
    std::vector<std::shared_ptr<THUAI6::Prop>> props;

    std::vector<std::shared_ptr<THUAI6::Bullet>> bullets;

    std::vector<std::shared_ptr<THUAI6::BombedBullet>> bombedBullets;

    std::vector<std::vector<THUAI6::PlaceType>> gameMap;

    std::shared_ptr<THUAI6::GameMap> mapInfo;

    std::shared_ptr<THUAI6::GameInfo> gameInfo;

    std::vector<int64_t> guids;
};

#endif
