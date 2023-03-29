import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
sys.path.append(os.path.dirname(os.path.dirname(
    os.path.realpath(__file__))) + '/proto')

from PyAPI.Interface import IAI
from PyAPI.AI import AI
from PyAPI.logic import Logic
from typing import List, Callable


def THUAI6Main(argv: List[str], AIBuilder: Callable) -> None:
    pID: int = 0
    sIP: str = "172.22.32.1"
    sPort: str = "8888"
    file: bool = True
    screen: bool = True
    warnOnly: bool = False
    logic = Logic(pID)
    print("Welcome to THUAI6")
    logic.Main(AIBuilder, sIP, sPort, file, screen, warnOnly)


def CreateAI() -> IAI:
    return AI()


if __name__ == '__main__':
    THUAI6Main(sys.argv, CreateAI)
