// 杂项函数
#pragma once
#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include "Message2Clients.pb.h"
#include "Message2Server.pb.h"
#include "MessageType.pb.h"

#include "structures.h"

namespace AssistFunction
{

    constexpr int numOfGridPerCell = 100;

    [[nodiscard]] constexpr inline int GridToCell(int grid) noexcept
    {
        return grid / numOfGridPerCell;
    }

    inline bool HaveView(int viewRange, int x, int y, int newX, int newY, const std::vector<std::vector<THUAI6::PlaceType>>& map)
    {
        int deltaX = newX - x;
        int deltaY = newY - y;
        double distance = deltaX * deltaX + deltaY * deltaY;
        if (distance < viewRange * viewRange)
        {
            int divide = std::max(std::abs(deltaX), std::abs(deltaY)) / 100;
            double dx = deltaX / divide;
            double dy = deltaY / divide;
            double myX = double(x);
            double myY = double(y);
            for (int i = 0; i < divide; i++)
            {
                myX += dx;
                myY += dy;
                if (map[GridToCell(myX)][GridToCell(myY)] != THUAI6::PlaceType::Land)
                    return false;
            }
            return true;
        }
        else
            return false;
    }
}  // namespace AssistFunction

// 辅助函数，用于将proto信息转换为THUAI6信息
namespace Proto2THUAI6
{
    // 用于将Protobuf中的枚举转换为THUAI6的枚举
    inline std::map<protobuf::PlaceType, THUAI6::PlaceType> placeTypeDict{
        {protobuf::PlaceType::NULL_PLACE_TYPE, THUAI6::PlaceType::NullPlaceType},
        {protobuf::PlaceType::LAND, THUAI6::PlaceType::Land},
        {protobuf::PlaceType::WALL, THUAI6::PlaceType::Wall},
        {protobuf::PlaceType::GRASS, THUAI6::PlaceType::Grass},
        {protobuf::PlaceType::CLASSROOM, THUAI6::PlaceType::ClassRoom},
        {protobuf::PlaceType::GATE, THUAI6::PlaceType::Gate},
        {protobuf::PlaceType::HIDDEN_GATE, THUAI6::PlaceType::HiddenGate},
    };

    inline std::map<protobuf::ShapeType, THUAI6::ShapeType> shapeTypeDict{
        {protobuf::ShapeType::NULL_SHAPE_TYPE, THUAI6::ShapeType::NullShapeType},
        {protobuf::ShapeType::CIRCLE, THUAI6::ShapeType::Circle},
        {protobuf::ShapeType::SQUARE, THUAI6::ShapeType::Square},
    };

    inline std::map<protobuf::PropType, THUAI6::PropType> propTypeDict{
        {protobuf::PropType::NULL_PROP_TYPE, THUAI6::PropType::NullPropType},
        {protobuf::PropType::PTYPE1, THUAI6::PropType::PropType1},
        {protobuf::PropType::PTYPE2, THUAI6::PropType::PropType2},
        {protobuf::PropType::PTYPE3, THUAI6::PropType::PropType3},
        {protobuf::PropType::PTYPE4, THUAI6::PropType::PropType4},
    };

    inline std::map<protobuf::PlayerType, THUAI6::PlayerType> playerTypeDict{
        {protobuf::PlayerType::NULL_PLAYER_TYPE, THUAI6::PlayerType::NullPlayerType},
        {protobuf::PlayerType::STUDENT_PLAYER, THUAI6::PlayerType::StudentPlayer},
        {protobuf::PlayerType::TRICKER_PLAYER, THUAI6::PlayerType::TrickerPlayer},
    };

    inline std::map<protobuf::StudentType, THUAI6::StudentType> studentTypeDict{
        {protobuf::StudentType::NULL_STUDENT_TYPE, THUAI6::StudentType::NullStudentType},
        {protobuf::StudentType::STUDENTTYPE1, THUAI6::StudentType::StudentType1},
        {protobuf::StudentType::STUDENTTYPE2, THUAI6::StudentType::StudentType2},
        {protobuf::StudentType::STUDENTTYPE3, THUAI6::StudentType::StudentType3},
        {protobuf::StudentType::STUDENTTYPE4, THUAI6::StudentType::StudentType4},
    };

    inline std::map<protobuf::TrickerType, THUAI6::TrickerType> trickerTypeDict{
        {protobuf::TrickerType::NULL_TRICKER_TYPE, THUAI6::TrickerType::NullTrickerType},
        {protobuf::TrickerType::TRICKERTYPE1, THUAI6::TrickerType::TrickerType1},
        {protobuf::TrickerType::TRICKERTYPE2, THUAI6::TrickerType::TrickerType2},
        {protobuf::TrickerType::TRICKERTYPE3, THUAI6::TrickerType::TrickerType3},
        {protobuf::TrickerType::TRICKERTYPE4, THUAI6::TrickerType::TrickerType4},
    };

    inline std::map<protobuf::StudentBuffType, THUAI6::StudentBuffType> studentBuffTypeDict{
        {protobuf::StudentBuffType::NULL_SBUFF_TYPE, THUAI6::StudentBuffType::NullStudentBuffType},
        {protobuf::StudentBuffType::SBUFFTYPE1, THUAI6::StudentBuffType::StudentBuffType1},
        {protobuf::StudentBuffType::SBUFFTYPE2, THUAI6::StudentBuffType::StudentBuffType2},
        {protobuf::StudentBuffType::SBUFFTYPE3, THUAI6::StudentBuffType::StudentBuffType3},
        {protobuf::StudentBuffType::SBUFFTYPE4, THUAI6::StudentBuffType::StudentBuffType4},
    };

    inline std::map<protobuf::TrickerBuffType, THUAI6::TrickerBuffType> trickerBuffTypeDict{
        {protobuf::TrickerBuffType::NULL_TBUFF_TYPE, THUAI6::TrickerBuffType::NullTrickerBuffType},
        {protobuf::TrickerBuffType::TBUFFTYPE1, THUAI6::TrickerBuffType::TrickerBuffType1},
        {protobuf::TrickerBuffType::TBUFFTYPE2, THUAI6::TrickerBuffType::TrickerBuffType2},
        {protobuf::TrickerBuffType::TBUFFTYPE3, THUAI6::TrickerBuffType::TrickerBuffType3},
        {protobuf::TrickerBuffType::TBUFFTYPE4, THUAI6::TrickerBuffType::TrickerBuffType4},
    };

    inline std::map<protobuf::StudentState, THUAI6::StudentState> studentStateDict{
        {protobuf::StudentState::NULL_STATUS, THUAI6::StudentState::NullStudentState},
        {protobuf::StudentState::IDLE, THUAI6::StudentState::Idle},
        {protobuf::StudentState::LEARNING, THUAI6::StudentState::Learning},
        {protobuf::StudentState::ADDICTED, THUAI6::StudentState::Addicted},
        {protobuf::StudentState::QUIT, THUAI6::StudentState::Quit},
        {protobuf::StudentState::GRADUATED, THUAI6::StudentState::Graduated},
    };

    inline std::map<protobuf::GameState, THUAI6::GameState> gameStateDict{
        {protobuf::GameState::NULL_GAME_STATE, THUAI6::GameState::NullGameState},
        {protobuf::GameState::GAME_START, THUAI6::GameState::GameStart},
        {protobuf::GameState::GAME_RUNNING, THUAI6::GameState::GameRunning},
        {protobuf::GameState::GAME_END, THUAI6::GameState::GameEnd},
    };

    // 用于将Protobuf中的类转换为THUAI6的类
    inline std::shared_ptr<THUAI6::Tricker> Protobuf2THUAI6Tricker(const protobuf::MessageOfTricker& trickerMsg)
    {
        auto tricker = std::make_shared<THUAI6::Tricker>();
        tricker->x = trickerMsg.x();
        tricker->y = trickerMsg.y();
        tricker->speed = trickerMsg.speed();
        tricker->damage = trickerMsg.damage();
        tricker->timeUntilSkillAvailable = trickerMsg.time_until_skill_available();
        tricker->place = placeTypeDict[trickerMsg.place()];
        tricker->prop = propTypeDict[trickerMsg.prop()];
        tricker->trickerType = trickerTypeDict[trickerMsg.tricker_type()];
        tricker->guid = trickerMsg.guid();
        tricker->movable = trickerMsg.movable();
        tricker->playerID = trickerMsg.player_id();
        tricker->viewRange = trickerMsg.view_range();
        tricker->radius = trickerMsg.radius();
        tricker->buff.clear();
        for (int i = 0; i < trickerMsg.buff().size(); i++)
        {
            tricker->buff.push_back(trickerBuffTypeDict[trickerMsg.buff(i)]);
        }
        return tricker;
    }

    inline std::shared_ptr<THUAI6::Student> Protobuf2THUAI6Student(const protobuf::MessageOfStudent& studentMsg)
    {
        auto student = std::make_shared<THUAI6::Student>();
        student->x = studentMsg.x();
        student->y = studentMsg.y();
        student->speed = studentMsg.speed();
        student->viewRange = studentMsg.view_range();
        student->playerID = studentMsg.player_id();
        student->guid = studentMsg.guid();
        student->radius = studentMsg.radius();
        student->timeUntilSkillAvailable = studentMsg.time_until_skill_available();
        student->damage = studentMsg.damage();
        student->playerType = THUAI6::PlayerType::StudentPlayer;
        student->prop = propTypeDict[studentMsg.prop()];
        student->place = placeTypeDict[studentMsg.place()];
        student->state = studentStateDict[studentMsg.state()];
        student->determination = studentMsg.determination();
        student->failNum = studentMsg.fail_num();
        student->failTime = studentMsg.fail_time();
        student->emoTime = studentMsg.emo_time();
        student->studentType = studentTypeDict[studentMsg.student_type()];
        student->buff.clear();
        for (int i = 0; i < studentMsg.buff_size(); i++)
        {
            student->buff.push_back(studentBuffTypeDict[studentMsg.buff(i)]);
        }
        return student;
    }

    inline std::shared_ptr<THUAI6::Prop> Protobuf2THUAI6Prop(const protobuf::MessageOfProp& propMsg)
    {
        auto prop = std::make_shared<THUAI6::Prop>();
        prop->x = propMsg.x();
        prop->y = propMsg.y();
        prop->type = propTypeDict[propMsg.type()];
        prop->place = placeTypeDict[propMsg.place()];
        prop->guid = propMsg.guid();
        prop->size = propMsg.size();
        prop->facingDirection = propMsg.facing_direction();
        prop->isMoving = propMsg.is_moving();
        return prop;
    }

    inline std::vector<std::vector<THUAI6::PlaceType>> Protobuf2THUAI6Map(const protobuf::MessageOfMap& mapMsg)
    {
        std::vector<std::vector<THUAI6::PlaceType>> map;
        for (int i = 0; i < mapMsg.row_size(); i++)
        {
            std::vector<THUAI6::PlaceType> row;
            for (int j = 0; j < mapMsg.row(i).col_size(); j++)
            {
                row.push_back(placeTypeDict[mapMsg.row(i).col(j)]);
            }
            map.push_back(row);
        }
        return map;
    }

}  // namespace Proto2THUAI6

namespace THUAI62Proto
{
    // 用于将THUAI6的枚举转换为Protobuf的枚举
    inline std::map<THUAI6::PlaceType, protobuf::PlaceType> placeTypeDict{
        {THUAI6::PlaceType::NullPlaceType, protobuf::PlaceType::NULL_PLACE_TYPE},
        {THUAI6::PlaceType::Land, protobuf::PlaceType::LAND},
        {THUAI6::PlaceType::Wall, protobuf::PlaceType::WALL},
        {THUAI6::PlaceType::Grass, protobuf::PlaceType::GRASS},
        {THUAI6::PlaceType::ClassRoom, protobuf::PlaceType::CLASSROOM},
        {THUAI6::PlaceType::Gate, protobuf::PlaceType::GATE},
        {THUAI6::PlaceType::HiddenGate, protobuf::PlaceType::HIDDEN_GATE},
    };

    inline std::map<THUAI6::ShapeType, protobuf::ShapeType> shapeTypeDict{
        {THUAI6::ShapeType::NullShapeType, protobuf::ShapeType::NULL_SHAPE_TYPE},
        {THUAI6::ShapeType::Circle, protobuf::ShapeType::CIRCLE},
        {THUAI6::ShapeType::Square, protobuf::ShapeType::SQUARE},
    };

    inline std::map<THUAI6::PropType, protobuf::PropType> propTypeDict{
        {THUAI6::PropType::NullPropType, protobuf::PropType::NULL_PROP_TYPE},
        {THUAI6::PropType::PropType1, protobuf::PropType::PTYPE1},
        {THUAI6::PropType::PropType2, protobuf::PropType::PTYPE2},
        {THUAI6::PropType::PropType3, protobuf::PropType::PTYPE3},
        {THUAI6::PropType::PropType4, protobuf::PropType::PTYPE4},
    };

    inline std::map<THUAI6::PlayerType, protobuf::PlayerType> playerTypeDict{
        {THUAI6::PlayerType::NullPlayerType, protobuf::PlayerType::NULL_PLAYER_TYPE},
        {THUAI6::PlayerType::StudentPlayer, protobuf::PlayerType::STUDENT_PLAYER},
        {THUAI6::PlayerType::TrickerPlayer, protobuf::PlayerType::TRICKER_PLAYER},
    };

    inline std::map<THUAI6::StudentType, protobuf::StudentType> studentTypeDict{
        {THUAI6::StudentType::NullStudentType, protobuf::StudentType::NULL_STUDENT_TYPE},
        {THUAI6::StudentType::StudentType1, protobuf::StudentType::STUDENTTYPE1},
        {THUAI6::StudentType::StudentType2, protobuf::StudentType::STUDENTTYPE2},
        {THUAI6::StudentType::StudentType3, protobuf::StudentType::STUDENTTYPE3},
        {THUAI6::StudentType::StudentType4, protobuf::StudentType::STUDENTTYPE4},
    };

    inline std::map<THUAI6::StudentBuffType, protobuf::StudentBuffType> studentBuffTypeDict{
        {THUAI6::StudentBuffType::NullStudentBuffType, protobuf::StudentBuffType::NULL_SBUFF_TYPE},
        {THUAI6::StudentBuffType::StudentBuffType1, protobuf::StudentBuffType::SBUFFTYPE1},
        {THUAI6::StudentBuffType::StudentBuffType2, protobuf::StudentBuffType::SBUFFTYPE2},
        {THUAI6::StudentBuffType::StudentBuffType3, protobuf::StudentBuffType::SBUFFTYPE3},
        {THUAI6::StudentBuffType::StudentBuffType4, protobuf::StudentBuffType::SBUFFTYPE4},
    };

    inline std::map<THUAI6::TrickerType, protobuf::TrickerType> trickerTypeDict{
        {THUAI6::TrickerType::NullTrickerType, protobuf::TrickerType::NULL_TRICKER_TYPE},
        {THUAI6::TrickerType::TrickerType1, protobuf::TrickerType::TRICKERTYPE1},
        {THUAI6::TrickerType::TrickerType2, protobuf::TrickerType::TRICKERTYPE2},
        {THUAI6::TrickerType::TrickerType3, protobuf::TrickerType::TRICKERTYPE3},
        {THUAI6::TrickerType::TrickerType4, protobuf::TrickerType::TRICKERTYPE4},
    };

    inline std::map<THUAI6::TrickerBuffType, protobuf::TrickerBuffType> trickerBuffTypeDict{
        {THUAI6::TrickerBuffType::NullTrickerBuffType, protobuf::TrickerBuffType::NULL_TBUFF_TYPE},
        {THUAI6::TrickerBuffType::TrickerBuffType1, protobuf::TrickerBuffType::TBUFFTYPE1},
        {THUAI6::TrickerBuffType::TrickerBuffType2, protobuf::TrickerBuffType::TBUFFTYPE2},
        {THUAI6::TrickerBuffType::TrickerBuffType3, protobuf::TrickerBuffType::TBUFFTYPE3},
        {THUAI6::TrickerBuffType::TrickerBuffType4, protobuf::TrickerBuffType::TBUFFTYPE4},
    };

    // 用于将THUAI6的类转换为Protobuf的消息
    inline protobuf::PlayerMsg THUAI62ProtobufPlayer(int64_t playerID, THUAI6::PlayerType playerType, THUAI6::StudentType studentType, THUAI6::TrickerType trickerType)
    {
        protobuf::PlayerMsg playerMsg;
        playerMsg.set_player_id(playerID);
        playerMsg.set_player_type(playerTypeDict[playerType]);
        if (playerType == THUAI6::PlayerType::StudentPlayer)
        {
            playerMsg.set_student_type(studentTypeDict[studentType]);
        }
        else if (playerType == THUAI6::PlayerType::TrickerPlayer)
        {
            playerMsg.set_tricker_type(trickerTypeDict[trickerType]);
        }
        return playerMsg;
    }

    inline protobuf::IDMsg THUAI62ProtobufID(int playerID)
    {
        protobuf::IDMsg idMsg;
        idMsg.set_player_id(playerID);
        return idMsg;
    }

    inline protobuf::MoveMsg THUAI62ProtobufMove(int64_t time, double angle, int64_t id)
    {
        protobuf::MoveMsg moveMsg;
        moveMsg.set_time_in_milliseconds(time);
        moveMsg.set_angle(angle);
        moveMsg.set_player_id(id);
        return moveMsg;
    }

    inline protobuf::PickMsg THUAI62ProtobufPick(THUAI6::PropType prop, int64_t id)
    {
        protobuf::PickMsg pickMsg;
        pickMsg.set_prop_type(propTypeDict[prop]);
        pickMsg.set_player_id(id);
        return pickMsg;
    }

    inline protobuf::SendMsg THUAI62ProtobufSend(std::string msg, int64_t toID, int64_t id)
    {
        protobuf::SendMsg sendMsg;
        sendMsg.set_message(msg);
        sendMsg.set_to_player_id(toID);
        sendMsg.set_player_id(id);
        return sendMsg;
    }

    inline protobuf::TrickMsg THUAI62ProtobufTrick(double angle, int64_t id)
    {
        protobuf::TrickMsg trickMsg;
        trickMsg.set_angle(angle);
        trickMsg.set_player_id(id);
        return trickMsg;
    }
}  // namespace THUAI62Proto

namespace Time
{
    inline double TimeSinceStart(const std::chrono::system_clock::time_point& sp)
    {
        std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
        std::chrono::duration<double, std::milli> time_span = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(tp - sp);
        return time_span.count();
    }
}  // namespace Time

#endif
