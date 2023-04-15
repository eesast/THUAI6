// 杂项函数
#pragma once
#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <cmath>
#include <map>
#include <vector>
#include "Message2Clients.pb.h"
#include "Message2Server.pb.h"
#include "MessageType.pb.h"

#include "structures.h"

namespace AssistFunction
{

    constexpr int numOfGridPerCell = 1000;

    [[nodiscard]] constexpr inline int GridToCell(int grid) noexcept
    {
        return grid / numOfGridPerCell;
    }

    [[nodiscard]] constexpr inline int GridToCell(double grid) noexcept
    {
        return int(grid) / numOfGridPerCell;
    }

    inline bool HaveView(int viewRange, int x, int y, int newX, int newY, std::vector<std::vector<THUAI6::PlaceType>>& map)
    {
        int deltaX = newX - x;
        int deltaY = newY - y;
        double distance = deltaX * deltaX + deltaY * deltaY;
        THUAI6::PlaceType myPlace = map[GridToCell(x)][GridToCell(y)];
        THUAI6::PlaceType newPlace = map[GridToCell(newX)][GridToCell(newY)];
        if (newPlace == THUAI6::PlaceType::Grass && myPlace != THUAI6::PlaceType::Grass)  // 草丛外必不可能看到草丛内
            return false;
        if (distance < viewRange * viewRange)
        {
            int divide = std::max(std::abs(deltaX), std::abs(deltaY)) / 100;
            if (divide == 0)
                return true;
            double dx = deltaX / divide;
            double dy = deltaY / divide;
            double myX = double(x);
            double myY = double(y);
            if (newPlace == THUAI6::PlaceType::Grass && myPlace == THUAI6::PlaceType::Grass)  // 都在草丛内，要另作判断
                for (int i = 0; i < divide; i++)
                {
                    myX += dx;
                    myY += dy;
                    if (map[GridToCell(myX)][GridToCell(myY)] != THUAI6::PlaceType::Grass)
                        return false;
                }
            else  // 不在草丛内，只需要没有墙即可
                for (int i = 0; i < divide; i++)
                {
                    myX += dx;
                    myY += dy;
                    if (map[GridToCell(myX)][GridToCell(myY)] == THUAI6::PlaceType::Wall)
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
        {protobuf::PlaceType::WINDOW, THUAI6::PlaceType::Window},
        {protobuf::PlaceType::DOOR3, THUAI6::PlaceType::Door3},
        {protobuf::PlaceType::DOOR5, THUAI6::PlaceType::Door5},
        {protobuf::PlaceType::DOOR6, THUAI6::PlaceType::Door6},
        {protobuf::PlaceType::CHEST, THUAI6::PlaceType::Chest},
    };

    inline std::map<protobuf::ShapeType, THUAI6::ShapeType> shapeTypeDict{
        {protobuf::ShapeType::NULL_SHAPE_TYPE, THUAI6::ShapeType::NullShapeType},
        {protobuf::ShapeType::CIRCLE, THUAI6::ShapeType::Circle},
        {protobuf::ShapeType::SQUARE, THUAI6::ShapeType::Square},
    };

    inline std::map<protobuf::PropType, THUAI6::PropType> propTypeDict{
        {protobuf::PropType::NULL_PROP_TYPE, THUAI6::PropType::NullPropType},
        {protobuf::PropType::KEY3, THUAI6::PropType::Key3},
        {protobuf::PropType::KEY5, THUAI6::PropType::Key5},
        {protobuf::PropType::KEY6, THUAI6::PropType::Key6},
        {protobuf::PropType::ADD_SPEED, THUAI6::PropType::AddSpeed},
        {protobuf::PropType::ADD_HP_OR_AP, THUAI6::PropType::AddHpOrAp},
        {protobuf::PropType::ADD_LIFE_OR_CLAIRAUDIENCE, THUAI6::PropType::AddLifeOrClairaudience},
        {protobuf::PropType::SHIELD_OR_SPEAR, THUAI6::PropType::ShieldOrSpear},
        {protobuf::PropType::RECOVERY_FROM_DIZZINESS, THUAI6::PropType::RecoveryFromDizziness},
    };

    inline std::map<protobuf::PlayerType, THUAI6::PlayerType> playerTypeDict{
        {protobuf::PlayerType::NULL_PLAYER_TYPE, THUAI6::PlayerType::NullPlayerType},
        {protobuf::PlayerType::STUDENT_PLAYER, THUAI6::PlayerType::StudentPlayer},
        {protobuf::PlayerType::TRICKER_PLAYER, THUAI6::PlayerType::TrickerPlayer},
    };

    inline std::map<protobuf::StudentType, THUAI6::StudentType> studentTypeDict{
        {protobuf::StudentType::NULL_STUDENT_TYPE, THUAI6::StudentType::NullStudentType},
        {protobuf::StudentType::ATHLETE, THUAI6::StudentType::Athlete},
        {protobuf::StudentType::TEACHER, THUAI6::StudentType::Teacher},
        {protobuf::StudentType::STRAIGHT_A_STUDENT, THUAI6::StudentType::StraightAStudent},
        {protobuf::StudentType::ROBOT, THUAI6::StudentType::Robot},
        {protobuf::StudentType::TECH_OTAKU, THUAI6::StudentType::TechOtaku},
        {protobuf::StudentType::SUNSHINE, THUAI6::StudentType::Sunshine},
    };

    inline std::map<protobuf::TrickerType, THUAI6::TrickerType> trickerTypeDict{
        {protobuf::TrickerType::NULL_TRICKER_TYPE, THUAI6::TrickerType::NullTrickerType},
        {protobuf::TrickerType::ASSASSIN, THUAI6::TrickerType::Assassin},
        {protobuf::TrickerType::KLEE, THUAI6::TrickerType::Klee},
        {protobuf::TrickerType::A_NOISY_PERSON, THUAI6::TrickerType::ANoisyPerson},
        {protobuf::TrickerType::IDOL, THUAI6::TrickerType::Idol},
    };

    inline std::map<protobuf::StudentBuffType, THUAI6::StudentBuffType> studentBuffTypeDict{
        {protobuf::StudentBuffType::NULL_SBUFF_TYPE, THUAI6::StudentBuffType::NullStudentBuffType},
        {protobuf::StudentBuffType::STUDENT_ADD_SPEED, THUAI6::StudentBuffType::AddSpeed},
        {protobuf::StudentBuffType::ADD_LIFE, THUAI6::StudentBuffType::AddLife},
        {protobuf::StudentBuffType::SHIELD, THUAI6::StudentBuffType::Shield},
        {protobuf::StudentBuffType::STUDENT_INVISIBLE, THUAI6::StudentBuffType::Invisible},
    };

    inline std::map<protobuf::TrickerBuffType, THUAI6::TrickerBuffType> trickerBuffTypeDict{
        {protobuf::TrickerBuffType::NULL_TBUFF_TYPE, THUAI6::TrickerBuffType::NullTrickerBuffType},
        {protobuf::TrickerBuffType::TRICKER_ADD_SPEED, THUAI6::TrickerBuffType::AddSpeed},
        {protobuf::TrickerBuffType::SPEAR, THUAI6::TrickerBuffType::Spear},
        {protobuf::TrickerBuffType::ADD_AP, THUAI6::TrickerBuffType::AddAp},
        {protobuf::TrickerBuffType::CLAIRAUDIENCE, THUAI6::TrickerBuffType::Clairaudience},
        {protobuf::TrickerBuffType::TRICKER_INVISIBLE, THUAI6::TrickerBuffType::Invisible},
    };

    inline std::map<protobuf::PlayerState, THUAI6::PlayerState> playerStateDict{
        {protobuf::PlayerState::NULL_STATUS, THUAI6::PlayerState::NullState},
        {protobuf::PlayerState::IDLE, THUAI6::PlayerState::Idle},
        {protobuf::PlayerState::LEARNING, THUAI6::PlayerState::Learning},
        {protobuf::PlayerState::ADDICTED, THUAI6::PlayerState::Addicted},
        {protobuf::PlayerState::QUIT, THUAI6::PlayerState::Quit},
        {protobuf::PlayerState::GRADUATED, THUAI6::PlayerState::Graduated},
        {protobuf::PlayerState::RESCUED, THUAI6::PlayerState::Roused},
        {protobuf::PlayerState::TREATED, THUAI6::PlayerState::Encouraged},
        {protobuf::PlayerState::STUNNED, THUAI6::PlayerState::Stunned},
        {protobuf::PlayerState::RESCUING, THUAI6::PlayerState::Rousing},
        {protobuf::PlayerState::TREATING, THUAI6::PlayerState::Encouraging},
        {protobuf::PlayerState::SWINGING, THUAI6::PlayerState::Swinging},
        {protobuf::PlayerState::ATTACKING, THUAI6::PlayerState::Attacking},
        {protobuf::PlayerState::LOCKING, THUAI6::PlayerState::Locking},
        // {protobuf::PlayerState::RUMMAGING, THUAI6::PlayerState::Rummaging},
        {protobuf::PlayerState::CLIMBING, THUAI6::PlayerState::Climbing},
        {protobuf::PlayerState::OPENING_A_CHEST, THUAI6::PlayerState::OpeningAChest},
        {protobuf::PlayerState::USING_SPECIAL_SKILL, THUAI6::PlayerState::UsingSpecialSkill},
        {protobuf::PlayerState::OPENING_A_GATE, THUAI6::PlayerState::OpeningAGate},
    };

    inline std::map<protobuf::GameState, THUAI6::GameState> gameStateDict{
        {protobuf::GameState::NULL_GAME_STATE, THUAI6::GameState::NullGameState},
        {protobuf::GameState::GAME_START, THUAI6::GameState::GameStart},
        {protobuf::GameState::GAME_RUNNING, THUAI6::GameState::GameRunning},
        {protobuf::GameState::GAME_END, THUAI6::GameState::GameEnd},
    };

    inline std::map<protobuf::BulletType, THUAI6::BulletType> bulletTypeDict{
        {protobuf::BulletType::NULL_BULLET_TYPE, THUAI6::BulletType::NullBulletType},
        {protobuf::BulletType::FLYING_KNIFE, THUAI6::BulletType::FlyingKnife},
        {protobuf::BulletType::COMMON_ATTACK_OF_TRICKER, THUAI6::BulletType::CommonAttackOfTricker},
        {protobuf::BulletType::BOMB_BOMB, THUAI6::BulletType::BombBomb},
        {protobuf::BulletType::JUMPY_DUMPTY, THUAI6::BulletType::JumpyDumpty},
        {protobuf::BulletType::ATOM_BOMB, THUAI6::BulletType::AtomBomb},
    };

    inline std::map<protobuf::MessageOfObj::MessageOfObjCase, THUAI6::MessageOfObj> messageOfObjDict{
        {protobuf::MessageOfObj::MessageOfObjCase::kStudentMessage, THUAI6::MessageOfObj::StudentMessage},
        {protobuf::MessageOfObj::MessageOfObjCase::kTrickerMessage, THUAI6::MessageOfObj::TrickerMessage},
        {protobuf::MessageOfObj::MessageOfObjCase::kPropMessage, THUAI6::MessageOfObj::PropMessage},
        {protobuf::MessageOfObj::MessageOfObjCase::kBulletMessage, THUAI6::MessageOfObj::BulletMessage},
        {protobuf::MessageOfObj::MessageOfObjCase::kBombedBulletMessage, THUAI6::MessageOfObj::BombedBulletMessage},
        {protobuf::MessageOfObj::MessageOfObjCase::kClassroomMessage, THUAI6::MessageOfObj::ClassroomMessage},
        {protobuf::MessageOfObj::MessageOfObjCase::kDoorMessage, THUAI6::MessageOfObj::DoorMessage},
        {protobuf::MessageOfObj::MessageOfObjCase::kGateMessage, THUAI6::MessageOfObj::GateMessage},
        {protobuf::MessageOfObj::MessageOfObjCase::kChestMessage, THUAI6::MessageOfObj::ChestMessage},
        {protobuf::MessageOfObj::MessageOfObjCase::MESSAGE_OF_OBJ_NOT_SET, THUAI6::MessageOfObj::NullMessageOfObj},
        {protobuf::MessageOfObj::MessageOfObjCase::kMapMessage, THUAI6::MessageOfObj::MapMessage},
        {protobuf::MessageOfObj::MessageOfObjCase::kNewsMessage, THUAI6::MessageOfObj::NewsMessage},
        {protobuf::MessageOfObj::MessageOfObjCase::kHiddenGateMessage, THUAI6::MessageOfObj::HiddenGateMessage},

    };

    // 用于将Protobuf中的类转换为THUAI6的类
    inline std::shared_ptr<THUAI6::Tricker> Protobuf2THUAI6Tricker(const protobuf::MessageOfTricker& trickerMsg)
    {
        auto tricker = std::make_shared<THUAI6::Tricker>();
        tricker->x = trickerMsg.x();
        tricker->y = trickerMsg.y();
        tricker->speed = trickerMsg.speed();
        tricker->score = trickerMsg.score();
        tricker->facingDirection = trickerMsg.facing_direction();
        tricker->bulletType = bulletTypeDict[trickerMsg.bullet_type()];
        tricker->trickDesire = trickerMsg.trick_desire();
        tricker->classVolume = trickerMsg.class_volume();
        tricker->timeUntilSkillAvailable.clear();
        for (int i = 0; i < trickerMsg.time_until_skill_available().size(); i++)
            tricker->timeUntilSkillAvailable.push_back(trickerMsg.time_until_skill_available(i));
        tricker->place = placeTypeDict[trickerMsg.place()];
        tricker->playerState = playerStateDict[trickerMsg.player_state()];
        tricker->props.clear();
        for (int i = 0; i < trickerMsg.prop().size(); i++)
            tricker->props.push_back(propTypeDict[trickerMsg.prop(i)]);
        tricker->trickerType = trickerTypeDict[trickerMsg.tricker_type()];
        tricker->guid = trickerMsg.guid();
        tricker->playerID = trickerMsg.player_id();
        tricker->viewRange = trickerMsg.view_range();
        tricker->radius = trickerMsg.radius();
        tricker->playerType = THUAI6::PlayerType::TrickerPlayer;
        tricker->buff.clear();
        for (int i = 0; i < trickerMsg.buff().size(); i++)
            tricker->buff.push_back(trickerBuffTypeDict[trickerMsg.buff(i)]);
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
        student->score = studentMsg.score();
        student->facingDirection = studentMsg.facing_direction();
        student->bulletType = bulletTypeDict[studentMsg.bullet_type()];
        student->learningSpeed = studentMsg.learning_speed();
        student->encourageSpeed = studentMsg.treat_speed();
        student->encourageProgress = studentMsg.treat_progress();
        student->rouseProgress = studentMsg.rescue_progress();
        student->dangerAlert = studentMsg.danger_alert();
        student->timeUntilSkillAvailable.clear();
        for (int i = 0; i < studentMsg.time_until_skill_available().size(); i++)
            student->timeUntilSkillAvailable.push_back(studentMsg.time_until_skill_available(i));
        student->playerType = THUAI6::PlayerType::StudentPlayer;
        student->props.clear();
        for (int i = 0; i < studentMsg.prop().size(); i++)
            student->props.push_back(propTypeDict[studentMsg.prop(i)]);
        student->place = placeTypeDict[studentMsg.place()];
        student->playerState = playerStateDict[studentMsg.player_state()];
        student->determination = studentMsg.determination();
        student->addiction = studentMsg.addiction();
        student->studentType = studentTypeDict[studentMsg.student_type()];
        student->buff.clear();
        for (int i = 0; i < studentMsg.buff_size(); i++)
            student->buff.push_back(studentBuffTypeDict[studentMsg.buff(i)]);
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
        prop->facingDirection = propMsg.facing_direction();
        return prop;
    }

    inline std::shared_ptr<THUAI6::GameInfo> Protobuf2THUAI6GameInfo(const protobuf::MessageOfAll& allMsg)
    {
        auto gameInfo = std::make_shared<THUAI6::GameInfo>();
        gameInfo->gameTime = allMsg.game_time();
        gameInfo->subjectFinished = allMsg.subject_finished();
        gameInfo->studentGraduated = allMsg.student_graduated();
        gameInfo->studentQuited = allMsg.student_quited();
        gameInfo->studentScore = allMsg.student_score();
        gameInfo->trickerScore = allMsg.tricker_score();
        return gameInfo;
    }

    inline std::shared_ptr<THUAI6::Bullet> Protobuf2THUAI6Bullet(const protobuf::MessageOfBullet& bulletMsg)
    {
        auto bullet = std::make_shared<THUAI6::Bullet>();
        bullet->bulletType = bulletTypeDict[bulletMsg.type()];
        bullet->x = bulletMsg.x();
        bullet->y = bulletMsg.y();
        bullet->facingDirection = bulletMsg.facing_direction();
        bullet->guid = bulletMsg.guid();
        bullet->team = playerTypeDict[bulletMsg.team()];
        bullet->place = placeTypeDict[bulletMsg.place()];
        bullet->bombRange = bulletMsg.bomb_range();
        bullet->speed = bulletMsg.speed();
        return bullet;
    }

    inline std::shared_ptr<THUAI6::BombedBullet> Protobuf2THUAI6BombedBullet(const protobuf::MessageOfBombedBullet& bombedBulletMsg)
    {
        auto bombedBullet = std::make_shared<THUAI6::BombedBullet>();
        bombedBullet->bulletType = bulletTypeDict[bombedBulletMsg.type()];
        bombedBullet->x = bombedBulletMsg.x();
        bombedBullet->y = bombedBulletMsg.y();
        bombedBullet->facingDirection = bombedBulletMsg.facing_direction();
        bombedBullet->mappingID = bombedBulletMsg.mapping_id();
        bombedBullet->bombRange = bombedBulletMsg.bomb_range();
        return bombedBullet;
    }

    inline THUAI6::HiddenGateState Bool2HiddenGateState(bool gateMsg)
    {
        if (gateMsg)
            return THUAI6::HiddenGateState::Opened;
        else
            return THUAI6::HiddenGateState::Refreshed;
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
        {THUAI6::PlaceType::Door3, protobuf::PlaceType::DOOR3},
        {THUAI6::PlaceType::Door5, protobuf::PlaceType::DOOR5},
        {THUAI6::PlaceType::Door6, protobuf::PlaceType::DOOR6},
        {THUAI6::PlaceType::Window, protobuf::PlaceType::WINDOW},
    };

    inline std::map<THUAI6::ShapeType, protobuf::ShapeType> shapeTypeDict{
        {THUAI6::ShapeType::NullShapeType, protobuf::ShapeType::NULL_SHAPE_TYPE},
        {THUAI6::ShapeType::Circle, protobuf::ShapeType::CIRCLE},
        {THUAI6::ShapeType::Square, protobuf::ShapeType::SQUARE},
    };

    inline std::map<THUAI6::PropType, protobuf::PropType> propTypeDict{
        {THUAI6::PropType::NullPropType, protobuf::PropType::NULL_PROP_TYPE},
        {THUAI6::PropType::Key3, protobuf::PropType::KEY3},
        {THUAI6::PropType::Key5, protobuf::PropType::KEY5},
        {THUAI6::PropType::Key6, protobuf::PropType::KEY6},
        {THUAI6::PropType::AddHpOrAp, protobuf::PropType::ADD_HP_OR_AP},
        {THUAI6::PropType::AddLifeOrClairaudience, protobuf::PropType::ADD_LIFE_OR_CLAIRAUDIENCE},
        {THUAI6::PropType::AddSpeed, protobuf::PropType::ADD_SPEED},
        {THUAI6::PropType::ShieldOrSpear, protobuf::PropType::SHIELD_OR_SPEAR},
    };

    inline std::map<THUAI6::PlayerType, protobuf::PlayerType> playerTypeDict{
        {THUAI6::PlayerType::NullPlayerType, protobuf::PlayerType::NULL_PLAYER_TYPE},
        {THUAI6::PlayerType::StudentPlayer, protobuf::PlayerType::STUDENT_PLAYER},
        {THUAI6::PlayerType::TrickerPlayer, protobuf::PlayerType::TRICKER_PLAYER},
    };

    inline std::map<THUAI6::StudentType, protobuf::StudentType> studentTypeDict{
        {THUAI6::StudentType::NullStudentType, protobuf::StudentType::NULL_STUDENT_TYPE},
        {THUAI6::StudentType::Athlete, protobuf::StudentType::ATHLETE},
        {THUAI6::StudentType::Teacher, protobuf::StudentType::TEACHER},
        {THUAI6::StudentType::StraightAStudent, protobuf::StudentType::STRAIGHT_A_STUDENT},
        {THUAI6::StudentType::Robot, protobuf::StudentType::ROBOT},
        {THUAI6::StudentType::TechOtaku, protobuf::StudentType::TECH_OTAKU},
        {THUAI6::StudentType::Sunshine, protobuf::StudentType::SUNSHINE},
    };

    // inline std::map<THUAI6::StudentBuffType, protobuf::StudentBuffType> studentBuffTypeDict{
    //     {THUAI6::StudentBuffType::NullStudentBuffType, protobuf::StudentBuffType::NULL_SBUFF_TYPE},
    //     {THUAI6::StudentBuffType::StudentBuffType1, protobuf::StudentBuffType::ADD_SPEED},
    //     {THUAI6::StudentBuffType::StudentBuffType2, protobuf::StudentBuffType::SBUFFTYPE2},
    //     {THUAI6::StudentBuffType::StudentBuffType3, protobuf::StudentBuffType::SBUFFTYPE3},
    //     {THUAI6::StudentBuffType::StudentBuffType4, protobuf::StudentBuffType::SBUFFTYPE4},
    // };

    inline std::map<THUAI6::TrickerType, protobuf::TrickerType> trickerTypeDict{
        {THUAI6::TrickerType::NullTrickerType, protobuf::TrickerType::NULL_TRICKER_TYPE},
        {THUAI6::TrickerType::Assassin, protobuf::TrickerType::ASSASSIN},
        {THUAI6::TrickerType::Klee, protobuf::TrickerType::KLEE},
        {THUAI6::TrickerType::ANoisyPerson, protobuf::TrickerType::A_NOISY_PERSON},
        {THUAI6::TrickerType::Idol, protobuf::TrickerType::IDOL},
    };

    // inline std::map<THUAI6::TrickerBuffType, protobuf::TrickerBuffType> trickerBuffTypeDict{
    //     {THUAI6::TrickerBuffType::NullTrickerBuffType, protobuf::TrickerBuffType::NULL_TBUFF_TYPE},
    //     {THUAI6::TrickerBuffType::TrickerBuffType1, protobuf::TrickerBuffType::TBUFFTYPE1},
    //     {THUAI6::TrickerBuffType::TrickerBuffType2, protobuf::TrickerBuffType::TBUFFTYPE2},
    //     {THUAI6::TrickerBuffType::TrickerBuffType3, protobuf::TrickerBuffType::TBUFFTYPE3},
    //     {THUAI6::TrickerBuffType::TrickerBuffType4, protobuf::TrickerBuffType::TBUFFTYPE4},
    // };

    // 用于将THUAI6的类转换为Protobuf的消息
    inline protobuf::PlayerMsg THUAI62ProtobufPlayer(int64_t playerID, THUAI6::PlayerType playerType, THUAI6::StudentType studentType, THUAI6::TrickerType trickerType)
    {
        protobuf::PlayerMsg playerMsg;
        playerMsg.set_player_id(playerID);
        playerMsg.set_player_type(playerTypeDict[playerType]);
        if (playerType == THUAI6::PlayerType::StudentPlayer)
        {
            playerMsg.set_player_type(protobuf::PlayerType::STUDENT_PLAYER);
            playerMsg.set_student_type(studentTypeDict[studentType]);
        }
        else if (playerType == THUAI6::PlayerType::TrickerPlayer)
        {
            playerMsg.set_player_type(protobuf::PlayerType::TRICKER_PLAYER);
            playerMsg.set_tricker_type(trickerTypeDict[trickerType]);
        }
        return playerMsg;
    }

    inline protobuf::IDMsg THUAI62ProtobufID(int64_t playerID)
    {
        protobuf::IDMsg idMsg;
        idMsg.set_player_id(playerID);
        return idMsg;
    }

    inline protobuf::TreatAndRescueMsg THUAI62ProtobufTreatAndRescue(int64_t playerID, int64_t mateID)
    {
        protobuf::TreatAndRescueMsg treatAndRescueMsg;
        treatAndRescueMsg.set_player_id(playerID);
        treatAndRescueMsg.set_to_player_id(mateID);
        return treatAndRescueMsg;
    }

    inline protobuf::MoveMsg THUAI62ProtobufMove(int64_t time, double angle, int64_t id)
    {
        protobuf::MoveMsg moveMsg;
        moveMsg.set_time_in_milliseconds(time);
        moveMsg.set_angle(angle);
        moveMsg.set_player_id(id);
        return moveMsg;
    }

    inline protobuf::PropMsg THUAI62ProtobufProp(THUAI6::PropType prop, int64_t id)
    {
        protobuf::PropMsg pickMsg;
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

    inline protobuf::AttackMsg THUAI62ProtobufAttack(double angle, int64_t id)
    {
        protobuf::AttackMsg attackMsg;
        attackMsg.set_angle(angle);
        attackMsg.set_player_id(id);
        return attackMsg;
    }

    inline protobuf::SkillMsg THUAI62ProtobufSkill(int32_t skillID, int64_t id)
    {
        protobuf::SkillMsg skillMsg;
        skillMsg.set_skill_id(skillID);
        skillMsg.set_player_id(id);
        return skillMsg;
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
