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
    bool print = false;
    bool level = false;
    extern const THUAI6::PlayerType playerType;
    extern const THUAI6::ButcherType butcherType;
    extern const THUAI6::HumanType humanType;
    // 仅供早期调试使用
    {
        Logic logic(playerType, pID, butcherType, humanType);
        logic.Main(AIBuilder, sIP, sPort, print, level);
        return 0;
    }

    // 使用cmdline的正式版本
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

        std::string DebugDesc = "Set this flag to print the debug log on the screen.\n"
                                "The log will always be saved to ./logs folder.\n";
        TCLAP::SwitchArg debug("d", "debug", DebugDesc);
        cmd.add(debug);

        TCLAP::SwitchArg warning("w", "warning", "Set this flag to only print warning on the screen.\n"
                                                 "This flag will be ignored if the debug flag is not set\n");
        cmd.add(warning);

        cmd.parse(argc, argv);
        pID = playerID.getValue();
        sIP = serverIP.getValue();
        sPort = serverPort.getValue();

        bool d = debug.getValue();
        bool w = warning.getValue();
        if (d)
        {
            print = true;
            level = w;
        }
    }
    catch (TCLAP::ArgException& e)
    {
        std::cerr << "Parsing error: " << e.error() << " for arg " << e.argId() << std::endl;
        return 1;
    }
    Logic logic(playerType, pID, butcherType, humanType);
    logic.Main(AIBuilder, sIP, sPort, print, level);
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
