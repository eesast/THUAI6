import PyAPI.structures as THUAI6
from PyAPI.Interface import IStudentAPI, ITrickerAPI, IAI
from typing import Union, Final, cast, List
from PyAPI.constants import Constants
import queue

import time

# 以下为自行添加的库
import math
import random
# import functools 


class Setting:
    # 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
    @staticmethod
    def asynchronous() -> bool:
        return True

    # 选手需要依次将player0到player4的职业都定义
    @staticmethod
    def studentType() -> List[THUAI6.StudentType]:
        return [THUAI6.StudentType.Athlete, THUAI6.StudentType.Athlete, THUAI6.StudentType.Athlete, THUAI6.StudentType.Athlete]

    @staticmethod
    def trickerType() -> THUAI6.TrickerType:
        return THUAI6.TrickerType.Assassin


# 辅助函数
numOfGridPerCell: Final[int] = 1000


class AssistFunction:

    @staticmethod
    def CellToGrid(cell: int) -> int:
        return cell * numOfGridPerCell + numOfGridPerCell // 2

    @staticmethod
    def GridToCell(grid: int) -> int:
        return grid // numOfGridPerCell


# -------------------分割线-----------------

class AI(IAI):
    def __init__(self, pID: int):
        self.__playerID = pID

    def StudentPlay(self, api: IStudentAPI) -> None:
        PreTime = time.time()*1000
        '''初始化'''
        global StudentInfo
        if 'StudentInfo' not in globals().keys():
            StudentInfo = InfoOfPlayers(api)

        global  GameInfo 
        if 'GameInfo' not in globals().keys(): 
            GameInfo = InfoOfGame(api)

        global Route0
        if 'Route0' not in globals().keys():
            Route0 = Routing()
            Route0.InitialRouteMap()
        
        global Message
        if 'Message' not in globals().keys():
            Message = MessageInfo()

        '''判断是否同帧部分'''
        if not GameInfo.upgradeFrameCount(api):
            return   #在同一帧则直接返回（可添加函数）
        '''学生信息更新'''
        StudentInfo.UpgradeStudentInfo(api)
        

        '''Tircker&Map信息更新'''
        StudentInfo.ViewTricker(api)
        finclassroom = []
        for [x,y] in GameInfo.UnFinClassRooms:
            if api.GetClassroomProgress(x,y) == 10000000:
                finclassroom.append([x,y])
        api.Print("View update classroom is")
        api.Print(GameInfo.upgradeClassRooms(finclassroom))

        openedgate = []
        for i in range(0,len(GameInfo.Gates)):
            if not GameInfo.IsGateOpen[i]:
                [x,y] = GameInfo.Gates[i]     
                if  api.GetGateProgress(x,y) == 18000:
                    openedgate.append([x,y])
        GameInfo.upgradeGates(openedgate)

        

        '''信息发送及接收'''
        Message.SendMessageToOther(StudentInfo, GameInfo, api)
        Message.ReceiveMessageFromOther(StudentInfo, GameInfo,api)


        api.Print(time.time()*1000-PreTime)
        PreTime = time.time()*1000

        '''控制学生移动'''

        StuPosCell = []
        for Student in StudentInfo.Students:
            StuPosCell = [AssistFunction.GridToCell(Student.x),
                        AssistFunction.GridToCell(Student.y)]
            if Student.playerID == StudentInfo.MyID:
                Route0.SetBeginNode(StuPosCell[0],StuPosCell[1])
            else :
                GameInfo.Map[StuPosCell[0]][StuPosCell[1]] = THUAI6.PlaceType.NullPlaceType #标注其他人的位置，防止互相碰撞

        '''设置寻路起点/终点并进行寻路'''
        if GameInfo.FinClassRoomNum < 7:
            Route0.SetGoalNodes(goalnodes=GameInfo.UnFinClassRooms,isgoalarrivabel=False)
        elif GameInfo.FinClassRoomNum >= 7:
            Route0.SetGoalNodes(goalnodes=GameInfo.Gates,isgoalarrivabel=False)
        Route0.FindRoute(GameInfo.Map)

        api.Print(time.time()*1000-PreTime)
        PreTime = time.time()*1000


        for i in range(0,4):
            if not StudentInfo.Students[i].playerID == StudentInfo.MyID:
                StuPosCell = [AssistFunction.GridToCell(StudentInfo.Students[i].x),
                        AssistFunction.GridToCell(StudentInfo.Students[i].y)]
                GameInfo.Map[StuPosCell[0]][StuPosCell[1]] = api.GetPlaceType(StuPosCell[0],StuPosCell[1]) #解除位置标注防止影响下一帧寻路
        # for i in range(0,Route0.RouteLenth):
        #     api.Print([Route0.Routes[i].x,Route0.Routes[i].y])
        #     api.Print("-----------------------------------")
        if Route0.RouteLenth:
            NextNode = Route0.GetNextNode()
            [NextX,NextY] = [NextNode.x,NextNode.y]
            if GameInfo.Map[NextX][NextY] == THUAI6.PlaceType.Window and api.GetSelfInfo().playerState != THUAI6.PlayerState.Climbing:
                api.SkipWindow()
            elif StudentInfo.IsStuck:   
                api.Print("Get Stuck!!!!!!!!!!")
                GetOffStuck(api)  #控制脱离卡死
            else:
                [NodeX,NodeY] = [AssistFunction.CellToGrid(NextNode.x),AssistFunction.CellToGrid(NextNode.y)]
                [Dis,Angle] = CalCulateMove([api.GetSelfInfo().x,api.GetSelfInfo().y],[NodeX,NodeY])
                MoveTime = round(Dis/api.GetSelfInfo().speed*1000)
                if MoveTime < 5: 
                    MoveTime = 5
                api.Move(MoveTime, Angle)
        
        elif GameInfo.FinClassRoomNum < 7 and api.GetSelfInfo().playerState != THUAI6.PlayerState.Learning: 
            api.StartLearning()
        elif GameInfo.FinClassRoomNum >=7:
            NowGoal:RouteNode = Route0.GetGoalNode()
            GoalNum = GameInfo.Gates.index([NowGoal.x,NowGoal.y])
            if not GameInfo.IsGateOpen[GoalNum] and api.GetSelfInfo().playerState != THUAI6.PlayerState.OpeningAGate:   #门还没开就开门
                api.StartOpenGate()
            elif GameInfo.IsGateOpen[GoalNum]:
                api.Graduate()
        
        '''记录信息以备下次使用'''
        StudentInfo.RecordStudentInfo()

        '''每个学生各自的代码'''
        if self.__playerID == 0:
            return

        elif self.__playerID == 1:
            # 玩家1执行操作
            return
        elif self.__playerID == 2:
            # 玩家2执行操作
            return
        elif self.__playerID == 3:
            # 玩家3执行操作
            return
                #可以写成if self.__playerID<2之类的写法

        return

    def TrickerPlay(self, api: ITrickerAPI) -> None:
        '''初始化'''
        global TrickerInfo
        if 'TrickerInfo' not in globals().keys():
            TrickerInfo = InfoOfTriker(api)

        global  GameInfo 
        if 'GameInfo' not in globals().keys(): 
            GameInfo = InfoOfGame(api)

        global Route1
        if 'Route1' not in globals().keys():
            Route1 = Routing()
            Route1.InitialRouteMap()

        '''判断是否同帧部分'''
        if not GameInfo.upgradeFrameCount(api):
            return   #在同一帧则直接返回（可添加函数）

        '''学生及自身信息更新'''
        TrickerInfo.UpgradeMyInfo(api)
        TrickerInfo.UpgradeStudentInfo(api)

        '''Map信息更新'''
        finclassroom = []
        for [x,y] in GameInfo.UnFinClassRooms:
            if api.GetClassroomProgress(x,y) == 10000000:
                finclassroom.append([x,y])

        openedgate = []
        for i in range(0,len(GameInfo.Gates)):
            if not GameInfo.IsGateOpen[i]:
                [x,y] = GameInfo.Gates[i]     
                if  api.GetGateProgress(x,y) == 18000:
                    openedgate.append([x,y])
        GameInfo.upgradeGates(openedgate)

        '''脱离卡死'''
        # if TrickerInfo.IsStuck:
        #     api.Print('GetStuck!!!!!!')
        #     GetOffStuck(api)
        #     return
        
        '''控制函数'''
        if TrickerInfo.CanViewStudents.count(True): #存在能够看见的学生
            StuDis = TrickerInfo.MyInfo.viewRange
            StuAngle = 0
            NearestStuNum = 0
            for i in range(0,4):
                if TrickerInfo.CanViewStudents[i]:
                    Student = TrickerInfo.Students[0]
                    for Stu in TrickerInfo.Students:
                        if Stu.playerID==i:
                            Student = Stu
                            break
                    [Dis,Angle] = CalCulateMove([TrickerInfo.MyInfo.x,TrickerInfo.MyInfo.y],
                     [Student.x,Student.y])
                    if Dis < StuDis:
                        StuDis = Dis
                        StuAngle = Angle
                        NearestStuNum = i
            for i in range(0,len(TrickerInfo.Students)):
                if TrickerInfo.Students[i].playerID == NearestStuNum:
                    NearestStuNum = i
                    break

            if StuDis < 3000 and TrickerInfo.MyInfo.playerState == THUAI6.PlayerState.Idle:
                api.UseSkill(1)
                api.Attack(StuAngle + random.randint(-50,50)/StuDis)
                return

            else:
                Route1.SetGoalNodes([[AssistFunction.GridToCell(TrickerInfo.Students[NearestStuNum].x),
                AssistFunction.GridToCell(TrickerInfo.Students[NearestStuNum].y)]],True)
        
        elif TrickerInfo.MyInfo.playerState == THUAI6.PlayerState.Idle:
            Route1.SetGoalNodes(GameInfo.UnFinClassRooms,False)
            '''这里有几种选择，一种是去算学生位置，另一种是按照固定路线巡逻，或者去守门'''  
            
        api.Print('Length of Goal is')
        api.Print(len(Route1.GoalNodes))
        Route1.SetBeginNode(AssistFunction.GridToCell(TrickerInfo.MyInfo.x),
            AssistFunction.GridToCell(TrickerInfo.MyInfo.y))
        Route1.FindRoute(GameInfo.Map)
        api.Print('Route Length:')
        api.Print(Route1.RouteLenth)
        api.Print('Begin Node:')
        api.Print([Route1.BeginNode.x,Route1.BeginNode.y])
        Goal:RouteNode = Route1.GetNextNode()
        [NodeX,NodeY] = [AssistFunction.CellToGrid(Goal.x),AssistFunction.CellToGrid(Goal.y)]
        api.Print('Next Node:')
        api.Print([Goal.x,Goal.y])
        [Dis,Angle] = CalCulateMove([TrickerInfo.MyInfo.x,TrickerInfo.MyInfo.y],[NodeX,NodeY])
        MoveTime = round(Dis/TrickerInfo.MyInfo.speed*1000)
        api.Print([MoveTime,Angle])
        if GameInfo.Map[Goal.x][Goal.y] == THUAI6.PlaceType.Window:
            api.SkipWindow()
        elif MoveTime < 5: 
            MoveTime = 5
        api.Move(MoveTime, Angle)
        return


def CalCulateMove(Begin: List[int],Goal: List[int]) -> List:  
    '''没有问题的直角坐标转极坐标函数，返回目标点间dis和angle
    '''
    Dis = math.sqrt((Begin[0] - Goal[0])**2 + (Begin[1] - Goal[1])**2)
    if Goal[0] - Begin[0] == 0:
        if Goal[1] - Begin[1] > 0: 
            Angle = math.pi/2 
        else: 
            Angle = -math.pi/2
    else : 
        Angle = math.atan((Goal[1] - Begin[1])/(Goal[0] - Begin[0])) 
        if Goal[0] - Begin[0] < 0:
            Angle = Angle + math.pi
    return [Dis,Angle]

def GetOffStuck(api:IStudentAPI) -> None:
    '''用于处理学生卡住时的情况'''
    selfinfo:THUAI6.Student = api.GetSelfInfo()
    [nowx,nowy] = [selfinfo.x,selfinfo.y]
    [cellx,celly] = [AssistFunction.GridToCell(nowx),AssistFunction.GridToCell(nowy)]
    [gridx,gridy] = [AssistFunction.CellToGrid(cellx),AssistFunction.CellToGrid(celly)]
    [dis,angle] = CalCulateMove([nowx,nowy], [gridx,gridy])
    MoveTime = round(dis/selfinfo.speed*1000)
    if MoveTime < 5:
        MoveTime = 5
    api.Move(MoveTime, angle)
    


class InfoOfGame:

    def __init__(self,api:IStudentAPI): 
        self.FrameCount :int = 0
        self.Map :List[List[THUAI6.PlaceType]] = api.GetFullMap()
        self.GameInfo = api.GetGameInfo()


        self.UnFinClassRooms : List[[int,int]] = []
        self.FinClassRooms : List[[int,int]] = []
        self.FinClassRoomNum : int = 0
        self.Gates : List[[int,int]] = []
        self.IsGateOpen: List[bool] = []  #由于Gate在开启之后仍然需要使用，因此需要记录其状态
        self.HiddenGatesPlace: List[[int,int]] = []
        '''只存刷新点'''
        self.HiddenGates: List[[int,int]] = []
        '''真正的隐藏门'''
        
        self.InitAllPlaceList()

    def upgradeFrameCount(self,api:IStudentAPI) -> bool:
        GameTime = api.GetGameInfo().gameTime
        framecount = round(GameTime/50)
        ''' 返回真表明帧数有更新，否则没有'''
        if self.FrameCount == framecount:
            return False
        else :
            self.FrameCount = framecount
            return True

    def upgradeMap(self,map:[int,int,THUAI6.PlaceType]) -> bool:
        ''' 可输入某个地图的点或整个地图
        输入点时格式为[x,y,MapType]
        输入地图时格式为List[List[THUAI6.PlaceType]]
        返回真表明有更新，否则没有'''
        if len(map) == 3:
            [x ,y ,MapType] = map
            if self.Map[x][y] == MapType:
                return False
            else :
                self.Map[x][y] = MapType
                return True

        else: 
            if self.Map == map:
                return False
            else:
                self.Map = map
                return True

    def InitAllPlaceList(self) ->None:
        '''依据地图更新教室/大门等所在地点'''
        for x in range(0,50):
            for y in range(0,50):
                if self.Map[x][y] == THUAI6.PlaceType.ClassRoom:
                    self.UnFinClassRooms.append([x,y])
                elif self.Map[x][y] == THUAI6.PlaceType.Gate:
                    self.Gates.append([x,y])
                    self.IsGateOpen.append(False)
                elif self.Map[x][y] == THUAI6.PlaceType.HiddenGate:
                    self.HiddenGatesPlace.append([x,y])
    

    def upgradeClassRooms(self,ClassRooms:[[int,int]]) ->bool:
        '''将传入列表中的教室设定为已完成并同时更改地图，有变化返回True'''
        if len(ClassRooms) == 0:
            return False
        else:
            change = 0
            for ClassRoom in ClassRooms:
                if self.UnFinClassRooms.count(ClassRoom):
                    i = self.UnFinClassRooms.index(ClassRoom)
                    self.UnFinClassRooms.pop(i)
                    self.FinClassRooms.append(ClassRoom)
                    self.upgradeMap([ClassRoom[0],ClassRoom[1],THUAI6.PlaceType.Wall]) #认为已完成的作业为wall
                    change = change +1 
            self.FinClassRoomNum = len(self.FinClassRooms)
            return (change==0)
    
    def upgradeGates(self,Gates:[[int,int]]) ->bool:
        '''将传入列表中的校门设定为已开启，有变化返回True'''
        if len(Gates) == 0:
            return False
        else:
            change = 0
            for Gate in Gates:
                if self.Gates.count(Gate):
                    i = self.Gates.index(Gate)
                    if not self.IsGateOpen[i]:
                        self.IsGateOpen[i] = True
                        change = change +1 
            return (change==0)


class InfoOfTriker:
    
    ''' Tricker阵营用函数'''
    def __init__(self,api: ITrickerAPI):
        self.MyInfo : THUAI6.Tricker = api.GetSelfInfo()
        self.PreMyInfo = []
        self.Students : List[THUAI6.Student] = []
        #学生乱序排列
        self.CanViewStudents : List[bool] = [False,False,False,False]
        #依照学生ID排列
        self.UpgradeStudentInfo(api)
        
        self.Bullets :List[THUAI6.Bullet] = api.GetBullets()


    def UpgradeStudentInfo(self,api:ITrickerAPI) ->None:
        '''在程序运行初更新学生信息'''
        Students :List[THUAI6.Student]= api.GetStudents()
        self.CanViewStudents = [False,False,False,False]
        if len(Students):
            for Student in Students:
                ID = Student.playerID
                self.CanViewStudents[ID] = True
                HaveStudent = False
                if len(self.Students):
                    for StuNum in range(0,len(self.Students)):
                        if self.Students[StuNum].playerID == ID:
                            self.Students[StuNum] = Student
                            HaveStudent = True           
                if not HaveStudent:
                    self.Students.append(Student)
        return

    def UpgradeMyInfo(self,api:ITrickerAPI) ->None:
        ''' 更新自身信息并比较前一帧信息'''
        self.PreMyInfo = self.MyInfo
        self.MyInfo = api.GetSelfInfo()
        if self.MyInfo.playerState == THUAI6.PlayerState.Idle and self.MyInfo.x == self.PreMyInfo.x and self.MyInfo.y == self.PreMyInfo.y:
            self.IsStuck = True
        else :
            self.IsStuck = False


class InfoOfPlayers:
    
    ''' Save the info of students including myself,and tricker(if can see)'''
    def __init__(self,api: IStudentAPI):
        self.Students : List[THUAI6.Student] = api.GetStudents()
        '''注意顺序并不按照player排，是随机的'''
        self.MyID :int = api.GetSelfInfo().playerID
        self.PreSelfPos: [int,int] = []
        self.IsStuck:bool =False

        self.ViewTricker(api)
        if  self.CanViewTricker == True:
            return
        else:
            '''主要是Tricker没有定义init函数，超麻烦'''
            self.Tricker: List[THUAI6.Tricker] = [THUAI6.Tricker(),]
            #需要声明这些变量否则无法使用
            self.Tricker[0].x = 0
            self.Tricker[0].y = 0
            self.Tricker[0].viewRange = 0 
            self.Tricker[0].trickerType = THUAI6.TrickerType.NullTrickerType
            return
        
        self.Bullets = api.GetBullets()


    def UpgradeStudentInfo(self,api:IStudentAPI) ->None:
        '''在程序运行初更新学生信息'''
        self.Students = api.GetStudents()
        if api.GetSelfInfo().playerState == THUAI6.PlayerState.Idle and self.PreSelfPos == [api.GetSelfInfo().x,api.GetSelfInfo().y]:
            self.IsStuck = True
        else :
            self.IsStuck = False

    def RecordStudentInfo(self) ->None:
        '''在程序最后记录学生信息以便下次对比'''
        for i in range(0,4):
            if self.Students[i].playerID == self.MyID:
                self.PreSelfPos = [self.Students[i].x,self.Students[i].y]

    def ViewTricker(self,api:IStudentAPI)->None:
        '''函数会自动更新Tricker信息,该函数应当在每帧开头调用一次'''
        Tricker = api.GetTrickers()
        if len(Tricker):
            self.Tricker = Tricker
            self.CanViewTricker = True
        else:
            self.CanViewTricker = False

class MessageInfo:

    def __init__(self):
        self.Message: List[str] = ['','','','']
        '''message按照studentID储存，自己为发送，别人为接收'''

    def SendMessageToOther(self,StudentInfo: InfoOfPlayers,GameInfo:InfoOfGame,api:IStudentAPI) ->None:
        self.MessageEncode(StudentInfo,GameInfo)
        for i in range(0,4):
            if StudentInfo.MyID != i:
                api.SendMessage(i,self.Message[StudentInfo.MyID])
        return

    def MessageEncode(self,StudentInfo:InfoOfPlayers,GameInfo:InfoOfGame) ->None:
        '''Important Info In Messages'''
        message = str()
        '''Student Info'''  #这部分貌似被getStudent替代了，问题不大

        '''Tricker Info'''
        if StudentInfo.CanViewTricker:
            '''能看见才发送，否则不发'''
            message = message + 'T' + str(StudentInfo.Tricker[0].x) + '?' + str(StudentInfo.Tricker[0].y) + '/'

        '''Map Info'''
        if len(GameInfo.FinClassRooms):  
            for ClassRoom in GameInfo.FinClassRooms:  #只发送已完成的教室
                message = message + 'C'+ str(ClassRoom[0]) + '?' + str(ClassRoom[1]) + '/'   
        
        for Gate in GameInfo.Gates:
            gatenum = GameInfo.Gates.index(Gate)
            if GameInfo.IsGateOpen[gatenum]:   #只发送已完成的大门
                message = message + 'G' + str(gatenum) + '/'
        
        self.Message[StudentInfo.MyID] = message

    def ReceiveMessageFromOther(self,StudentInfo: InfoOfPlayers,GameInfo:InfoOfGame,api:IStudentAPI) ->None:
        while api.HaveMessage():
            MessageTuple = api.GetMessage()
            self.Message[MessageTuple[0]] = MessageTuple[1]
            self.MessageDecode(StudentInfo, GameInfo, MessageTuple[0])

    def MessageDecode(self,StudentInfo: InfoOfPlayers,GameInfo:InfoOfGame,playerID:int)->None:

        ClassRooms = []

        DecodedMessage = self.Message[playerID].split(sep='/')
        while len(DecodedMessage):
            MessageUnit :str = DecodedMessage.pop()
            if len(MessageUnit) == 0: 
                continue
                '''Tricker Info'''
            elif MessageUnit[0] == 'T' and not StudentInfo.CanViewTricker: #只有自己看不见的时候才会听别人的
                [x,y] = MessageUnit[1:].split(sep='?')
                [StudentInfo.Tricker[0].x,StudentInfo.Tricker[0].y] = [int(x),int(y)]

                '''Map Info'''
            elif MessageUnit[0] == 'C':
                [x,y] = MessageUnit[1:].split(sep='?')
                ClassRooms.append([int(x),int(y)])
            elif MessageUnit[0] == 'G':
                gatenum = int(MessageUnit[1:])
                GameInfo.IsGateOpen[gatenum] = True

        GameInfo.upgradeClassRooms(ClassRooms)


# 用于计算最短路径
class RouteNode:  

    def __init__(self,x: int = 0,y: int = 0,dis: int = 114514) -> None:
        self.x :int= x
        self.y :int= y
        self.IsRouted :bool= False
        ''' 由于每帧都需要做一次更新，因此该变量不保持为定值，而是在二值变换'''
        self.prex :int= x
        self.prey :int= y
        self.Distance :float = dis

    # 以下重载运算符函数用于优先队列以及判断两点坐标是否相同   更改可能导致报错
    def __gt__(self,other):
        if isinstance(other, RouteNode):
            return self.Distance > other.Distance
        elif isinstance(other, Number):
            return self.Distance > other
        else:
            raise AttributeError("The of other is incorrect!")

    def __eq__(self, other):
        if isinstance(other, RouteNode):
            return self.x == other.x and self.y == other.y
        else:
            raise AttributeError("The of other is incorrect!")

    def __lt__(self,other): 
        if isinstance(other, RouteNode):
            return self.Distance < other.Distance
        elif isinstance(other, Number):
            return self.Distance < other
        else:
            raise AttributeError("The of other is incorrect!")


# 用于寻路的类
class Routing:

    def __init__(self):
        self.RouteLenth = 114514
        self.BeginNode = RouteNode()
        self.GoalNode = RouteNode()  #用来记录最终的目标点，该目标点可能无法到达
        self.GoalNodes : List[RouteNode] = []
        self.IsGoalArrivabel = False  #用来确认是走九宫格还是直接走到目标点处
        self.Routes :List[RouteNode] = []
        self.RouteMap :List[List[RouteNode]]= []

    def InitialRouteMap(self) ->None:
        '''初始化寻路地图'''
        for i in range(0,50):
            self.RouteMap.append([])
            for j in range(0,50):
                Node = RouteNode(i,j)
                self.RouteMap[i].append(Node)

    def GetNextNode(self) -> RouteNode:
        '''用于获取下一个需要前往的路径点'''
        return self.Routes[-1]

    def GetGoalNode(self)->RouteNode:
        '''用于获取寻路的最终目标点'''
        return self.GoalNode

    def SetGoalNodes(self,goalnodes :[[int,int]] = [],isgoalarrivabel:bool = False) ->None:
        '''设置终点为goalnotes，,isgoalarrivabel为假则将终点设置在周围'''
        self.IsGoalArrivabel = isgoalarrivabel
        self.GoalNodes = []
        if len(goalnodes):
            for [x,y] in goalnodes:
                Node = RouteNode(x,y)
                self.GoalNodes.append(Node)

    def SetBeginNode(self,x: int,y: int):
        self.BeginNode.x = x
        self.BeginNode.y = y
        self.BeginNode.Distance = 0

    def ShortingRoute(self,map:List[List[THUAI6.PlaceType]])->None:
        '''用于优化并缩短路径'''
        # for i in range(0,self.RouteLenth):
        #     for j in range(i,self.RouteLenth):
        #         BeginNode = self.Routes[i]
        return

    def FindRoute(self,map:List[List[THUAI6.PlaceType]]):
        '''
        使用的寻路方法为从起点向周围扩散并最终得到所有点到起点的最短距离

        可能出现的问题：
        在寻找最近的目标点时必定收敛，但是寻找最远点或者次元点未必收敛
        （可能在两个位置间来回振荡，因为进行的是动态优化）

        可以进行改进的地方：由于加入了极坐标方向系统，所以没必要一定得走中间
        (可以添加拐弯函数来优化最后的路线) 
        (只要观察从目前位置 到终点前的 某一个点是否能拉直线就行)

        Length 之后可以用来调节权重

        '''
        q = queue.PriorityQueue()
        q.put(self.BeginNode)
        BeginNodeState = ~self.RouteMap[self.BeginNode.x][self.BeginNode.y].IsRouted
        self.RouteMap[self.BeginNode.x][self.BeginNode.y].IsRouted = BeginNodeState #取反即可
        self.RouteMap[self.BeginNode.x][self.BeginNode.y].Distance = 0
        while not q.empty():
            Node :RouteNode = q.get()
            # if Node.Distance > 2500:
            #     break
                #算出所有点到起点的距离
            for [i,j] in [[Node.x - 1,Node.y ],[Node.x + 1,Node.y ],[Node.x ,Node.y - 1],[Node.x ,Node.y + 1]]:
            # 遍历四个方向
                if 0 <= i < 50 and  0 <= j < 50 and self.RouteMap[i][j].IsRouted != BeginNodeState:
                        self.RouteMap[i][j].IsRouted = BeginNodeState
                        #判断该点路径是否已经确定
                        if [THUAI6.PlaceType.Land,THUAI6.PlaceType.Window,THUAI6.PlaceType.Grass].count(map[i][j]): 
                            self.RouteMap[i][j].Distance = Node.Distance + 1
                            self.RouteMap[i][j].prex = Node.x
                            self.RouteMap[i][j].prey = Node.y
                        else:
                            self.RouteMap[i][j].Distance = 114514  #认为该点不可经过，但是需要入栈以保证最终所有点的IsRouted 与 BeginNodeState 保持一致
                            # 只追求计算速度而忽略稳定性时可删除该部分
                        q.put(self.RouteMap[i][j])
        
        # 用来确定去哪一个endnode和该endnode对应的goal
        EndNode = self.RouteMap[0][0] #肯定无法到达的点
        if self.IsGoalArrivabel:
            for k in range(0,len(self.GoalNodes)):
                if  self.RouteMap[self.GoalNodes[k].x][self.GoalNodes[k].y].Distance < EndNode.Distance:
                    EndNode = self.RouteMap[self.GoalNodes[k].x][self.GoalNodes[k].y]
                    self.GoalNode = EndNode
        elif not self.IsGoalArrivabel:
            for goalnode in self.GoalNodes:
                [i,j] = [goalnode.x,goalnode.y]
                for [ii,jj] in [[i + 1,j],    #九宫格内即可
                                [i + 1,j - 1],
                                [i - 1,j + 1],
                                [i - 1,j - 1],
                                [i + 1,j + 1 ],
                                [i - 1,j ],
                                [i ,j + 1],
                                [i ,j - 1]]:
                    if 0 <= ii < 50 and  0 <= jj < 50 and self.RouteMap[ii][jj].Distance < EndNode.Distance:
                        EndNode = self.RouteMap[ii][jj]
                        self.GoalNode = goalnode

        # 返回路径
        self.Routes = []
        PreNode = EndNode
        while PreNode.Distance :
            self.Routes.append(PreNode)
            PreNode = self.RouteMap[PreNode.prex][PreNode.prey]
        
        self.ShortingRoute(map)
        self.RouteLenth = len(self.Routes)


                    

            


