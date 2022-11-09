// 杂项函数
#pragma once
#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include "Message2Clients.pb.h"
#include "Message2Server.pb.h"
#include "MessageType.pb.h"

#include "structures.h"

// 辅助函数，用于将proto信息转换为THUAI6信息
namespace Proto2THUAI6
{
    // 用于将Protobuf中的枚举转换为THUAI6的枚举
    inline std::map<protobuf::PlaceType, THUAI6::PlaceType> placeTypeDict{
        {protobuf::PlaceType::NULL_PLACE_TYPE, THUAI6::PlaceType::NullPlaceType},
        {protobuf::PlaceType::LAND, THUAI6::PlaceType::Land},
        {protobuf::PlaceType::WALL, THUAI6::PlaceType::Wall},
        {protobuf::PlaceType::GRASS, THUAI6::PlaceType::Grass},
        {protobuf::PlaceType::MACHINE, THUAI6::PlaceType::Machine},
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
        {protobuf::PlayerType::HUMAN_PLAYER, THUAI6::PlayerType::HumanPlayer},
        {protobuf::PlayerType::BUTCHER_PLAYER, THUAI6::PlayerType::ButcherPlayer},
    };

    inline std::map<protobuf::HumanType, THUAI6::HumanType> humanTypeDict{
        {protobuf::HumanType::NULL_HUMAN_TYPE, THUAI6::HumanType::NullHumanType},
        {protobuf::HumanType::HUMANTYPE1, THUAI6::HumanType::HumanType1},
        {protobuf::HumanType::HUMANTYPE2, THUAI6::HumanType::HumanType2},
        {protobuf::HumanType::HUMANTYPE3, THUAI6::HumanType::HumanType3},
        {protobuf::HumanType::HUMANTYPE4, THUAI6::HumanType::HumanType4},
    };

    inline std::map<protobuf::ButcherType, THUAI6::ButcherType> butcherTypeDict{
        {protobuf::ButcherType::NULL_BUTCHER_TYPE, THUAI6::ButcherType::NullButcherType},
        {protobuf::ButcherType::BUTCHERTYPE1, THUAI6::ButcherType::ButcherType1},
        {protobuf::ButcherType::BUTCHERTYPE2, THUAI6::ButcherType::ButcherType2},
        {protobuf::ButcherType::BUTCHERTYPE3, THUAI6::ButcherType::ButcherType3},
        {protobuf::ButcherType::BUTCHERTYPE4, THUAI6::ButcherType::ButcherType4},
    };

    inline std::map<protobuf::HumanBuffType, THUAI6::HumanBuffType> humanBuffTypeDict{
        {protobuf::HumanBuffType::NULL_HBUFF_TYPE, THUAI6::HumanBuffType::NullHumanBuffType},
        {protobuf::HumanBuffType::HBUFFTYPE1, THUAI6::HumanBuffType::HumanBuffType1},
        {protobuf::HumanBuffType::HBUFFTYPE2, THUAI6::HumanBuffType::HumanBuffType2},
        {protobuf::HumanBuffType::HBUFFTYPE3, THUAI6::HumanBuffType::HumanBuffType3},
        {protobuf::HumanBuffType::HBUFFTYPE4, THUAI6::HumanBuffType::HumanBuffType4},
    };

    inline std::map<protobuf::ButcherBuffType, THUAI6::ButcherBuffType> butcherBuffTypeDict{
        {protobuf::ButcherBuffType::NULL_BBUFF_TYPE, THUAI6::ButcherBuffType::NullButcherBuffType},
        {protobuf::ButcherBuffType::BBUFFTYPE1, THUAI6::ButcherBuffType::ButcherBuffType1},
        {protobuf::ButcherBuffType::BBUFFTYPE2, THUAI6::ButcherBuffType::ButcherBuffType2},
        {protobuf::ButcherBuffType::BBUFFTYPE3, THUAI6::ButcherBuffType::ButcherBuffType3},
        {protobuf::ButcherBuffType::BBUFFTYPE4, THUAI6::ButcherBuffType::ButcherBuffType4},
    };

    // 用于将Protobuf中的类转换为THUAI6的类
    inline std::shared_ptr<THUAI6::Butcher> Protobuf2THUAI6Butcher(const protobuf::MessageOfButcher& butcherMsg)
    {
        auto butcher = std::make_shared<THUAI6::Butcher>();
        butcher->x = butcherMsg.x();
        butcher->y = butcherMsg.y();
        butcher->speed = butcherMsg.speed();
        butcher->damage = butcherMsg.damage();
        butcher->timeUntilSkillAvailable = butcherMsg.time_until_skill_available();
        butcher->place = placeTypeDict[butcherMsg.place()];
        butcher->prop = propTypeDict[butcherMsg.prop()];
        butcher->butcherType = butcherTypeDict[butcherMsg.butcher_type()];
        butcher->guid = butcherMsg.guid();
        butcher->movable = butcherMsg.movable();
        butcher->playerID = butcherMsg.player_id();
        butcher->viewRange = butcherMsg.view_range();
        butcher->radius = butcherMsg.radius();
        butcher->buff.clear();
        for (int i = 0; i < butcherMsg.buff().size(); i++)
        {
            butcher->buff.push_back(butcherBuffTypeDict[butcherMsg.buff(i)]);
        }
        return butcher;
    }

    inline std::shared_ptr<THUAI6::Human> Protobuf2THUAI6Human(const protobuf::MessageOfHuman& humanMsg)
    {
        auto human = std::make_shared<THUAI6::Human>();
        human->x = humanMsg.x();
        human->y = humanMsg.y();
        human->speed = humanMsg.speed();
        human->viewRange = humanMsg.view_range();
        human->playerID = humanMsg.player_id();
        human->guid = humanMsg.guid();
        human->radius = humanMsg.radius();
        human->timeUntilSkillAvailable = humanMsg.time_until_skill_available();
        human->playerType = THUAI6::PlayerType::HumanPlayer;
        human->prop = propTypeDict[humanMsg.prop()];
        human->place = placeTypeDict[humanMsg.place()];
        human->onChair = humanMsg.on_chair();
        human->onGround = humanMsg.on_ground();
        human->life = humanMsg.life();
        human->hangedTime = humanMsg.hanged_time();
        human->humanType = humanTypeDict[humanMsg.human_type()];
        human->buff.clear();
        for (int i = 0; i < humanMsg.buff_size(); i++)
        {
            human->buff.push_back(humanBuffTypeDict[humanMsg.buff(i)]);
        }
        return human;
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
        {THUAI6::PlaceType::Machine, protobuf::PlaceType::MACHINE},
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
        {THUAI6::PlayerType::HumanPlayer, protobuf::PlayerType::HUMAN_PLAYER},
        {THUAI6::PlayerType::ButcherPlayer, protobuf::PlayerType::BUTCHER_PLAYER},
    };

    inline std::map<THUAI6::HumanType, protobuf::HumanType> humanTypeDict{
        {THUAI6::HumanType::NullHumanType, protobuf::HumanType::NULL_HUMAN_TYPE},
        {THUAI6::HumanType::HumanType1, protobuf::HumanType::HUMANTYPE1},
        {THUAI6::HumanType::HumanType2, protobuf::HumanType::HUMANTYPE2},
        {THUAI6::HumanType::HumanType3, protobuf::HumanType::HUMANTYPE3},
        {THUAI6::HumanType::HumanType4, protobuf::HumanType::HUMANTYPE4},
    };

    inline std::map<THUAI6::HumanBuffType, protobuf::HumanBuffType> humanBuffTypeDict{
        {THUAI6::HumanBuffType::NullHumanBuffType, protobuf::HumanBuffType::NULL_HBUFF_TYPE},
        {THUAI6::HumanBuffType::HumanBuffType1, protobuf::HumanBuffType::HBUFFTYPE1},
        {THUAI6::HumanBuffType::HumanBuffType2, protobuf::HumanBuffType::HBUFFTYPE2},
        {THUAI6::HumanBuffType::HumanBuffType3, protobuf::HumanBuffType::HBUFFTYPE3},
        {THUAI6::HumanBuffType::HumanBuffType4, protobuf::HumanBuffType::HBUFFTYPE4},
    };

    inline std::map<THUAI6::ButcherType, protobuf::ButcherType> butcherTypeDict{
        {THUAI6::ButcherType::NullButcherType, protobuf::ButcherType::NULL_BUTCHER_TYPE},
        {THUAI6::ButcherType::ButcherType1, protobuf::ButcherType::BUTCHERTYPE1},
        {THUAI6::ButcherType::ButcherType2, protobuf::ButcherType::BUTCHERTYPE2},
        {THUAI6::ButcherType::ButcherType3, protobuf::ButcherType::BUTCHERTYPE3},
        {THUAI6::ButcherType::ButcherType4, protobuf::ButcherType::BUTCHERTYPE4},
    };

    inline std::map<THUAI6::ButcherBuffType, protobuf::ButcherBuffType> butcherBuffTypeDict{
        {THUAI6::ButcherBuffType::NullButcherBuffType, protobuf::ButcherBuffType::NULL_BBUFF_TYPE},
        {THUAI6::ButcherBuffType::ButcherBuffType1, protobuf::ButcherBuffType::BBUFFTYPE1},
        {THUAI6::ButcherBuffType::ButcherBuffType2, protobuf::ButcherBuffType::BBUFFTYPE2},
        {THUAI6::ButcherBuffType::ButcherBuffType3, protobuf::ButcherBuffType::BBUFFTYPE3},
        {THUAI6::ButcherBuffType::ButcherBuffType4, protobuf::ButcherBuffType::BBUFFTYPE4},
    };

    // 用于将THUAI6的类转换为Protobuf的消息
    inline protobuf::PlayerMsg THUAI62ProtobufPlayer(int playerID, THUAI6::PlayerType playerType, THUAI6::HumanType humanType, THUAI6::ButcherType butcherType)
    {
        protobuf::PlayerMsg playerMsg;
        playerMsg.set_player_id(playerID);
        playerMsg.set_player_type(playerTypeDict[playerType]);
        if (playerType == THUAI6::PlayerType::HumanPlayer)
        {
            playerMsg.set_human_type(humanTypeDict[humanType]);
        }
        else if (playerType == THUAI6::PlayerType::ButcherPlayer)
        {
            playerMsg.set_butcher_type(butcherTypeDict[butcherType]);
        }
        return playerMsg;
    }
    inline protobuf::IDMsg THUAI62ProtobufID(int playerID)
    {
        protobuf::IDMsg idMsg;
        idMsg.set_player_id(playerID);
        return idMsg;
    }
}  // namespace THUAI62Proto

#endif