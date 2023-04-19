#include <optional>
#include "AI.h"
#include "API.h"
#define PI 3.14159265358979323846

int StudentAPI::GetFrameCount() const
{
    return logic.GetCounter();
}

int TrickerAPI::GetFrameCount() const
{
    return logic.GetCounter();
}

std::future<bool> StudentAPI::Move(int64_t timeInMilliseconds, double angleInRadian)
{
    return std::async(std::launch::async, [=]()
                      { return logic.Move(timeInMilliseconds, angleInRadian); });
}

std::future<bool> StudentAPI::MoveDown(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, 0);
}

std::future<bool> StudentAPI::MoveRight(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI * 0.5);
}

std::future<bool> StudentAPI::MoveUp(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI);
}

std::future<bool> StudentAPI::MoveLeft(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI * 1.5);
}

std::future<bool> TrickerAPI::Move(int64_t timeInMilliseconds, double angleInRadian)
{
    return std::async(std::launch::async, [=]()
                      { return logic.Move(timeInMilliseconds, angleInRadian); });
}

std::future<bool> TrickerAPI::MoveDown(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, 0);
}

std::future<bool> TrickerAPI::MoveRight(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI * 0.5);
}

std::future<bool> TrickerAPI::MoveUp(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI);
}

std::future<bool> TrickerAPI::MoveLeft(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI * 1.5);
}

std::future<bool> StudentAPI::PickProp(THUAI6::PropType prop)
{
    return std::async(std::launch::async, [=]()
                      { return logic.PickProp(prop); });
}

std::future<bool> StudentAPI::UseProp(THUAI6::PropType prop)
{
    return std::async(std::launch::async, [=]()
                      { return logic.UseProp(prop); });
}

std::future<bool> TrickerAPI::PickProp(THUAI6::PropType prop)
{
    return std::async(std::launch::async, [=]()
                      { return logic.PickProp(prop); });
}

std::future<bool> TrickerAPI::UseProp(THUAI6::PropType prop)
{
    return std::async(std::launch::async, [=]()
                      { return logic.UseProp(prop); });
}

std::future<bool> StudentAPI::ThrowProp(THUAI6::PropType prop)
{
    return std::async(std::launch::async, [=]()
                      { return logic.ThrowProp(prop); });
}

std::future<bool> TrickerAPI::ThrowProp(THUAI6::PropType prop)
{
    return std::async(std::launch::async, [=]()
                      { return logic.ThrowProp(prop); });
}

std::future<bool> StudentAPI::UseSkill(int32_t skillID)
{
    return std::async(std::launch::async, [=]()
                      { return logic.UseSkill(skillID); });
}

std::future<bool> TrickerAPI::UseSkill(int32_t skillID)
{
    return std::async(std::launch::async, [=]()
                      { return logic.UseSkill(skillID); });
}

std::future<bool> StudentAPI::OpenDoor()
{
    return std::async(std::launch::async, [this]()
                      { return logic.OpenDoor(); });
}

std::future<bool> TrickerAPI::OpenDoor()
{
    return std::async(std::launch::async, [this]()
                      { return logic.OpenDoor(); });
}

std::future<bool> StudentAPI::CloseDoor()
{
    return std::async(std::launch::async, [this]()
                      { return logic.CloseDoor(); });
}

std::future<bool> TrickerAPI::CloseDoor()
{
    return std::async(std::launch::async, [this]()
                      { return logic.CloseDoor(); });
}

std::future<bool> StudentAPI::SkipWindow()
{
    return std::async(std::launch::async, [this]()
                      { return logic.SkipWindow(); });
}

std::future<bool> TrickerAPI::SkipWindow()
{
    return std::async(std::launch::async, [this]()
                      { return logic.SkipWindow(); });
}

std::future<bool> StudentAPI::StartOpenGate()
{
    return std::async(std::launch::async, [this]()
                      { return logic.StartOpenGate(); });
}

std::future<bool> TrickerAPI::StartOpenGate()
{
    return std::async(std::launch::async, [this]()
                      { return logic.StartOpenGate(); });
}

std::future<bool> StudentAPI::StartOpenChest()
{
    return std::async(std::launch::async, [this]()
                      { return logic.StartOpenChest(); });
}

std::future<bool> TrickerAPI::StartOpenChest()
{
    return std::async(std::launch::async, [this]()
                      { return logic.StartOpenChest(); });
}

std::future<bool> StudentAPI::EndAllAction()
{
    return std::async(std::launch::async, [this]()
                      { return logic.EndAllAction(); });
}

std::future<bool> TrickerAPI::EndAllAction()
{
    return std::async(std::launch::async, [this]()
                      { return logic.EndAllAction(); });
}

std::future<bool> StudentAPI::SendMessage(int64_t toID, std::string message)
{
    return std::async(std::launch::async, [=]()
                      { return logic.SendMessage(toID, message); });
}

std::future<bool> TrickerAPI::SendMessage(int64_t toID, std::string message)
{
    return std::async(std::launch::async, [=]()
                      { return logic.SendMessage(toID, message); });
}

bool StudentAPI::HaveMessage()
{
    return logic.HaveMessage();
}

bool TrickerAPI::HaveMessage()
{
    return logic.HaveMessage();
}

std::pair<int64_t, std::string> StudentAPI::GetMessage()
{
    return logic.GetMessage();
}

std::pair<int64_t, std::string> TrickerAPI::GetMessage()
{
    return logic.GetMessage();
}

bool StudentAPI::Wait()
{
    if (logic.GetCounter() == -1)
        return false;
    else
        return logic.WaitThread();
}

bool TrickerAPI::Wait()
{
    if (logic.GetCounter() == -1)
        return false;
    else
        return logic.WaitThread();
}

std::vector<std::shared_ptr<const THUAI6::Tricker>> StudentAPI::GetTrickers() const
{
    return logic.GetTrickers();
}

std::vector<std::shared_ptr<const THUAI6::Student>> StudentAPI::GetStudents() const
{
    return logic.GetStudents();
}

std::vector<std::shared_ptr<const THUAI6::Tricker>> TrickerAPI::GetTrickers() const
{
    return logic.GetTrickers();
}

std::vector<std::shared_ptr<const THUAI6::Student>> TrickerAPI::GetStudents() const
{
    return logic.GetStudents();
}

std::vector<std::shared_ptr<const THUAI6::Prop>> StudentAPI::GetProps() const
{
    return logic.GetProps();
}

std::vector<std::shared_ptr<const THUAI6::Prop>> TrickerAPI::GetProps() const
{
    return logic.GetProps();
}

std::vector<std::shared_ptr<const THUAI6::Bullet>> StudentAPI::GetBullets() const
{
    return logic.GetBullets();
}

std::vector<std::shared_ptr<const THUAI6::Bullet>> TrickerAPI::GetBullets() const
{
    return logic.GetBullets();
}

std::vector<std::vector<THUAI6::PlaceType>> StudentAPI::GetFullMap() const
{
    return logic.GetFullMap();
}

THUAI6::PlaceType StudentAPI::GetPlaceType(int32_t cellX, int32_t cellY) const
{
    return logic.GetPlaceType(cellX, cellY);
}

THUAI6::PlaceType TrickerAPI::GetPlaceType(int32_t cellX, int32_t cellY) const
{
    return logic.GetPlaceType(cellX, cellY);
}

std::vector<std::vector<THUAI6::PlaceType>> TrickerAPI::GetFullMap() const
{
    return logic.GetFullMap();
}

bool StudentAPI::IsDoorOpen(int32_t cellX, int32_t cellY) const
{
    return logic.IsDoorOpen(cellX, cellY);
}

bool TrickerAPI::IsDoorOpen(int32_t cellX, int32_t cellY) const
{
    return logic.IsDoorOpen(cellX, cellY);
}

int32_t StudentAPI::GetClassroomProgress(int32_t cellX, int32_t cellY) const
{
    return logic.GetClassroomProgress(cellX, cellY);
}

int32_t TrickerAPI::GetClassroomProgress(int32_t cellX, int32_t cellY) const
{
    return logic.GetClassroomProgress(cellX, cellY);
}

int32_t StudentAPI::GetChestProgress(int32_t cellX, int32_t cellY) const
{
    return logic.GetChestProgress(cellX, cellY);
}

int32_t TrickerAPI::GetChestProgress(int32_t cellX, int32_t cellY) const
{
    return logic.GetChestProgress(cellX, cellY);
}

int32_t StudentAPI::GetGateProgress(int32_t cellX, int32_t cellY) const
{
    return logic.GetGateProgress(cellX, cellY);
}

int32_t TrickerAPI::GetGateProgress(int32_t cellX, int32_t cellY) const
{
    return logic.GetGateProgress(cellX, cellY);
}

int32_t StudentAPI::GetDoorProgress(int32_t cellX, int32_t cellY) const
{
    return logic.GetDoorProgress(cellX, cellY);
}

int32_t TrickerAPI::GetDoorProgress(int32_t cellX, int32_t cellY) const
{
    return logic.GetDoorProgress(cellX, cellY);
}

THUAI6::HiddenGateState StudentAPI::GetHiddenGateState(int32_t cellX, int32_t cellY) const
{
    return logic.GetHiddenGateState(cellX, cellY);
}

THUAI6::HiddenGateState TrickerAPI::GetHiddenGateState(int32_t cellX, int32_t cellY) const
{
    return logic.GetHiddenGateState(cellX, cellY);
}

std::shared_ptr<const THUAI6::GameInfo> StudentAPI::GetGameInfo() const
{
    return logic.GetGameInfo();
}

std::shared_ptr<const THUAI6::GameInfo> TrickerAPI::GetGameInfo() const
{
    return logic.GetGameInfo();
}

std::vector<int64_t> StudentAPI::GetPlayerGUIDs() const
{
    return logic.GetPlayerGUIDs();
}

std::vector<int64_t> TrickerAPI::GetPlayerGUIDs() const
{
    return logic.GetPlayerGUIDs();
}

std::future<bool> StudentAPI::StartLearning()
{
    return std::async(std::launch::async, [this]()
                      { return logic.StartLearning(); });
}

std::future<bool> StudentAPI::StartEncourageMate(int64_t mateID)
{
    return std::async(std::launch::async, [=]()
                      { return logic.StartEncourageMate(mateID); });
}

std::future<bool> StudentAPI::StartRouseMate(int64_t mateID)
{
    return std::async(std::launch::async, [=]()
                      { return logic.StartRouseMate(mateID); });
}

std::future<bool> StudentAPI::Graduate()
{
    return std::async(std::launch::async, [this]()
                      { return logic.Graduate(); });
}

std::shared_ptr<const THUAI6::Student> StudentAPI::GetSelfInfo() const
{
    return logic.StudentGetSelfInfo();
}

std::future<bool> TrickerAPI::Attack(double angleInRadian)
{
    return std::async(std::launch::async, [=]()
                      { return logic.Attack(angleInRadian); });
}

std::future<bool> StudentAPI::Attack(double angleInRadian)
{
    return std::async(std::launch::async, [=]()
                      { return logic.Attack(angleInRadian); });
}

std::shared_ptr<const THUAI6::Tricker> TrickerAPI::GetSelfInfo() const
{
    return logic.TrickerGetSelfInfo();
}

void StudentAPI::Play(IAI& ai)
{
    ai.play(*this);
}

void TrickerAPI::Play(IAI& ai)
{
    ai.play(*this);
}
