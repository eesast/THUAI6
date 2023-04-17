#include "AI.h"
#include "logic.h"
#include "structures.h"
#include <tclap/CmdLine.h>
#include <array>

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

int THUAI6Main(int argc, char** argv, CreateAIFunc AIBuilder)
{
    int pID = 0;
    std::string sIP = "172.22.32.1";
    std::string sPort = "8888";
    bool file = false;
    bool print = false;
    bool warnOnly = false;
    extern const THUAI6::TrickerType trickerType;
    extern const std::array<THUAI6::StudentType, 4> studentType;
    // {
    //     file = true;
    //     print = true;
    //     Logic logic(playerType, pID, trickerType, studentType);
    //     logic.Main(AIBuilder, sIP, sPort, file, print, warnOnly);
    //     return 0;
    // }

    // 使用cmdline的正式版本
    try
    {
        TCLAP::CmdLine cmd("THUAI6 C++ interface commandline parameter introduction");

        TCLAP::ValueArg<std::string> serverIP("I", "serverIP", "Server`s IP 127.0.0.1 in default", false, "127.0.0.1", "string");
        cmd.add(serverIP);

        TCLAP::ValueArg<std::string> serverPort("P", "serverPort", "Port the server listens to 7777 in default", false, "7777", "USORT");
        cmd.add(serverPort);

        std::vector<int> validPlayerIDs{0, 1, 2, 3, 4};
        TCLAP::ValuesConstraint<int> playerIdConstraint(validPlayerIDs);
        TCLAP::ValueArg<int> playerID("p", "playerID", "Player ID 0,1,2,3 valid only", true, -1, &playerIdConstraint);
        cmd.add(playerID);

        std::string DebugDesc = "Set this flag to save the debug log to ./logs folder.\n";
        TCLAP::SwitchArg debug("d", "debug", DebugDesc);
        cmd.add(debug);

        std::string OutputDesc = "Set this flag to print the debug log to the screen.\n";
        TCLAP::SwitchArg output("o", "output", OutputDesc);
        cmd.add(output);

        TCLAP::SwitchArg warning("w", "warning", "Set this flag to only print warning on the screen.\n"
                                                 "This flag will be ignored if the output flag is not set\n");
        cmd.add(warning);

        cmd.parse(argc, argv);
        pID = playerID.getValue();
        sIP = serverIP.getValue();
        sPort = serverPort.getValue();

        file = debug.getValue();
        print = output.getValue();
        if (print)
            warnOnly = warning.getValue();
    }
    catch (TCLAP::ArgException& e)
    {
        std::cerr << "Parsing error: " << e.error() << " for arg " << e.argId() << std::endl;
        return 1;
    }
    try
    {
        THUAI6::PlayerType playerType;
        THUAI6::StudentType stuType = THUAI6::StudentType::NullStudentType;
        if (pID == 4)
            playerType = THUAI6::PlayerType::TrickerPlayer;
        else
        {
            playerType = THUAI6::PlayerType::StudentPlayer;
            stuType = studentType[pID];
        }
        Logic logic(playerType, pID, trickerType, stuType);
        logic.Main(AIBuilder, sIP, sPort, file, print, warnOnly);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}

std::unique_ptr<IAI> CreateAI(int64_t pID)
{
    return std::make_unique<AI>(pID);
}

int main(int argc, char* argv[])
{
    return THUAI6Main(argc, argv, CreateAI);
}
