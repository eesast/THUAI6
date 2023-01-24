from queue import Queue
import grpc
import threading
import proto.Message2Clients_pb2 as Message2Clients
import proto.Message2Server_pb2 as Message2Server
import proto.MessageType_pb2 as MessageType
import proto.Services_pb2_grpc as Services
from logic import Logic
from Interface import IErrorHandler
from utils import THUAI62Proto
import structures as THUAI6


# 使用gRPC的异步来减少通信对于选手而言损失的时间，而gRPC的return值有result()方法，故若连接错误时也应当返回一个具有result()方法的对象，使用此处的ErrorHandler类来实现
class BoolErrorHandler(IErrorHandler):
    @staticmethod
    def result():
        return False


class Communication:

    __THUAI6Stub: Services.AvailableServiceStub
    __haveNewMessage: bool
    __message2Client: Message2Clients.MessageToClient
    __messageQueue: Queue  # Python的Queue是线程安全的，故无须自己实现Queue

    def __init__(self, sIP: str, sPort: str):
        aim = sIP + ':' + sPort
        channel = grpc.insecure_channel(aim)
        self.__THUAI6Stub = Services.AvailableServiceStub(channel)
        self.__haveNewMessage = False
        self.__messageQueue = Queue()

    def Move(self, time: int, angle: float, playerID: int) -> bool:
        try:
            moveResult = self.__THUAI6Stub.Move(
                THUAI62Proto.THUAI62ProtobufMove(time, angle, playerID))
        except grpc.RpcError as e:
            return False
        else:
            return moveResult.act_success

    def PickProp(self, propType: THUAI6.PropType, playerID: int) -> bool:
        try:
            pickResult = self.__THUAI6Stub.PickProp(
                THUAI62Proto.THUAI62ProtobufPick(propType, playerID))
        except grpc.RpcError as e:
            return False
        else:
            return pickResult.act_success

    def UseProp(self, playerID: int):
        try:
            useResult = self.__THUAI6Stub.UseProp(
                THUAI62Proto.THUAI62ProtobufID(playerID))
        except grpc.RpcError as e:
            return False
        else:
            return useResult.act_success

    def UseSkill(self, playerID: int) -> bool:
        try:
            useResult = self.__THUAI6Stub.UseSkill(
                THUAI62Proto.THUAI62ProtobufID(playerID))
        except grpc.RpcError as e:
            return False
        else:
            return useResult.act_success

    def SendMessage(self, toID: int, message: str, playerID: int) -> bool:
        try:
            sendResult = self.__THUAI6Stub.SendMessage(
                THUAI62Proto.THUAI62ProtobufSend(message, toID, playerID))
        except grpc.RpcError as e:
            return False
        else:
            return sendResult.act_success

    def HaveMessage(self) -> bool:
        return not self.__messageQueue.empty()

    def GetMessage(self) -> tuple[int, str]:
        try:
            message = self.__messageQueue.get_nowait()
        except Exception as e:
            return -1, ''
        else:
            return message

    def ReadMessage(self, playerID: int) -> None:
        def tRead():
            try:
                for msg in self.__THUAI6Stub.GetMessage(
                        THUAI62Proto.THUAI62ProtobufID(playerID)):
                    self.__messageQueue.put(
                        (msg.from_player_id, msg.message_received))
            except grpc.RpcError as e:
                return

        threading.Thread(target=tRead).start()

    def Escape(self, playerID: int) -> bool:
        try:
            escapeResult = self.__THUAI6Stub.Escape(
                THUAI62Proto.THUAI62ProtobufID(playerID))
        except grpc.RpcError as e:
            return False
        else:
            return escapeResult.act_success

    def StartFixMachine(self, playerID: int) -> bool:
        try:
            fixResult = self.__THUAI6Stub.StartFixMachine(
                THUAI62Proto.THUAI62ProtobufID(playerID))
        except grpc.RpcError as e:
            return False
        else:
            return fixResult.act_success

    def EndFixMachine(self, playerID: int) -> bool:
        try:
            fixResult = self.__THUAI6Stub.EndFixMachine(
                THUAI62Proto.THUAI62ProtobufID(playerID))
        except grpc.RpcError as e:
            return False
        else:
            return fixResult.act_success

    def StartSaveHuman(self, playerID: int) -> bool:
        try:
            saveResult = self.__THUAI6Stub.StartSaveHuman(
                THUAI62Proto.THUAI62ProtobufID(playerID))
        except grpc.RpcError as e:
            return False
        else:
            return saveResult.act_success

    def EndSaveHuman(self, playerID: int) -> bool:
        try:
            saveResult = self.__THUAI6Stub.EndSaveHuman(
                THUAI62Proto.THUAI62ProtobufID(playerID))
        except grpc.RpcError as e:
            return False
        else:
            return saveResult.act_success

    def Attack(self, angle: float, playerID: int) -> bool:
        try:
            attackResult = self.__THUAI6Stub.Attack(
                THUAI62Proto.THUAI62ProtobufAttack(angle, playerID))
        except grpc.RpcError as e:
            return False
        else:
            return attackResult.act_success

    def CarryHuman(self, playerID: int) -> bool:
        try:
            carryResult = self.__THUAI6Stub.CarryHuman(
                THUAI62Proto.THUAI62ProtobufID(playerID))
        except grpc.RpcError as e:
            return False
        else:
            return carryResult.act_success

    def ReleaseHuman(self, playerID: int) -> bool:
        try:
            releaseResult = self.__THUAI6Stub.ReleaseHuman(
                THUAI62Proto.THUAI62ProtobufID(playerID))
        except grpc.RpcError as e:
            return False
        else:
            return releaseResult.act_success

    def HangHuman(self, playerID: int) -> bool:
        try:
            hangResult = self.__THUAI6Stub.HangHuman(
                THUAI62Proto.THUAI62ProtobufID(playerID))
        except grpc.RpcError as e:
            return False
        else:
            return hangResult.act_success

    def TryConnection(self, playerID: int) -> bool:
        try:
            connectResult = self.__THUAI6Stub.TryConnection(
                THUAI62Proto.THUAI62ProtobufID(playerID))
        except grpc.RpcError as e:
            return False
        else:
            return True

    def HaveMessage2Client(self) -> bool:
        return self.__haveNewMessage

    def GetMessage2Client(self) -> Message2Clients.MessageToClient:
        self.__haveNewMessage = False
        return self.__message2Client

    def AddPlayer(self, playerID: int, playerType: THUAI6.PlayerType, humanType: THUAI6.HumanType, butcherType: THUAI6.ButcherType) -> None:
        def tMessage():
            try:
                playerMsg = THUAI62Proto.THUAI62ProtobufPlayer(
                    playerID, playerType, humanType, butcherType)
                for msg in self.__THUAI6Stub.AddPlayer(playerMsg):
                    self.__haveNewMessage = True
                    self.__message2Client = msg
            except grpc.RpcError as e:
                return

        threading.Thread(target=tMessage).start()
