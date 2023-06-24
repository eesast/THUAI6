import PyAPI.structures as THUAI6
from PyAPI.Interface import IStudentAPI, ITrickerAPI, IAI,IAPI
from typing import Union, Final, cast, List

import PyAPI.constants as Constants
import queue
import math
import time
import random



class Setting:
    # 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
    @staticmethod
    def asynchronous() -> bool:
        return True

    # 选手需要依次将player0到player4的职业都定义
    @staticmethod
    def studentType() -> List[THUAI6.StudentType]:
        return [THUAI6.StudentType.StraightAStudent, THUAI6.StudentType.StraightAStudent, THUAI6.StudentType.StraightAStudent, THUAI6.StudentType.Sunshine]

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


class AI(IAI):
    initial = True              # 控制初始化
    mapInfo = list(list())      # 地图信息
    mapdict = {}                # 地图字典
    path = []                   # 人物的路径
    state = 1                   # 人物的状态 1为修机或开门 2为溜鬼 3为救人中
    learnstate = 0              # 人物的修机状态 0为未修机 1为在修机 2为修完机找大门 3为在开大门 4为找箱子 5为开箱子
    classroom = (-1,-1)         # 人物在修的机子的位置
    finishnum = 0               # 场上修完机子的数量
    classroomprogress = {}      # 场上已修过的机子信息
    gate = (-1,-1)              # 人物在开的大门位置
    gateprogress = {}           # 场上已修过的大门信息
    closeddoors = {}            # 场上关闭的门信息
    trickerPastInfo = []        # 用于溜鬼的鬼坐标
    trickerPosition=[]          # 最新看到的鬼坐标 用于找机找门
    studentType = ""            # 职业
    alertnessRadius = 0         # 警戒范围半径
    chestprogress = {}          # 场上开过的箱子信息
    stuposition = {}            # 场上除自己外学生的位置
    canRescue = False           # 是否能救人
    rescueid=-1                 # 救人目标
    needrescue=False            # 救人请求
    needRescueList=[False,False,False,False] # 是否需要救人
    doingrescue=False           # 场上的救人声明
    #messages=[]                 # 每帧收到的信息
    framecount=0                # 记录帧数的变量
    escapelist=[]               # 处于溜鬼状态的队友列表
    targetclassroom=()          # 即将要修的机子目标
    targetgate=()               # 即将要开的大门目标
    trickertype=0               # 捣蛋鬼类型
    topTarget=()                # 不顾一切要前往的目标
    
    


    def __init__(self, pID: int):
        self.__playerID = pID



    #地图处理,获得一个地图字典
    def MapProcess(self, mapInfo) -> dict:
        mapdict={'Null':[],'Land':[],'Wall':[],'Grass':[],'ClassRoom':[],'Gate':[],'HiddenGate':[],'Window':[],'Door3':[],'Door5':[],'Door6':[],'Chest':[]}
        for i in range(50):
            for j in range(50):
                if i==48 and j==19 and mapInfo[5][6].name=='Gate':
                    mapdict['Wall'].append((i,j))
                    continue
                if mapInfo[i][j].name=='NullPlaceType':
                    mapdict["Null"].append((i,j)) 
                elif mapInfo[i][j].name=='Land':
                    mapdict['Land'].append((i,j))
                elif mapInfo[i][j].name=='Wall':
                    mapdict['Wall'].append((i,j))
                elif mapInfo[i][j].name=='Grass':
                    mapdict['Grass'].append((i,j))
                elif mapInfo[i][j].name=='ClassRoom':
                    mapdict['ClassRoom'].append((i,j))
                elif mapInfo[i][j].name=='Gate':
                    mapdict['Gate'].append((i,j))
                elif mapInfo[i][j].name=='HiddenGate':
                    mapdict['HiddenGate'].append((i,j))
                elif mapInfo[i][j].name=='Window':
                    mapdict['Window'].append((i,j))
                elif mapInfo[i][j].name=='Door3':
                    mapdict['Door3'].append((i,j))
                elif mapInfo[i][j].name=='Door5':
                    mapdict['Door5'].append((i,j))
                elif mapInfo[i][j].name=='Door6':
                    mapdict['Door6'].append((i,j))
                elif mapInfo[i][j].name=='Chest':
                    mapdict['Chest'].append((i,j))
        return mapdict

    # 学生职业获取及常数设定
    def SetStudentType(self, api: IStudentAPI) -> None:
        selfInfo = api.GetSelfInfo()
        if selfInfo.studentType.name == "Athlete":
            self.alertnessRadius = Constants.Athlete.alertnessRadius
            self.studentType = "Athlete"
        elif selfInfo.studentType.name == "Teacher":
            self.alertnessRadius = Constants.Teacher.alertnessRadius
            self.studentType = "Teacher"
        elif selfInfo.studentType.name == "StraightAStudent":
            self.alertnessRadius = Constants.StraightAStudent.alertnessRadius
            self.studentType = "StraightAStudent"
        elif selfInfo.studentType.name == "Robot":
            self.alertnessRadius = Constants.Robot.alertnessRadius
            self.studentType = "Robot"
        elif selfInfo.studentType.name == "TechOtaku":
            self.alertnessRadius = Constants.TechOtaku.alertnessRadius
            self.studentType = "TechOtaku"
        elif selfInfo.studentType.name == "Sunshine":
            self.alertnessRadius = Constants.Sunshine.alertnessRadius
            self.studentType = "Sunshine"
    

    #某点的临近可走的点
    def neighbors(self,current:tuple,api:IAPI)->list:
        X=current[0]
        Y=current[1]
        neighbor=list()
        for i in [X-1,X,X+1]:
            for j in [Y-1,Y,Y+1]:
                if(i>49 or i<0 or j>49 or j<0):continue  #边界处理
                if(i==X and j==Y):continue
                #如果队友在这一格
                if (i,j)in self.stuposition.values():continue
                #如果是空地或草地
                if (i,j)in self.mapdict['Land']or (i,j)in self.mapdict['Grass'] :
                    if i!=X and j!=Y:#此格为斜着方向的格子
                        if ((X,j) not in self.mapdict['Land'] and (X,j)not in self.mapdict['Grass'] or (X,j)in self.stuposition.values() or (X,j) in self.closeddoors.keys())\
                        and ((i,Y) not in self.mapdict['Land'] and (i,Y)not in self.mapdict['Grass']  or  (i,Y) in self.stuposition.values() or (i,Y) in self.closeddoors.keys()):
                            continue   
                    neighbor.append((i,j))
                #如果是窗 需要站在前后一格内
                elif (i,j)in self.mapdict['Window']:
                    if i==X or j==Y:
                        neighbor.append((i,j))
                #如果是教室门
                elif (i,j)in self.mapdict['Door3']or (i,j)in self.mapdict['Door5']or (i,j)in self.mapdict['Door6']:
                    if (i,j)not in self.closeddoors.keys():
                        neighbor.append((i,j))
        return neighbor


    #从当前点到周边某点的代价
    def cost(self,current,next)->float:
        x,y=current
        x1,y1=next
        #斜着走
        if abs(x-x1)==1 and abs(y-y1)==1:
            return 1.414
        #翻窗
        elif (x1,y1)in self.mapdict['Window']:
            return 1.25 #这里翻窗的数值需要重新调
        #水平或竖直移动
        else: return 1
    
    #评估函数  先写的欧几里得距离
    def heuristic(self,a:tuple,b:tuple)->float:
        x,y=a
        x1,y1=b
        return math.sqrt((x-x1)**2+(y-y1)**2)
    
    #A*算法 输入起始点和终点，返回一个路径list，每个元素是一个元组代表一个格子
    def a_star_search(self,start,goal,api:IAPI)->list:
        frontier=queue.PriorityQueue()
        frontier.put((0,start))
        came_from={}
        cost_so_far={}
        came_from[start]=None
        cost_so_far[start]=0
        while not frontier.empty():
            current=frontier.get()[1]
            if current==goal:
                break
            for next in self.neighbors(current,api):

                new_cost=cost_so_far[current]+self.cost(current,next)
                if next not in cost_so_far.keys() or new_cost < cost_so_far[next]:
                    cost_so_far[next]=new_cost
                    priority=new_cost+self.heuristic(goal,next)
                    frontier.put((priority,next))
                    came_from[next]=current

        path=list()
        i=goal
        path.append(i)
        api.Print(str(came_from))
        if i not in came_from.keys():
            api.Print("路径错误")
            return []
        while came_from[i]!=None:
            path.append(came_from[i])
            i=came_from[i]
        path.reverse()
        return path
    
    #移动函数  沿给定路径移动一格
    def MoveTo(self,path,api:IAPI)->bool:
        x=api.GetSelfInfo().x
        y=api.GetSelfInfo().y
        X=AssistFunction.GridToCell(x)
        Y=AssistFunction.GridToCell(y)
        #current=(X,Y)
        #获取从当前位置到目标的最优路径
        #path=self.a_star_search(current,target,api)

        #X1,Y1=path[0]
        
        #api.Print(str((X1,Y1)))
        #api.Print(str((X,Y)))
        #api.Print(str(path))

        if len(path)<=1:#出现错误
            return False

        #判断是否在格子中心正负100内，不在则微调
        if abs(x-1000*X-500)>100 or abs(y-1000*Y-500)>100:
            self.micromove(api)

        x1,y1=path[1]#每次只移动一格
        speed=api.GetSelfInfo().speed


        if (x1,y1)in self.mapdict['Window']:#如果是窗
            api.SkipWindow()
            time.sleep(1.2)
            api.Wait()
            
        else:#不是窗
            if (x1-X,y1-Y)==(-1,-1):#往左上方走
                if (X-1,Y)not in self.mapdict['Land']and (X-1,Y)not in self.mapdict['Grass']or(X-1,Y)in self.stuposition.values():#上面是墙，就先往左再往上
                    api.Move(int(1000*1000/speed),1.5*math.pi)
                    time.sleep(1100/speed)
                    api.Move(int(1000*1000/speed),math.pi)
                    time.sleep(1100/speed)
                elif (X,Y-1)not in self.mapdict['Land']and (X,Y-1)not in self.mapdict['Grass'] or (X,Y-1)in self.stuposition.values():#左边是墙，就先往上再往左
                    api.Move(int(1000*1000/speed),math.pi)
                    time.sleep(1100/speed)
                    api.Move(int(1000*1000/speed),1.5*math.pi)
                    time.sleep(1100/speed)
                else:
                    api.Move(int(1414*1000/speed),1.25*math.pi)
                    time.sleep(1514/speed)
            elif (x1-X,y1-Y)==(-1,0):
                api.Move(int(1000*1000/speed),math.pi)
                time.sleep(1100/speed)
            elif (x1-X,y1-Y)==(-1,1):#往右上方走
                if (X-1,Y)not in self.mapdict['Land']and(X-1,Y)not in self.mapdict['Grass'] or (X-1,Y) in self.stuposition.values():#上面是墙，就先往右再往上
                    api.Move(int(1000*1000/speed),0.5*math.pi)
                    time.sleep(1100/speed)
                    api.Move(int(1000*1000/speed),math.pi)
                    time.sleep(1100/speed)
                elif (X,Y+1)not in self.mapdict['Land']and(X,Y+1)not in self.mapdict['Grass'] or(X,Y+1) in self.stuposition.values():#右边是墙，就先往上再往右
                    api.Move(int(1000*1000/speed),math.pi)
                    time.sleep(1100/speed)
                    api.Move(int(1000*1000/speed),0.5*math.pi)
                    time.sleep(1100/speed)
                else:
                    api.Move(int(1414*1000/speed),0.75*math.pi)
                    time.sleep(1514/speed)
            elif (x1-X,y1-Y)==(0,-1):
                api.Move(int(1000*1000/speed),1.5*math.pi)
                time.sleep(1100/speed)
            elif (x1-X,y1-Y)==(0,1):
                api.Move(int(1000*1000/speed),0.5*math.pi)
                time.sleep(1100/speed)
            elif (x1-X,y1-Y)==(1,-1):#往左下方走
                if (X+1,Y)not in self.mapdict['Land']and(X+1,Y)not in self.mapdict['Grass'] or(X+1,Y) in self.stuposition.values():#下面是墙，就先往左再往下
                    api.Move(int(1000*1000/speed),1.5*math.pi)
                    time.sleep(1100/speed)
                    api.Move(int(1000*1000/speed),0)
                    time.sleep(1100/speed)
                elif (X,Y-1)not in self.mapdict['Land']and(X,Y-1)not in self.mapdict['Grass'] or(X,Y-1) in self.stuposition.values():#左边是墙，就先往下再往左
                    api.Move(int(1000*1000/speed),0)
                    time.sleep(1100/speed)
                    api.Move(int(1000*1000/speed),1.5*math.pi)
                    time.sleep(1100/speed)
                else:
                    api.Move(int(1414*1000/speed),1.75*math.pi)
                    time.sleep(1514/speed)
            elif (x1-X,y1-Y)==(1,0):
                api.Move(int(1000*1000/speed),0)
                time.sleep(1100/speed)
            elif (x1-X,y1-Y)==(1,1):#往右下方走
                if (X+1,Y)not in self.mapdict['Land'] and (X+1,Y) not in self.mapdict['Grass'] or(X+1,Y) in self.stuposition.values():#下面是墙，就先往右再往下
                    api.Move(int(1000*1000/speed),0.5*math.pi)
                    time.sleep(1100/speed)
                    api.Move(int(1000*1000/speed),0)
                    time.sleep(1100/speed)
                elif (X,Y+1)not in self.mapdict['Land']and(X,Y+1)not in self.mapdict['Grass'] or(X,Y+1)in self.stuposition.values():#右边是墙，就先往下再往右
                    api.Move(int(1000*1000/speed),0)
                    time.sleep(1100/speed)
                    api.Move(int(1000*1000/speed),0.5*math.pi)
                    time.sleep(1100/speed)
                else:
                    api.Move(int(1414*1000/speed),0.25*math.pi)
                    time.sleep(1514/speed)
        return True

    #微移动，从当前位置移到当前格子中心
    def micromove(self,api:IAPI)->None:
        x=api.GetSelfInfo().x
        y=api.GetSelfInfo().y
        X=AssistFunction.GridToCell(x)
        Y=AssistFunction.GridToCell(y)
        x_goal=1000*X+500
        y_goal=1000*Y+500

        speed=api.GetSelfInfo().speed
        dis=math.sqrt((x-x_goal)**2+(y-y_goal)**2)
        if x_goal>x:
            angle=math.atan((y_goal-y)/(x_goal-x))
        elif x_goal==x:
            if y_goal>y:angle=0.5*math.pi
            else :angle=1.5*math.pi
        else:
            angle=math.pi-math.atan((y_goal-y)/(x-x_goal))
        api.Move(int(dis*1000/speed),angle)
        time.sleep(dis/speed)

    #自身到target距离
    def Distance(self,target,api:IAPI)->float:
        x=api.GetSelfInfo().x
        y=api.GetSelfInfo().y
        x=AssistFunction.GridToCell(x)
        y=AssistFunction.GridToCell(y)
        (x1,y1)=target
        return math.sqrt((x-x1)**2+(y-y1)**2) 
    
    #自身到某机子的评估距离 计算方式为欧几里得距离＋修完该机所需时间×移动速度/10+80000/鬼离机子的距离
    def classroomdis(self,target,api:IStudentAPI)->float:
        x=api.GetSelfInfo().x
        y=api.GetSelfInfo().y
        x=AssistFunction.GridToCell(x)
        y=AssistFunction.GridToCell(y)
        (x1,y1)=target
        if target not in self.classroomprogress.keys():#机子没修过
            lspeed=1000*api.GetSelfInfo().learningSpeed
            speed=api.GetSelfInfo().speed
            if len(self.trickerPosition)==2:
                x2=self.trickerPosition[0]
                y2=self.trickerPosition[1]
                trickerdis=math.sqrt((x1-x2)**2+(y1-y2)**2)
                return math.sqrt((x-x1)**2+(y-y1)**2)*1000+(10000000)*speed/(lspeed*10)+80000/(trickerdis)#修机进度权重除以十
            else:
                return math.sqrt((x-x1)**2+(y-y1)**2)*1000+(10000000)*speed/(lspeed*10)#修机进度权重除以十
        
        else:#机子已经修过
            classprogress=self.classroomprogress[target]
            if classprogress==10000000:#机子修完了
                return math.inf #返回无穷大，即不去已经修完的机子
            else:
                lspeed=1000*api.GetSelfInfo().learningSpeed
                speed=api.GetSelfInfo().speed
                if len(self.trickerPosition)==2:
                    x2=self.trickerPosition[0]
                    y2=self.trickerPosition[1]
                    trickerdis=math.sqrt((x1-x2)**2+(y1-y2)**2)
                    return math.sqrt((x-x1)**2+(y-y1)**2)*1000+(10000000-classprogress)*speed/(lspeed*10)+80000/trickerdis#修机进度权重除以十
                else:
                    return math.sqrt((x-x1)**2+(y-y1)**2)*1000+(10000000-classprogress)*speed/(lspeed*10)#修机进度权重除以十                
  
            
    #返回离自身“最近”的机子坐标
    def Searchclassroom(self,api:IStudentAPI)->tuple:
        classlist=self.mapdict['ClassRoom']
        classdistlist=[]
        for classroom in classlist:
            classdistlist.append(self.classroomdis(classroom,api))
        return classlist[classdistlist.index(min(classdistlist))]                         


    #自身到某大门的评估距离 计算方式为欧几里得距离＋开完该门所需时间×移动速度/10+150000/鬼离大门的距离
    def gatedis(self,target,api:IStudentAPI)->float:
        x=api.GetSelfInfo().x
        y=api.GetSelfInfo().y
        x=AssistFunction.GridToCell(x)
        y=AssistFunction.GridToCell(y)
        (x1,y1)=target
        if target not in self.gateprogress.keys():#门没开过
            lspeed=1000*5000
            speed=api.GetSelfInfo().speed
            if len(self.trickerPosition)==2:#已知上一次鬼的位置
                x2=self.trickerPosition[0]
                y2=self.trickerPosition[1]
                trickerdis=math.sqrt((x1-x2)**2+(y1-y2)**2)
                return math.sqrt((x-x1)**2+(y-y1)**2)*1000+(18000)*speed/(lspeed*10)+150000/trickerdis
            else:#未知鬼的位置
                return math.sqrt((x-x1)**2+(y-y1)**2)*1000+(18000)*speed/(lspeed*10)    
        else:#门已经开过
            gateprogress=self.gateprogress[target]
            lspeed=1000*5000#开门速度
            speed=api.GetSelfInfo().speed
            if len(self.trickerPosition)==2:
                x2=self.trickerPosition[0]
                y2=self.trickerPosition[1]
                trickerdis=math.sqrt((x1-x2)**2+(y1-y2)**2)
                return math.sqrt((x-x1)**2+(y-y1)**2)*1000+(18000-gateprogress)*speed/(lspeed*10)+80000/trickerdis
            else:
                return math.sqrt((x-x1)**2+(y-y1)**2)*1000+(18000-gateprogress)*speed/(lspeed*10)               
            
    #返回离自身“最近”的大门坐标
    def Searchgate(self,api:IStudentAPI)->tuple:
        gatelist=self.mapdict['Gate']
        gatedistlist=[]
        for gate in gatelist:
            gatedistlist.append(self.gatedis(gate,api))
        return gatelist[gatedistlist.index(min(gatedistlist))]                         

    #自身到某箱子的评估距离 计算方式为欧几里得距离+打开该箱子所需时间×移动速度
    def chestdis(self,target,api:IStudentAPI)->float:
        x=api.GetSelfInfo().x
        y=api.GetSelfInfo().y
        x=AssistFunction.GridToCell(x)
        y=AssistFunction.GridToCell(y)
        (x1,y1)=target
        if target not in self.chestprogress.keys():#箱子没开过
            cspeed=1000
            speed=api.GetSelfInfo().speed
            return math.sqrt((x-x1)**2+(y-y1)**2)*1000+(10000000)*speed/cspeed
        
        else:#箱子已经开过
            chestprogress=self.chestprogress[target]
            if chestprogress==10000000:#机子修完了
                return math.inf
            else:
                cspeed=1000
                speed=api.GetSelfInfo().speed
                return math.sqrt((x-x1)**2+(y-y1)**2)*1000+(10000000-chestprogress)*speed/cspeed  

    #返回离自身“最近”的箱子坐标
    def Searchchest(self,api:IStudentAPI)->tuple:
        chestlist=self.mapdict['Chest']
        chestdistlist=[]
        for chest in chestlist:
            chestdistlist.append(self.chestdis(chest,api))
        return chestlist[chestdistlist.index(min(chestdistlist))]     

    #返回离自身最近的某地形的坐标
    def Search(self, targetType, api:IAPI) -> tuple:
        """
        target是PlaceType字符串，比如"Grass"、"Wall"
        返回最近的该地形的坐标
        """
        placeList = self.mapdict[targetType]
        placeDistList = []
        for place in placeList:
            placeDistList.append(self.Distance(place, api))
        return placeList[placeDistList.index(min(placeDistList))]


    #更新其他学生位置
    def updatestuposition(self,api:IStudentAPI)->None:
        students=api.GetStudents()
        for s in students:
            if s.playerID!=api.GetSelfInfo().playerID:
                x=AssistFunction.GridToCell(s.x)
                y=AssistFunction.GridToCell(s.y)
                self.stuposition[s.playerID]=(x,y)

    #获取视野，发送信息
    def getviewrange(self,api:IStudentAPI)->None:
        #教室门情况更新
        doornum=[3,5,6]
        for i in doornum:
            x,y=self.mapdict['Door{}'.format(i)][0]
            X=AssistFunction.CellToGrid(x)
            Y=AssistFunction.CellToGrid(y)
            if api.HaveView(X,Y):#此门在视野内
                if api.IsDoorOpen(x,y)==False:#门锁上了
                    self.closeddoors[(x,y)]=True#更新已锁门的信息
                elif (x,y)in self.closeddoors.keys():#门重新打开
                    del self.closeddoors[(x,y)]#更新门信息
                break
        #捣蛋鬼位置更新
        if api.GetTrickers()!=[]:#视野里出现捣蛋鬼
            x,y=api.GetTrickers()[0].x,api.GetTrickers()[0].y#获取位置信息
            x=AssistFunction.GridToCell(x)
            y=AssistFunction.GridToCell(y)
            self.trickerPosition=[x,y]#更新自己的鬼位置信息
            self.sendmessage('t{0:0>2}{1:0>2}'.format(x,y),api)#给队友报点，传送数据格式为五位，例如“t0101"
            api.Print('鬼位置：'+'t{0:0>2}{1:0>2}'.format(x,y))
            if self.trickertype==0:
                self.trickertype=api.GetTrickers()[0].trickerType.value#更新鬼信息
                self.sendmessage('p{}'.format(self.trickertype),api)#给队友发送
  
    #给除自己以外的三个队友发送字符串
    #通信协议：h我需要救援 r我开始救援 c我中止救援 s我救人成功 e我进入溜鬼状态 o我出了溜鬼状态 l报告修机进度 g报告大门进度 t报告鬼位置 p报告鬼类型
    def sendmessage(self,mes,api:IStudentAPI)->None:
        for i in [0,1,2,3]:
            if i!=api.GetSelfInfo().playerID:#非本人
                api.SendMessage(i,mes)

    # 报修机开门进度
    def ReportProgress(self, behaveType: str ,api :IStudentAPI) -> None:
        """
        behaveType为"l"为修机，behaveType为"g"为开门
        """
        # 给除自己以外的三个队友发送字符串
        # 通信协议：h我需要救援 r我开始救援 c我中止救援 s我救人成功 e我进入溜鬼状态 o我出了溜鬼状态 l报告修机进度 g 报告开门进度
        if behaveType == "l":
            fullProgress = 10000000
        elif behaveType == "g":
            fullProgress = 18000
        else:
            api.Print("报告未知进度")
            return
        def sendmessage(mes) -> None:
            for i in range(4):
                if i != api.GetSelfInfo().playerID: # 非本人
                    api.SendMessage(i, mes)
        
        def ProgressToCode(progress: int) -> int:
            percentage = progress / fullProgress
            code = 0
            if percentage == 0:
                code = 0
            elif percentage > 0 and percentage <= 0.2:
                code = 1
            elif percentage > 0.2 and percentage <= 0.4:
                code = 2
            elif percentage > 0.4 and percentage <= 0.6:
                code = 3
            elif percentage > 0.6 and percentage <= 0.8:
                code = 4
            elif percentage > 0.8 and percentage < 1:
                code = 5
            else:
                code = 6
            return code
        if behaveType == "l":
            progress = api.GetClassroomProgress(self.classroom[0], self.classroom[1])
            code = ProgressToCode(progress)
            msg = "l" + str(code) + '{:0>2}'.format(str(self.classroom[0])) + '{:0>2}'.format(str(self.classroom[1]))
        elif behaveType == "g":
            progress = api.GetGateProgress(self.gate[0], self.gate[1])
            code = ProgressToCode(progress)
            msg = "g" + str(code) + '{:0>2}'.format(str(self.gate[0])) + '{:0>2}'.format(str(self.gate[1]))           
        sendmessage(msg)
        

    #获取信息,更新状态
    def updatemessage(self,api:IStudentAPI)->None:
        messages=[]
        while api.HaveMessage():#有信息
            messages.append(api.GetMessage())
        if len(messages)==0:
            return
        for message in messages:
            id=message[0]
            mes=message[1][0]
            if mes=='h':#需要救援
                self.needRescueList[id] = True
            elif mes=='r':#有别人处在救援状态
                self.doingrescue=True
            elif mes=='c':#有人取消了救援状态
                self.doingrescue=False
            elif mes=='s':#救人成功了
                self.doingrescue=False
                self.needRescueList[id] = False
            elif mes=='e':#进入溜鬼状态了
                self.escapelist.append(id)
            elif mes=='o':#出溜鬼状态
                if id in self.escapelist:
                    self.escapelist.remove(id)
            elif mes=='l':#报修机进度
                if len(message[1])<6:return
                p=int(message[1][1])
                x=int(message[1][2:4])
                y=int(message[1][4:6])
                api.Print("更新教室")
                api.Print(str(x))
                if 1<=p<=6:
                    self.classroomprogress[(x,y)]=(p-1)*10000000/5
            elif mes=='g':#报开门进度
                if len(message[1])<6:return
                p=int(message[1][1])
                x=int(message[1][2:4])
                y=int(message[1][4:6])
                api.Print(str(x))
                if 1<=p<=6:
                    self.gateprogress[(x,y)]=(p-1)*18000/5
            elif mes=='t':#报鬼位置
                if len(message[1])<5:return
                if api.GetTrickers()!=[]:#自己能看到鬼
                    return
                #自己没有鬼视野且队友报点，此时才更新鬼位置信息
                api.Print("更新鬼位置")
                x=int(message[1][1:3])
                y=int(message[1][3:5])
                self.trickerPosition=[x,y] 
            elif mes=='p':#报告鬼类型
                self.trickertype=message[1][1]#标注鬼的类型
                
        stuList = api.GetStudents()
        for stu in stuList:
            if stu.playerState.value == 4:#已经quit
                self.needRescueList[stu.playerID] = False
        
        if self.needRescueList == [False, False, False, False]:
            self.needrescue = False
        else:
            self.needrescue = True
        for i in [0,1,2,3]:
            if self.needRescueList[i] and i != api.GetSelfInfo().playerID:
                self.rescueid = i
                api.Print(f'{i}需要救援')
                break
        api.Print(str(self.needRescueList))                               



    # 躲草丛    
    def HideInGrass(self, api: IStudentAPI) -> None:

        target = self.Search("Grass", api)
        selfInfo = api.GetSelfInfo()
        (X, Y) = (AssistFunction.GridToCell(selfInfo.x), AssistFunction.GridToCell(selfInfo.y))
        if (X, Y) != target:
            self.path = self.a_star_search((X, Y), target, api) 
            self.MoveTo(self.path, api)

    # 溜鬼
    def EscapeFromTricker(self, api: IStudentAPI) -> None:
        """
        溜鬼并记录鬼信息，调用后自动移动
        """
        # 欧几里得距离
        def Euclid(a: tuple, b: tuple):
            (x, y) = a
            (x1, y1) = b
            return math.sqrt((x - x1) ** 2 + (y - y1) ** 2)
        
        # 临近两层可走点
        def DeeperNeighbors(coordinate: tuple) -> list:
            X = coordinate[0]
            Y = coordinate[1]
            neighbor = list()
            for i in [X - 2, X - 1, X, X + 1, X + 2]:
                for j in [Y - 2, Y - 1, Y, Y + 1, Y + 2]:
                    if(i>49 or i<0 or j>49 or j<0):continue  #边界处理
                    if(i==X and j==Y):continue
                    #如果是空地或草地
                    if (i,j)in self.mapdict['Land']or (i,j)in self.mapdict['Grass'] :
                        neighbor.append((i,j))
                    #如果是窗 需要站在前后一格内
                    elif (i,j)in self.mapdict['Window']:
                        if i==X or j==Y:
                            neighbor.append((i,j))
                    #如果是教室门
                    elif (i,j)in self.mapdict['Door3']or (i,j)in self.mapdict['Door5']or (i,j)in self.mapdict['Door6']:
                        if (i,j)not in self.closeddoors.keys():
                            neighbor.append((i,j))
            return neighbor
        
        # 坐标权值
        def Value(coordinate: tuple, trickerCoordinate: tuple, mateXYList: list) -> float:
            trickerDist =  Euclid(trickerCoordinate, coordinate)
            neighborAmount = len(DeeperNeighbors(coordinate))
            grassPoint = 0
            if self.mapInfo[coordinate[0]][coordinate[1]] == "Grass":
                grassPoint = 1
            mateDistList = []
            for mateXY in mateXYList:
                mateDistList.append(Euclid(coordinate, mateXY))
            mateDist = min(mateDistList)
            
            trickerWeight = 15  # 鬼距离权重
            neighborWeight = 3  # 可走格数权值
            grassWeight = 1     # 草权值
            mateWeight = 2      # 队友权重
            
            if self.mapInfo[trickerCoordinate[0]][trickerCoordinate[1]] == "Grass": 
                grassWeight = 0
            
            return float(trickerDist * trickerWeight + neighborAmount * neighborWeight + grassPoint * grassWeight + mateDist * mateWeight)
        
        selfInfo = api.GetSelfInfo()
        X, Y = AssistFunction.GridToCell(selfInfo.x), AssistFunction.GridToCell(selfInfo.y)
        if len(api.GetTrickers())>0:
            trickerInfo = api.GetTrickers()[0]
            trickerXY = (AssistFunction.GridToCell(trickerInfo.x), AssistFunction.GridToCell(trickerInfo.y))
            # 更新鬼信息
            if len(self.trickerPastInfo) < 2:
                self.trickerPastInfo.append(trickerXY[0])
                self.trickerPastInfo.append(trickerXY[1])
            else:
                self.trickerPastInfo[0], self.trickerPastInfo[1] = trickerXY[0], trickerXY[1]
            """
            这里还需要报点通信
            """
            mateList = api.GetStudents()
            mateXYList = []
            for mate in mateList:
                mateXYList.append((AssistFunction.GridToCell(mate.x), AssistFunction.GridToCell(mate.y)))
            
            neighborList = self.neighbors((X, Y), api)
            valueList = []
            for neighbor in neighborList:
                valueList.append(Value(neighbor, trickerXY, mateXYList))
            if len(valueList) == 0: return
            bestNeighbor = neighborList[valueList.index(max(valueList))]
            api.Print(f"我在{(X, Y)}，鬼在{trickerXY}，目标{bestNeighbor}")
            self.MoveTo([(X, Y), bestNeighbor], api)
    
    # 隐身对抗
    def EscapeInvisibleTricker(self, api: IStudentAPI) -> None:
        """
        鬼为刺客时进行调用，若有视野也可以使用
        """
        # 欧几里得距离
        def Euclid(a: tuple, b: tuple):
            (x, y) = a
            (x1, y1) = b
            return math.sqrt((x - x1) ** 2 + (y - y1) ** 2)
       
        # 临近两层可走点
        def DeeperNeighbors(coordinate: tuple) -> list:
            X = coordinate[0]
            Y = coordinate[1]
            neighbor = list()
            for i in [X - 2, X - 1, X, X + 1, X + 2]:
                for j in [Y - 2, Y - 1, Y, Y + 1, Y + 2]:
                    if(i>49 or i<0 or j>49 or j<0):continue  #边界处理
                    if(i==X and j==Y):continue
                    #如果是空地或草地
                    if (i,j)in self.mapdict['Land']or (i,j)in self.mapdict['Grass'] :
                        neighbor.append((i,j))
                    #如果是窗 需要站在前后一格内
                    elif (i,j)in self.mapdict['Window']:
                        if i==X or j==Y:
                            neighbor.append((i,j))
                    #如果是教室门
                    elif (i,j)in self.mapdict['Door3']or (i,j)in self.mapdict['Door5']or (i,j)in self.mapdict['Door6']:
                        if (i,j)not in self.closeddoors.keys():
                            neighbor.append((i,j))
            return neighbor
        
        # 坐标权值
        def Value(coordinate: tuple, trickerPastCoordinate: tuple, mateXYList: list) -> float:
            trickerDist =  Euclid(trickerPastCoordinate, coordinate)
            neighborAmount = len(DeeperNeighbors(coordinate))
            grassPoint = 0
            if self.mapInfo[coordinate[0]][coordinate[1]] == "Grass":
                grassPoint = 1
            mateDistList = []
            for mateXY in mateXYList:
                mateDistList.append(Euclid(coordinate, mateXY))
            mateDist = min(mateDistList)
            
            trickerWeight = 9   # 鬼距离权重
            neighborWeight = 3  # 可走格数权值
            grassWeight = 1     # 草权值
            mateWeight = 2      # 队友权重
            
            return float(trickerDist * trickerWeight + neighborAmount * neighborWeight + grassPoint * grassWeight + mateDist * mateWeight)
        
        margin = 500.
        
        # 测试有无鬼视野
        if len(api.GetTrickers()) > 0:
            self.EscapeFromTricker(api)
        else:
            # 走一步测试
            selfInfo = api.GetSelfInfo()
            X, Y = AssistFunction.GridToCell(selfInfo.x), AssistFunction.GridToCell(selfInfo.y)
            oldAlert = selfInfo.dangerAlert
            if oldAlert > 0:
                oldDist = self.alertnessRadius / oldAlert
            else: 
                return

            # 如果有上次鬼点信息
            if len(self.trickerPastInfo) >= 2:
                mateList = api.GetStudents()
                mateXYList = []
                for mate in mateList:
                    mateXYList.append((AssistFunction.GridToCell(mate.x), AssistFunction.GridToCell(mate.y)))
                neighborList = self.neighbors((X, Y), api)
                valueList = []
                for neighbor in neighborList:
                    valueList.append(Value(neighbor, (self.trickerPastInfo[0],self.trickerPastInfo[1]), mateXYList))
                if len(valueList) == 0: return
                bestNeighbor_X, bestNeighbor_Y = neighborList[valueList.index(max(valueList))]
                self.MoveTo([(X, Y), (bestNeighbor_X, bestNeighbor_Y)], api)
            # 无上次鬼信息
            else:
                neighborList = self.neighbors((X, Y), api)
                bestNeighbor_X, bestNeighbor_Y = random.choice(neighborList)
                self.MoveTo([(X, Y), (bestNeighbor_X, bestNeighbor_Y)], api)
            
            newSelfInfo = api.GetSelfInfo()
            newAlert = newSelfInfo.dangerAlert
            
            if newAlert > 0:
                newDist = self.alertnessRadius / newAlert
            else:
                return
            
            forward_x, forward_y = X - bestNeighbor_X , Y - bestNeighbor_Y
            forward_x, forward_y = forward_x/math.sqrt(forward_x ** 2 + forward_y ** 2), forward_y/math.sqrt(forward_x ** 2 + forward_y ** 2)
                        
            x, y = AssistFunction.CellToGrid(newSelfInfo.x), AssistFunction.CellToGrid(newSelfInfo.y)
            
            # 猜对方向
            if newDist - oldDist < margin:
                tricker_x, tricker_y = math.ceil(x + forward_x * newDist), math.ceil(y + forward_y * newDist)
            # 猜错方向
            else:
                tricker_x, tricker_y = math.ceil(x - forward_x * newDist), math.ceil(y - forward_y * newDist)
                
            if len(self.trickerPastInfo) < 2:
                self.trickerPastInfo.append(AssistFunction.GridToCell(tricker_x))
                self.trickerPastInfo.append(AssistFunction.GridToCell(tricker_y))
            else:
                self.trickerPastInfo[0], self.trickerPastInfo[1] = AssistFunction.GridToCell(tricker_x), AssistFunction.GridToCell(tricker_y)
                        
    # 到达救人地
    def Rescue(self, mateId: int, api: IStudentAPI) -> None:
        """
        MateId为被救人的Id，设置canRescue标志，为真时可以执行救人函数
        """
        # 设置救人路线
        studentInfoList = api.GetStudents()
        for mateInfo in studentInfoList:
            if mateInfo.playerID == mateId:
                break
        api.Print(f"{self.__playerID}去救{mateInfo.playerID}")
        selfInfo = api.GetSelfInfo()
        self_X, self_Y = AssistFunction.GridToCell(selfInfo.x), AssistFunction.GridToCell(selfInfo.y)
        mate_X, mate_Y = AssistFunction.GridToCell(mateInfo.x), AssistFunction.GridToCell(mateInfo.y)
        if (self_X - mate_X) in range(-1, 2) and (self_Y - mate_Y) in range(-1, 2):
            self.canRescue = True
            return
        target = random.choice(self.neighbors((mate_X, mate_Y), api))
        self.path = self.a_star_search((self_X, self_Y),target, api)
        self.MoveTo(self.path, api)
        self_X, self_Y = AssistFunction.GridToCell(api.GetSelfInfo().x), AssistFunction.GridToCell(api.GetSelfInfo().y)
     
    #判断自己是否离被救者最近
    def isnearest(self,api:IStudentAPI):

        distance=[]
        otherposition=[]
        target=(-1,-1)
        #自己坐标
        X=AssistFunction.GridToCell(api.GetSelfInfo().x)
        Y=AssistFunction.GridToCell(api.GetSelfInfo().y)
        for i in [0,1,2,3]:
            if i ==self.rescueid:#被救者位置
                target=self.stuposition[i]
            elif i not in self.escapelist and i !=api.GetSelfInfo().playerID:#不是自己且不在溜鬼且不是被救者
                otherposition.append(self.stuposition[i])
        if target==(-1,-1):
            return False
        if otherposition==[]:
            return True
        dis1=math.sqrt((X-target[0])**2+(Y-target[1])**2)
        for a in otherposition:
            distance.append(math.sqrt((a[0]-target[0])**2+(a[1]-target[1])**2))
        for a in distance:
            if a<dis1:
                return False
        return True
            

    #更新机子数量
    def updateclassroom(self,api:IStudentAPI):
        num=0
        for i in self.classroomprogress.items():
            if i[1]==10000000:
                num+=1
        self.finishnum=num  
        api.Print("机子数量:{}".format(self.finishnum))   

    # 玩家操作
    def StuPlay(self, api:IStudentAPI):
        if api.GetSelfInfo().determination<=0:#如果倒了
                self.state=1#置为修机模式
                #发送需要救我的信息
                self.sendmessage('h',api)


        #首先获取视野，发送信息
        self.getviewrange(api)


        api.Print('最新鬼位置：'+str(self.trickerPosition))
        api.Print(str(self.gateprogress))
        #获取信息，更新状态
        self.updatemessage(api)
        #更新学生位置
        self.updatestuposition(api)
        #更新已修完机子数量
        self.updateclassroom(api)
        #有限状态机
        
        # 判断是否进入毕业优先模式
        if len(self.gateprogress)>0 and self.learnstate != 3:
            gateList = self.mapdict['Gate']
            for gate in gateList:
                if gate in self.gateprogress.keys():
                    if self.gateprogress[gate] == 18000:
                        self.state = 4
                        self.topTarget = gate
                        api.Print('我要不顾一切毕业啦！')
                        break
                
        if self.state==1:#处于修机模式
            if (api.GetSelfInfo().dangerAlert>1.3 and api.GetSelfInfo().dangerAlert<50000) or api.GetTrickers()!=[]:#警戒半径内或视野内有鬼出现 且排除noisyperson影响
                api.EndAllAction()
                self.state=2 #进入溜鬼模式
                #发送在溜鬼的信息
                self.sendmessage('e',api)
                api.Print("我进入溜鬼状态了！")
                #重置操作
                self.path=[]
                self.learnstate=0 
                return
            else:

                if api.GetSelfInfo().studentType.name=='Sunshine':#如果自己是开心果 优先救人
                    if self.needrescue==True and self.doingrescue==False:#需要救人
                        api.EndAllAction()
                        self.state=3#进入救人模式
                        #发送救人信息
                        self.sendmessage('r',api)

                        self.path=[]#重置操作
                        self.learnstate=0
                        return 
                else:#自己是学霸
                    if self.needrescue==True and self.doingrescue==False:#需要救人且场上暂时无人救
                        if self.framecount==0:
                            self.framecount=api.GetFrameCount()#开始记录帧数
                        else:
                            if api.GetFrameCount()-self.framecount>=5:#过了五帧了还没人救人
                                #判断自己是否离被救者最近
                                a=self.isnearest(api)
                                if a==True:

                                    api.EndAllAction()
                                    self.state=3#进入救人模式
                                    #发送救人信息
                                    self.sendmessage('r',api)

                                    self.path=[]#重置操作
                                    self.framecount=0
                                    self.learnstate=0
                                    return
                                else:
                                    self.framecount=0
                    else:self.framecount=0


                if self.learnstate==0:#未修机
                    if api.GetGameInfo().subjectFinished>=7:
                        self.learnstate=2
                        self.path=[]
                        return
                    #翻箱子行为待写
                    '''if api.GetSelfInfo().prop==[0,0,0]:#手中未持有道具
                        chest=self.Searchchest(api)
                        classroom=self.Searchclassroom(api)
                        chestdistance=self.chestdis(chest,api)
                        classroomdistance=self.classroomdis(classroom,api)
                        if chestdistance<=classroomdistance:
                            self.learnstate=4#找箱子
                            return
                    else:
                        classroom=self.Searchclassroom(api)'''
                        

                    x=api.GetSelfInfo().x
                    y=api.GetSelfInfo().y
                    X=AssistFunction.GridToCell(x)
                    Y=AssistFunction.GridToCell(y)
                    if len(self.path)==0:
                        api.Print("开始找机子")
                        classroom=self.Searchclassroom(api)
                        api.Print("找到目标机子")                    
                        target=()
                        #找到目标周围九宫格内某一格空地且没有队友在
                        for i in [-1,0,1]:
                            for j in [-1,0,1]:
                                #边界处理：待写
                                if target!=():break
                                elif (classroom[0]+i,classroom[1]+j)in self.mapdict['Land'] and ((classroom[0]+i,classroom[1]+j)not in self.stuposition.values()):
                                    target=(classroom[0]+i,classroom[1]+j)
                                    break
                        #api.Print(str(target))
                        #获取从当前位置到目标的最优路径
                    
                        self.targetclassroom=classroom
                        api.Print("开始规划路线")
                        self.path=self.a_star_search((X,Y),target,api)
                        api.Print("规划路线结束")
                        api.Print(str(self.path))
                    else:
                        classroom=self.targetclassroom
                    if self.targetclassroom in self.classroomprogress.keys():
                        if self.classroomprogress[self.targetclassroom]==10000000:#move时也要看是否机子已修完，修完则更改目标
                            self.path=[]
                            return
                    if len(self.path)==0:return
                    #小格移动
                    if abs(X-classroom[0])>1 or abs(Y-classroom[1])>1:
                        moveresult=self.MoveTo(self.path, api)
                        self.path.pop(0)
                        if moveresult==False:#移动失败
                            self.path=[]#路径清零，重新找路
                    else:#到了目标点：开始修机
                        if api.GetSelfInfo().studentType.name=='StraightAStudent':#是学霸
                            if api.GetSelfInfo().timeUntilSkillAvailable[0]==0:#技能不在冷却
                                res=api.UseSkill(0)#使用技能
                                #result=res.result()
                                #api.Print(str(result))
                        if api.HaveView(AssistFunction.CellToGrid(classroom[0]),AssistFunction.CellToGrid(classroom[1])) and api.GetClassroomProgress(classroom[0],classroom[1])==10000000:#附近这台机子已经修完
                            self.classroomprogress[classroom]=10000000
                            self.path=[]
                            return
                        learning=api.StartLearning()
                        actuallearning=learning.result()#获得是否在修机
                        if actuallearning==True:
                            self.learnstate=1#修机状态置为1
                            self.classroom=classroom#保存正在修的电机
                            self.classroomprogress[classroom]=0#保存修机进度
                        self.path=[]
                elif self.learnstate==1:#在修机状态
                    if api.GetGameInfo().subjectFinished>=7:#修机子过程中，发现已修完七台机子
                        self.learnstate=2
                        self.path=[]
                        return
                    res=api.GetClassroomProgress(self.classroom[0],self.classroom[1])
                    if res==-1:#查询进度失败 说明不在视野内
                        #self.learnstate=0#回到找机状态
                        if api.HaveView(AssistFunction.CellToGrid(self.classroom[0]),AssistFunction.CellToGrid(self.classroom[1]))==False:
                            self.learnstate=0
                        return

                    if res==10000000:#机子修完
                        #报告修机进度
                        self.ReportProgress('l',api)
                        self.classroomprogress[self.classroom]=10000000
                        if api.GetGameInfo().subjectFinished >=7:#修完七台
                            self.learnstate=2#置为找大门状态
                            self.path=[]
                        else:
                            self.learnstate=0#置为未修机状态
                            self.path=[]
                    else:
                        #报告修机进度
                        self.ReportProgress('l',api)
                        #api.Print(str(api.GetClassroomProgress(self.classroom[0],self.classroom[1])))
                        self.classroomprogress[self.classroom]=res
                elif self.learnstate==2:#在找大门状态
                    x=api.GetSelfInfo().x
                    y=api.GetSelfInfo().y
                    X=AssistFunction.GridToCell(x)
                    Y=AssistFunction.GridToCell(y)
                    if len(self.path)==0:

                        api.Print("开始找大门")
                        gate=self.Searchgate(api)
                        api.Print("找到目标大门")
                        api.Print(str(gate))

                        target=()
                        #找到目标周围九宫格内某一格空地且没有队友在
                        for i in [-1,0,1]:
                            for j in [-1,0,1]:
                                #边界处理：待写
                                if target!=():break
                                if (gate[0]+i,gate[1]+j)in self.mapdict['Land'] and ((gate[0]+i,gate[1]+j) not in self.stuposition.values()):
                                    target=(gate[0]+i,gate[1]+j)

                        #获取从当前位置到目标的最优路径

                        self.targetgate=gate
                    
                        api.Print(str(target))
                        self.path=self.a_star_search((X,Y),target,api)
                        api.Print("路径为"+str(self.path))
                        #api.Print(str((X,Y)))
                    else:
                        gate=self.targetgate

                    #小格移动
                    if len(self.path)==0:return

                    api.Print("目标大门"+str(gate))

                    if abs(X-gate[0])>1 or abs(Y-gate[1])>1:
                        api.Print("向大门移动")
                        moveresult1=self.MoveTo(self.path, api)
                        self.path.pop(0)
                        if moveresult1==False:#移动失败
                            self.path=[]#路径清零，重新找路
                    else:#到了目标点：开始开门
                        if gate in self.gateprogress.keys() and self.gateprogress[gate]==18000:#门已开完
                            api.Print("我申请毕业")
                            api.Graduate()
                            return 
                        api.Print("我准备开门")
                        opening=api.StartOpenGate()
                        actualopening=opening.result()#获得是否在开门
                        if actualopening:
                            self.learnstate=3#修机状态置为3
                            self.gate=gate
                            self.gateprogress[gate]=0 #保存开门进度
                        self.path=[]                        
                elif self.learnstate==3:#在开大门状态
                    api.Print('大门进度'+str(api.GetGateProgress(self.gate[0],self.gate[1])))
                    if api.GetGateProgress(self.gate[0],self.gate[1])==18000:#门开完
                        self.ReportProgress('g',api)
                        self.gateprogress[self.gate]=18000
                        api.Graduate()#申请毕业

                    else:
                        self.ReportProgress('g',api)
                        self.gateprogress[self.gate]=api.GetGateProgress(self.gate[0],self.gate[1])#更新开门进度
                elif self.learnstate==4:#在找箱子状态
                    pass

        elif self.state==2:#处于溜鬼模式 
            if (api.GetSelfInfo().dangerAlert<=1.3 or api.GetSelfInfo().dangerAlert>50000) and api.GetTrickers()==[]:
                self.state=1#回到修机模式
                self.sendmessage('o',api)
                api.Print("我回到修机模式了！")
                return
            api.Print("鬼类型:"+str(self.trickertype))
            if self.trickertype==1:#如果是assasin
                self.EscapeInvisibleTricker(api)
            else:
            
                self.EscapeFromTricker(api)
        
        elif self.state==3:#处于救人模式
            if (api.GetSelfInfo().dangerAlert>1.3 and api.GetSelfInfo().dangerAlert<50000) or api.GetTrickers()!=[]:#警戒半径内或视野内有鬼出现
                api.EndAllAction()
                self.state=2 #进入溜鬼模式
                self.sendmessage('c',api)#发送取消救人的信息
                self.sendmessage('e',api)#发送开始溜鬼的信息
                #重置操作
                self.path=[]
                self.learnstate=0
                self.canRescue=0
                return
            self.Rescue(self.rescueid,api)
            if self.canRescue:#到达救人位置
                if api.GetSelfInfo().studentType.name=='Sunshine':#如果是开心果
                    if api.GetSelfInfo().timeUntilSkillAvailable[0]==0:#技能冷却为0
                        res=api.UseSkill(0)#使用唤醒人技能
                        result=res.result()
                        if result==False:
                            api.StartRouseMate(self.rescueid)
                    else:
                        api.StartRouseMate(self.rescueid)
                else:
                    api.StartRouseMate(self.rescueid)
            students=api.GetStudents()
            for student in students:
                if student.playerID==self.rescueid:
                    if student.determination>0:#救起来了
                        api.Print("救起来了")
                        self.state=1#回到修机状态
                        self.path=[]
                        self.canRescue=False
                        self.doingrescue=False
                        self.needRescueList[self.rescueid] = False
                        #发送救人成功的信息
                        self.sendmessage('s',api)

        elif self.state==4:#处于毕业优先模式
            x=api.GetSelfInfo().x
            y=api.GetSelfInfo().y
            X=AssistFunction.GridToCell(x)
            Y=AssistFunction.GridToCell(y)
            gate=self.topTarget
            api.Print("不顾一切找到大门")
            api.Print(str(gate))

            target=()
            #找到目标周围九宫格内某一格空地且没有队友在
            for i in [-1,0,1]:
                for j in [-1,0,1]:
                    #边界处理：待写
                    if target!=():break
                    if (gate[0]+i,gate[1]+j)in self.mapdict['Land'] and ((gate[0]+i,gate[1]+j) not in self.stuposition.values()):
                        target=(gate[0]+i,gate[1]+j)

            #获取从当前位置到目标的最优路径

        
            api.Print(str(target))
            self.path=self.a_star_search((X,Y),target,api)
            api.Print("路径为"+str(self.path))
            #api.Print(str((X,Y)))

            #小格移动
            if len(self.path)==0:return

            api.Print("目标大门"+str(gate))

            if abs(X-gate[0])>1 or abs(Y-gate[1])>1:
                api.Print("不顾一切向大门移动")
                moveresult1=self.MoveTo(self.path, api)
                self.path.pop(0)
                if moveresult1==False:#移动失败
                    self.path=[]#路径清零，重新找路
            else:#到了目标点：开始开门
                if gate in self.gateprogress.keys() and self.gateprogress[gate]==18000:#门已开完
                    api.Print("申请毕业")
                    api.Graduate()
                    return 

    """
    
    #自身到某机子的评估距离 计算方式为欧几里得距离
    def classroomdis(self,target,api:ITrickerAPI)->float:
        x=api.GetSelfInfo().x
        y=api.GetSelfInfo().y
        x=AssistFunction.GridToCell(x)
        y=AssistFunction.GridToCell(y)
        (x1,y1)=target
        speed=api.GetSelfInfo().speed
        if target not in self.classroomprogress.keys():#机子没修过
            return math.sqrt((x-x1)**2+(y-y1)**2)*1000+10000000*speed/120  
        
        else:#机子已经修过
            classprogress=self.classroomprogress[target]
            if classprogress==10000000:#机子修完了，急切需要打，直接返回0
                return math.inf
            else:
                speed=api.GetSelfInfo().speed
                return math.sqrt((x-x1)**2+(y-y1)**2)*1000+(10000000-classprogress)*speed/120   

    #返回离自身最近的机子坐标,以修机进度优先
    def Searchclassroom(self,api:IStudentAPI)->tuple:
        classlist=self.mapdict['ClassRoom']
        classdistlist=[]
        for classroom in classlist:
            classdistlist.append(self.classroomdis(classroom,api))
        api.Print(str(classdistlist))
        return classlist[classdistlist.index(min(classdistlist))]                         

    #返回下一个最近的未探索过的草丛
    def SearchnextGrass(self, x, y, api:IAPI) -> tuple:
        self.path=[]
        grassList = self.mapdict['Grass']
        self.AddNeighGrass(X=x,Y=y)
        next=(0,0)
        mindist=10000000
        for grass in grassList:
            if self.Distance(grass, api) < mindist and grass not in self.neighbor_grass:
                next = grass
                mindist = self.Distance(next, api)
        return next

    #鬼找人
    def Findstudents(self, api:ITrickerAPI)->None:
        # 欧几里得距离
        def Euclid(a: tuple, b: tuple):
            (x, y) = a
            (x1, y1) = b
            return math.sqrt((x - x1) ** 2 + (y - y1) ** 2)
        
        # 临近两层可走点
        def DeeperNeighbors(coordinate: tuple) -> list:
            X = coordinate[0]
            Y = coordinate[1]
            neighbor = list()
            for i in [X - 2, X - 1, X, X + 1, X + 2]:
                for j in [Y - 2, Y - 1, Y, Y + 1, Y + 2]:
                    if(i>49 or i<0 or j>49 or j<0):continue  #边界处理
                    if(i==X and j==Y):continue
                    #如果是空地或草地
                    if (i,j)in self.mapdict['Land']or (i,j)in self.mapdict['Grass'] :
                        neighbor.append((i,j))
                    #如果是窗 需要站在前后一格内
                    elif (i,j)in self.mapdict['Window']:
                        if i==X or j==Y:
                            neighbor.append((i,j))
                    #如果是教室门
                    elif (i,j)in self.mapdict['Door3']or (i,j)in self.mapdict['Door5']or (i,j)in self.mapdict['Door6']:
                        if (i,j)not in self.closeddoors.keys():
                            neighbor.append((i,j))
            return neighbor
        
        # 坐标权值
        def Value(coordinate: tuple, studentsCoordinate: tuple) -> float:
            studentsDist =  Euclid(studentsCoordinate, coordinate)
            neighborAmount = len(DeeperNeighbors(coordinate))
            
            studentWeight = -15  # 人距离权重
            neighborWeight = 3  # 可走格数权值
            
            return float(studentsDist * studentWeight + neighborAmount * neighborWeight)
        
        selfInfo = api.GetSelfInfo()
        X, Y = AssistFunction.GridToCell(selfInfo.x), AssistFunction.GridToCell(selfInfo.y)
        studentsInfo = self.SearchNextStu(api)
        if studentsInfo!=None:
            studentsXY = (AssistFunction.GridToCell(studentsInfo.x), AssistFunction.GridToCell(studentsInfo.y))

            neighborList = self.neighbors((X, Y), api)
            valueList = []
            for neighbor in neighborList:
                valueList.append(Value(neighbor, studentsXY))
            bestNeighbor = neighborList[valueList.index(max(valueList))]
            api.Print(f"我在{(X, Y)}，人在{studentsXY}，目标{bestNeighbor}")
            self.MoveTo([(X, Y), bestNeighbor], api)

    # 将与(X,Y)相连的所有Grass添加到全局变量neighbor_grass中
    def AddNeighGrass(self, X, Y)->None:
        for i in [X - 1, X, X + 1]:
            for j in [Y - 1, Y, Y + 1]:
                if(i>49 or i<0 or j>49 or j<0):continue  #边界处理
                if (i,j)in self.mapdict['Grass'] and (i,j) not in self.neighbor_grass:
                    self.neighbor_grass.append((i,j))
                    self.AddNeighGrass(X=i, Y=j)

    # 坐标转角度
    def XYtoAngle(self, X, Y)->float:
        if Y<0:
            angle=math.atan2(Y, X)+2*math.pi
        else:
            angle=math.atan2(Y, X)
        return angle

    # 返回距离自己最近的还没死的学生,否则返回None
    def SearchNextStu(self, api:IAPI):
        stuList = api.GetStudents()
        mindist=100000
        if len(stuList)!=0:
            stu = None
            for stud in stuList:
                if self.Distance((stud.x,stud.y),api)<mindist and stud.determination>0 and stud.studentType.name!='Teacher':
                    stu = stud
                    mindist = self.Distance((stud.x, stud.y), api)
            return stu
        return None

    #更新其他学生位置
    def updatestuposition(self,api:ITrickerAPI)->None:
        self.stuposition=[]
        students=api.GetStudents()
        for s in students:
            x=AssistFunction.GridToCell(s.x)
            y=AssistFunction.GridToCell(s.y)
            self.stuposition.append((x,y))
    """

    def StudentPlay(self, api: IStudentAPI) -> None:
        # 公共操作
        if self.initial:#初始化操作
            self.mapInfo=api.GetFullMap()
            self.mapdict=self.MapProcess(self.mapInfo)
            self.initial=False
            self.state=1#置为修机模式
        
        self.StuPlay(api)
        '''
        
        if self.__playerID == 0:
            self.StuPlay(api)      
            return

        elif self.__playerID == 1:
            self.StuPlay(api)
            return
        
        elif self.__playerID == 2:
            self.StuPlay(api)
            return

        elif self.__playerID == 3:
            self.StuPlay(api)
            return

        # 可以写成if self.__playerID<2之类的写法
        # 公共操作
        return
        '''

    def TrickerPlay(self, api: ITrickerAPI) -> None:
        return
    """    
    def TrickerPlay(self, api: ITrickerAPI) -> None:
        selfInfo = api.GetSelfInfo()
        api.PrintSelfInfo()


        #初始化操作
        if self.initial:
            self.mapInfo=api.GetFullMap()
            self.mapdict=self.MapProcess(self.mapInfo)
            self.trickerstate=0#置为未遇到人状态
            self.path=[]
            self.initial=False



        # 状态0：没看到人也没感受到人，去最近的机子
        if self.trickerstate==0:
            self.updatestuposition(api)
            # 状态转移逻辑
            # 若感受到人但没看到人则进入状态1
            # 若看到人且最近的人没死则进入状态2
            if api.GetSelfInfo().trickDesire>0 or api.GetSelfInfo().classVolume>0 and len(api.GetStudents())==0:
                api.EndAllAction()
                self.path=[]
                self.neighbor_grass=[]
                self.trickerstate=1
            if self.SearchNextStu(api)!=None:
                if len(api.GetStudents())!=0 and self.SearchNextStu(api).determination>0:
                    api.EndAllAction()
                    self.path=[]
                    self.trickerstate=2

            classroom=self.Searchclassroom(api)
            x=api.GetSelfInfo().x
            y=api.GetSelfInfo().y
            X=AssistFunction.GridToCell(x)
            Y=AssistFunction.GridToCell(y)
            target=()
            #找到目标周围九宫格内某一格
            for i in [-1,0,1]:
                for j in [-1,0,1]:
                    #边界处理：待写
                    if(classroom[0]+i>49 or classroom[0]+i<0 or classroom[1]+j>49 or classroom[1]+j<0):continue # 边界处理
                    if (classroom[0]+i,classroom[1]+j)in self.mapdict['Land']:
                        target=(classroom[0]+i,classroom[1]+j)
                        break
            #获取从当前位置到目标的最优路径
            if len(self.path)==0:
                self.path=self.a_star_search((X,Y),target,api)
            #小格移动
            if abs(X-target[0])>1 or abs(Y-target[1])>1:
                self.MoveTo(self.path, api)
                self.path.pop(0)
            else:
                if api.GetClassroomProgress(cellX=classroom[0]-X, cellY=classroom[1]-Y)>0:
                    api.Attack(angle=self.XYtoAngle(classroom[1]-Y, classroom[0]-X))
                    time.sleep(5)




        # 状态1：感到有人，但没看到，进入隐身模式，并进入草丛
        if self.trickerstate==1:
            #api.UseSkill(0)
            self.updatestuposition(api)
            # 状态转移逻辑
            # 若看到人且人没死就进入状态2
            # 若没感到人也没看到人就进入状态0
            if self.SearchNextStu(api)!=None:
                if len(api.GetStudents())!=0 and self.SearchNextStu(api).determination>0:# and api.GetStudents()[0].studentType.name != 'Teacher':
                    api.EndAllAction()
                    self.path=[]
                    self.trickerstate=2
            if api.GetSelfInfo().trickDesire==0 and api.GetSelfInfo().classVolume==0:
                api.EndAllAction()
                self.path=[]
                self.trickerstate=0

            selfInfo = api.GetSelfInfo()
            X, Y = AssistFunction.GridToCell(selfInfo.x), AssistFunction.GridToCell(selfInfo.y)
            # 如果自己在草丛里，就从里面出来，并将整片草丛标记
            if self.mapInfo[X][Y].name == "Grass":
                grass=self.SearchnextGrass(x = X, y = Y, api = api)
                x=api.GetSelfInfo().x
                y=api.GetSelfInfo().y
                X=AssistFunction.GridToCell(x)
                Y=AssistFunction.GridToCell(y)
                #获取从当前位置到目标的最优路径
                #api.Print(str(self.path))
                if len(self.path)==0:
                    self.path=self.a_star_search((X,Y),grass,api)
                #小格移动
                #api.Print(str(self.path))
                x=api.GetSelfInfo().x
                y=api.GetSelfInfo().y
                X=AssistFunction.GridToCell(x)
                Y=AssistFunction.GridToCell(y)
                #api.Print(f"我({X}, {Y})")
                #api.Print(str((grass[0],grass[1])))
                if X-grass[0]!=0 or Y-grass[1]!=0:
                    self.MoveTo(self.path, api)
                    self.path.pop(0)
                else:
                    self.path=[]
            # 如果自己不在草丛里，就去最近的草丛
            else:
                grass=self.Search("Grass",api)
                x=api.GetSelfInfo().x
                y=api.GetSelfInfo().y
                X=AssistFunction.GridToCell(x)
                Y=AssistFunction.GridToCell(y)
                #获取从当前位置到目标的最优路径
                #api.Print(str(self.path))
                if len(self.path)==0:
                    self.path=self.a_star_search((X,Y),grass,api)
                #小格移动
                #api.Print(str(self.path))
                x=api.GetSelfInfo().x
                y=api.GetSelfInfo().y
                X=AssistFunction.GridToCell(x)
                Y=AssistFunction.GridToCell(y)
                #api.Print(f"我在({X}, {Y})")
                if X-grass[0]!=0 or Y-grass[1]!=0:
                    self.MoveTo(self.path, api)
                    self.path.pop(0)
                else:
                    self.path=[]





        # 状态2：发现人，进入攻击状态
        if self.trickerstate==2:
            stuInfo=self.SearchNextStu(api)
            self.updatestuposition(api)
            # 状态转移逻辑
            # 若附近还有没死的人，则继续状态2
            # 若附近没有活人，但能感受到人，进入状态1
            # 若附近没有活人，且感受不到人，进入状态0
            if stuInfo==None and (api.GetSelfInfo().trickDesire>0 or api.GetSelfInfo().classVolume>0):
                api.EndAllAction()
                self.path=[]
                self.neighbor_grass=[]
                self.trickerstate=1
                
            if stuInfo==None and api.GetSelfInfo().trickDesire==0 and api.GetSelfInfo().classVolume==0:
                api.EndAllAction()
                self.trickerstate=0
                self.path=[]

            if stuInfo!=None:
                (x,y)=api.GetSelfInfo().x,api.GetSelfInfo().y
                (x1,y1)=stuInfo.x,stuInfo.y
                if not api.UseSkill(1):
                    api.Attack(angle=self.XYtoAngle(Y=y1-y,X=x1-x))
                self.Findstudents(api)
                stuInfo=self.SearchNextStu(api)
                if stuInfo !=None:
                    (x,y)=api.GetSelfInfo().x,api.GetSelfInfo().y
                    (x1,y1)=stuInfo.x,stuInfo.y
                    dist = math.sqrt((x-x1)**2+(y-y1)**2)
                    if dist<2200 and stuInfo.determination>0:
                        api.Attack(angle=self.XYtoAngle(Y=y1-y,X=x1-x))
                        #determ=self.SearchNextStu(api).determination   
                        #if determ==self.lastdeterm:
                        #    time.sleep(2)   
                        #else:
                        #    self.lastdeterm=determ
                        #    time.sleep(5)

        return
    """
