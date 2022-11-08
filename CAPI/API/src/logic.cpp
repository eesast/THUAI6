#include "logic.h"
#include "structures.h"
#include <grpcpp/grpcpp.h>
#include <functional>
using namespace protobuf;
using grpc::ClientContext;

extern const THUAI6::PlayerType playerType;

Logic::Logic(THUAI6::PlayerType type, int ID, THUAI6::ButcherType butcher, THUAI6::HumanType human) :
    playerType(type),
    playerID(ID),
    butcherType(butcher),
    humanType(human)
{
    currentState = &state[0];
    bufferState = &state[1];
}

protobuf::MessageToClient Logic::GetFullMessage()
{
}

void Logic::Update() noexcept
{
}

void Logic::PlayerWrapper(std::function<void()> player)
{
    {
        std::unique_lock<std::mutex> lock(mtxAI);
        cvAI.wait(lock, [this]()
                  { return AIStart; });
    }
    player();
}

bool Logic::TryConnection()
{
    IDMsg request;
    BoolRes reply;
    ClientContext context;
    request.set_player_id(playerID);
    auto status = THUAI6Stub->TryConnection(&context, request, &reply);
    if (status.ok())
        return true;
    else
        return false;
}

void Logic::Main(CreateAIFunc createAI, std::string IP, std::string port)
{
    // 构造AI
    pAI = createAI();

    // 建立与服务器之间通信的Stub
    std::string aim = IP + ':' + port;
    auto channel = grpc::CreateChannel(aim, grpc::InsecureChannelCredentials());
    THUAI6Stub = protobuf::AvailableService::NewStub(channel);

    // 构造timer
    if (playerType == THUAI6::PlayerType::HumanPlayer)
        timer = std::make_unique<HumanAPI>(*this);
    else if (playerType == THUAI6::PlayerType::ButcherPlayer)
        timer = std::make_unique<ButcherAPI>(*this);

    // 构造AI线程
    auto AIThread = [&, this]()
    {
        auto ai = createAI();
        while (AILoop)
        {
            Update();
            timer->StartTimer();
            timer->Play(*ai);
            timer->EndTimer();
        }
    };

    tAI = std::thread(&Logic::PlayerWrapper, this, AIThread);

    // 连接服务器
    if (TryConnection())
    {
        std::cout << "Connect to the server successfully, AI thread will be start." << std::endl;
        if (tAI.joinable())
        {
            std::cout << "Join the AI thread." << std::endl;
            tAI.join();
        }
    }
    else
    {
        std::cout << "Connection error!" << std::endl;
    }
}