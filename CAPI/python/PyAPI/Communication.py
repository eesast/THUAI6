import PyAPI.structures as THUAI6
from PyAPI.AI import Setting
from PyAPI.utils import THUAI62Proto
from PyAPI.Interface import IErrorHandler
import proto.Services_pb2_grpc as Services
import proto.Message2Clients_pb2 as Message2Clients
import threading
import grpc

from typing import Union


# 使用gRPC的异步来减少通信对于选手而言损失的时间，而gRPC的return值有result()方法，故若连接错误时也应当返回一个具有result()方法的对象，使用此处的ErrorHandler类来实现
class BoolErrorHandler(IErrorHandler):
    @staticmethod
    def result():
        return False


class Communication:
    def __init__(self, sIP: str, sPort: str):
        aim = sIP + ":" + sPort
        channel = grpc.insecure_channel(aim)
        self.__THUAI6Stub = Services.AvailableServiceStub(channel)
        self.__haveNewMessage = False
        self.__cvMessage = threading.Condition()
        self.__message2Client: Message2Clients.MessageToClient
        self.__mtxLimit = threading.Lock()
        self.__counter = 0
        self.__counterMove = 0
        self.__limit = 50
        self.__moveLimit = 10

    def Move(self, time: int, angle: float, playerID: int) -> bool:
        try:
            with self.__mtxLimit:
                if (
                    self.__counter >= self.__limit
                    or self.__counterMove >= self.__moveLimit
                ):
                    return False
                self.__counter += 1
                self.__counterMove += 1
            moveResult = self.__THUAI6Stub.Move(
                THUAI62Proto.THUAI62ProtobufMove(time, angle, playerID)
            )
        except grpc.RpcError as e:
            return False
        else:
            return moveResult.act_success

    def PickProp(self, propType: THUAI6.PropType, playerID: int) -> bool:
        try:
            with self.__mtxLimit:
                if self.__counter >= self.__limit:
                    return False
                self.__counter += 1
            pickResult = self.__THUAI6Stub.PickProp(
                THUAI62Proto.THUAI62ProtobufProp(propType, playerID)
            )
        except grpc.RpcError as e:
            return False
        else:
            return pickResult.act_success

    def UseProp(self, propType: THUAI6.PropType, playerID: int) -> bool:
        try:
            with self.__mtxLimit:
                if self.__counter >= self.__limit:
                    return False
                self.__counter += 1
            useResult = self.__THUAI6Stub.UseProp(
                THUAI62Proto.THUAI62ProtobufProp(propType, playerID)
            )
        except grpc.RpcError as e:
            return False
        else:
            return useResult.act_success

    def ThrowProp(self, propType: THUAI6.PropType, playerID: int) -> bool:
        try:
            with self.__mtxLimit:
                if self.__counter >= self.__limit:
                    return False
                self.__counter += 1
            throwResult = self.__THUAI6Stub.ThrowProp(
                THUAI62Proto.THUAI62ProtobufProp(propType, playerID)
            )
        except grpc.RpcError as e:
            return False
        else:
            return throwResult.act_success

    def UseSkill(self, skillID: int, playerID: int) -> bool:
        try:
            with self.__mtxLimit:
                if self.__counter >= self.__limit:
                    return False
                self.__counter += 1
            useResult = self.__THUAI6Stub.UseSkill(
                THUAI62Proto.THUAI62ProtobufSkill(skillID, playerID)
            )
        except grpc.RpcError as e:
            return False
        else:
            return useResult.act_success

    def SendMessage(self, toID: int, message: Union[str, bytes], playerID: int) -> bool:
        try:
            with self.__mtxLimit:
                if self.__counter >= self.__limit:
                    return False
                self.__counter += 1
            sendResult = self.__THUAI6Stub.SendMessage(
                THUAI62Proto.THUAI62ProtobufSend(message, toID, playerID)
            )
        except grpc.RpcError as e:
            return False
        else:
            return sendResult.act_success

    def Graduate(self, playerID: int) -> bool:
        try:
            with self.__mtxLimit:
                if self.__counter >= self.__limit:
                    return False
                self.__counter += 1
            escapeResult = self.__THUAI6Stub.Graduate(
                THUAI62Proto.THUAI62ProtobufID(playerID)
            )
        except grpc.RpcError as e:
            return False
        else:
            return escapeResult.act_success

    def StartLearning(self, playerID: int) -> bool:
        try:
            with self.__mtxLimit:
                if self.__counter >= self.__limit:
                    return False
                self.__counter += 1
            learnResult = self.__THUAI6Stub.StartLearning(
                THUAI62Proto.THUAI62ProtobufID(playerID)
            )
        except grpc.RpcError as e:
            return False
        else:
            return learnResult.act_success

    def StartEncourageMate(self, playerID: int, mateID: int) -> bool:
        try:
            with self.__mtxLimit:
                if self.__counter >= self.__limit:
                    return False
                self.__counter += 1
            helpResult = self.__THUAI6Stub.StartTreatMate(
                THUAI62Proto.THUAI62ProtobufTreatAndRescue(playerID, mateID)
            )
        except grpc.RpcError as e:
            return False
        else:
            return helpResult.act_success

    def StartRouseMate(self, playerID: int, mateID: int) -> bool:
        try:
            with self.__mtxLimit:
                if self.__counter >= self.__limit:
                    return False
                self.__counter += 1
            helpResult = self.__THUAI6Stub.StartRescueMate(
                THUAI62Proto.THUAI62ProtobufTreatAndRescue(playerID, mateID)
            )
        except grpc.RpcError as e:
            return False
        else:
            return helpResult.act_success

    def Attack(self, angle: float, playerID: int) -> bool:
        try:
            with self.__mtxLimit:
                if self.__counter >= self.__limit:
                    return False
                self.__counter += 1
            attackResult = self.__THUAI6Stub.Attack(
                THUAI62Proto.THUAI62ProtobufAttack(angle, playerID)
            )
        except grpc.RpcError as e:
            return False
        else:
            return attackResult.act_success

    def OpenDoor(self, playerID: int) -> bool:
        try:
            with self.__mtxLimit:
                if self.__counter >= self.__limit:
                    return False
                self.__counter += 1
            openResult = self.__THUAI6Stub.OpenDoor(
                THUAI62Proto.THUAI62ProtobufID(playerID)
            )
        except grpc.RpcError as e:
            return False
        else:
            return openResult.act_success

    def CloseDoor(self, playerID: int) -> bool:
        try:
            with self.__mtxLimit:
                if self.__counter >= self.__limit:
                    return False
                self.__counter += 1
            closeResult = self.__THUAI6Stub.CloseDoor(
                THUAI62Proto.THUAI62ProtobufID(playerID)
            )
        except grpc.RpcError as e:
            return False
        else:
            return closeResult.act_success

    def SkipWindow(self, playerID: int) -> bool:
        try:
            with self.__mtxLimit:
                if self.__counter >= self.__limit:
                    return False
                self.__counter += 1
            skipResult = self.__THUAI6Stub.SkipWindow(
                THUAI62Proto.THUAI62ProtobufID(playerID)
            )
        except grpc.RpcError as e:
            return False
        else:
            return skipResult.act_success

    def StartOpenGate(self, playerID: int) -> bool:
        try:
            with self.__mtxLimit:
                if self.__counter >= self.__limit:
                    return False
                self.__counter += 1
            openResult = self.__THUAI6Stub.StartOpenGate(
                THUAI62Proto.THUAI62ProtobufID(playerID)
            )
        except grpc.RpcError as e:
            return False
        else:
            return openResult.act_success

    def StartOpenChest(self, playerID: int) -> bool:
        try:
            with self.__mtxLimit:
                if self.__counter >= self.__limit:
                    return False
                self.__counter += 1
            openResult = self.__THUAI6Stub.StartOpenChest(
                THUAI62Proto.THUAI62ProtobufID(playerID)
            )
        except grpc.RpcError as e:
            return False
        else:
            return openResult.act_success

    def EndAllAction(self, playerID: int) -> bool:
        try:
            with self.__mtxLimit:
                if (
                    self.__counter >= self.__limit
                    or self.__counterMove >= self.__moveLimit
                ):
                    return False
                self.__counter += 1
                self.__counterMove += 1
            endResult = self.__THUAI6Stub.EndAllAction(
                THUAI62Proto.THUAI62ProtobufID(playerID)
            )
        except grpc.RpcError as e:
            return False
        else:
            return endResult.act_success

    def TryConnection(self, playerID: int) -> bool:
        try:
            connectResult = self.__THUAI6Stub.TryConnection(
                THUAI62Proto.THUAI62ProtobufID(playerID)
            )
        except grpc.RpcError as e:
            return False
        else:
            return True

    def GetMessage2Client(self) -> Message2Clients.MessageToClient:
        with self.__cvMessage:
            self.__cvMessage.wait_for(lambda: self.__haveNewMessage)
            self.__haveNewMessage = False
            return self.__message2Client

    def AddPlayer(self, playerID: int, playerType: THUAI6.PlayerType) -> None:
        def tMessage():
            try:
                if playerType == THUAI6.PlayerType.StudentPlayer:
                    studentType = Setting.studentType()[playerID]
                else:
                    studentType = THUAI6.StudentType.NullStudentType
                playerMsg = THUAI62Proto.THUAI62ProtobufPlayer(
                    playerID, playerType, studentType, Setting.trickerType()
                )
                for msg in self.__THUAI6Stub.AddPlayer(playerMsg):
                    with self.__cvMessage:
                        self.__haveNewMessage = True
                        self.__message2Client = msg
                        self.__cvMessage.notify()
                        with self.__mtxLimit:
                            self.__counter = 0
                            self.__counterMove = 0
            except grpc.RpcError as e:
                return

        threading.Thread(target=tMessage).start()
