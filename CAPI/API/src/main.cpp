#include "AI.h"
#include "logic.h"
#include "structures.h"
#include <tclap/CmdLine.h>

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

int THUAI6Main(int argc, char** argv, CreateAIFunc AIBuilder)
{
    int pID = 114514;
    std::string sIP = "114.51.41.91";
    std::string sPort = "9810";
    std::string filename = "";
    bool level = false;
    extern const THUAI6::PlayerType playerType;
    extern const THUAI6::ButcherType butcherType;
    extern const THUAI6::HumanType humanType;
    // 仅供早期调试使用
    {
        Logic logic(playerType, pID, butcherType, humanType);
        logic.Main(AIBuilder, sIP, sPort, level, filename);
        return 0;
    }

    // 使用cmdline的版本
    try
    {
        TCLAP::CmdLine cmd("THUAI6 C++ interface commandline parameter introduction");

        TCLAP::ValueArg<std::string> serverIP("I", "serverIP", "Server`s IP 127.0.0.1 in default", false, "127.0.0.1", "string");
        cmd.add(serverIP);

        TCLAP::ValueArg<uint16_t> serverPort("P", "serverPort", "Port the server listens to 7777 in default", false, 7777, "USORT");
        cmd.add(serverPort);

        std::vector<int> validPlayerIDs{0, 1, 2, 3};
        TCLAP::ValuesConstraint<int> playerIdConstraint(validPlayerIDs);
        TCLAP::ValueArg<int> playerID("p", "playerID", "Player ID 0,1,2,3 valid only", true, -1, &playerIdConstraint);
        cmd.add(playerID);

        std::string DebugDesc = "Set this flag to use API for debugging.\n"
                                "If \"-f\" is not set, the log will be printed on the screen.\n"
                                "Or you could specify a file to store it.";
        TCLAP::SwitchArg debug("d", "debug", DebugDesc);
        cmd.add(debug);

        TCLAP::ValueArg<std::string> FileName("f", "filename", "Specify a file to store the log.", false, "", "string");
        cmd.add(FileName);

        TCLAP::SwitchArg warning("w", "warning", "Warn of some obviously invalid operations (only when \"-d\" is set).");
        cmd.add(warning);

        cmd.parse(argc, argv);
        pID = playerID.getValue();
        sIP = serverIP.getValue();
        sPort = serverPort.getValue();

        bool d = debug.getValue();
        bool w = warning.getValue();
        if (d)
        {
            level = w;
        }
        filename = FileName.getValue();
    }
    catch (TCLAP::ArgException& e)
    {
        std::cerr << "Parsing error: " << e.error() << " for arg " << e.argId() << std::endl;
        return 1;
    }
    Logic logic(playerType, pID, butcherType, humanType);
    logic.Main(AIBuilder, sIP, sPort, level, filename);
    return 0;
}

std::unique_ptr<IAI> CreateAI()
{
    return std::make_unique<AI>();
}

int main(int argc, char* argv[])
{
    return THUAI6Main(argc, argv, CreateAI);
}