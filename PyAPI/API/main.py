import sys
from typing import List, Callable
from logic import Logic
from AI import AI, Setting
from Interface import IAI
import structures as THUAI6


def THUAI6Main(argv: List[str], AIBuilder: Callable) -> None:
    pID: int = 114514
    sIP: str = "114.51.41.91"
    sPort: str = "9810"
    file: bool = False
    print: bool = False
    warnOnly: bool = False
    logic = Logic(Setting.playerType(), pID,
                  Setting.humanType(), Setting.butcherType())
    logic.Main(AIBuilder, sIP, sPort, file, print, warnOnly)


def CreateAI() -> IAI:
    return AI()


if __name__ == '__main__':
    THUAI6Main(sys.argv, CreateAI)
