#pragma once
#include "logic.h"

Logic::Logic(std::shared_ptr<grpc::Channel> channel) :
    THUAI6Stub(Protobuf::AvailableService::NewStub(channel))
{
}