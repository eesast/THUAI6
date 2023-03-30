import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__))))
sys.path.append(os.path.dirname(os.path.dirname(
    os.path.realpath(__file__))) + '/proto')

from PyAPI.Interface import IAI
from PyAPI.AI import AI
from PyAPI.logic import Logic
from typing import List, Callable
import argparse


def THUAI6Main(argv: List[str], AIBuilder: Callable) -> None:
    pID: int = 0
    sIP: str = "127.0.0.1"
    sPort: str = "8888"
    file: bool = True
    screen: bool = True
    warnOnly: bool = False
    parser = argparse.ArgumentParser(
        description="THUAI6 Python Interface Commandline Parameter Introduction")
    parser.add_argument("-I", type=str, required=True,
                        help="Server`s IP 127.0.0.1 in default", dest="sIP", default="127.0.0.1")
    parser.add_argument("-P", type=str, required=True,
                        help="Server`s Port 8888 in default", dest="sPort", default="8888")
    parser.add_argument("-p", type=int, required=True,
                        help="Player`s ID", dest="pID", choices=[0, 1, 2, 3, 4])
    parser.add_argument("-d", type=bool, required=False,
                        help="Set this flag to save the debug log to ./logs folder", dest="file", default=False, const=True, nargs='?')
    parser.add_argument("-o", type=bool, required=False,
                        help="Set this flag to print the debug log to the screen", dest="screen", default=False, const=True, nargs='?')
    parser.add_argument("-w", type=bool, required=False,
                        help="Set this flag to only print warning on the screen", dest="warnOnly", default=False, const=True, nargs='?')
    args = parser.parse_args()
    pID = args.pID
    sIP = args.sIP
    sPort = args.sPort
    file = args.file
    screen = args.screen
    warnOnly = args.warnOnly
    logic = Logic(pID)
    logic.Main(AIBuilder, sIP, sPort, file, screen, warnOnly)


def CreateAI() -> IAI:
    return AI()


if __name__ == '__main__':
    THUAI6Main(sys.argv, CreateAI)
