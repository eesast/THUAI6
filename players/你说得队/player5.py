from email import message
from shutil import move
from unittest import mock


import PyAPI.structures as THUAI6
from PyAPI.Interface import IStudentAPI, ITrickerAPI, IAI, IAPI
from typing import Union, Final, cast, List
from PyAPI.constants import Constants
import queue

import time
import math
import random


class Setting:
    # 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
    @staticmethod
    def asynchronous() -> bool:
        return True

    # 选手需要依次将player0到player4的职业都定义
    @staticmethod
    def studentType() -> List[THUAI6.StudentType]:
        return [THUAI6.StudentType.Athlete, THUAI6.StudentType.Teacher, THUAI6.StudentType.StraightAStudent, THUAI6.StudentType.Sunshine]

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
    initial=True#控制初始化
    mapInfo=list(list())#地图信息
    mapdict={}#地图子典
    path=[]#人物的路径
    state=1 #人物的状态  1为修机或开门 2为溜鬼 3为救人中
    learnstate=0 #人物的修机状态 0为未修机 1为在修机 2为修完机找大门 3为在开大门
    classroom=(-1,-1)#人物在修的机子的位置
    finishnum=0#场上修完机子的数量
    classroomprogress={}#场上已修过的机子信息
    gate=(-1,-1)#人物在开的大门位置
    gateprogress={}#场上已修过的大门信息
    closeddoors={}#场上关闭的门信息
    trickerstate=0 #鬼的状态 0为没遇到人 1为警戒范围遇到人 2为看到人 3守尸
    neighbor_grass=[] # 相邻草丛的列表,元素为一堆相连草丛的所有坐标元组
    lastdeterm=0# 记录上次攻击时的对方血量，用于判断打没打到人
    stuposition = {}  # 场上除自己外学生的位置
    stulastposition={}       #场上最后看到的学生位置
    notsearchedclassroom=[]  #本轮未搜过的教室
    arriveclass=False        #是否已到目标教室
    framecount=0             #记录帧数的变量
    losesight=False          #记录是否追人时丢失视野
    hidingrass=[]            #记录是否有人躲在草里挡住位置

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


    #某点的临近可走的点
    def neighbors(self,current:tuple,api:IAPI)->list:
        X=current[0]
        Y=current[1]
        neighbor=list()
        for i in [X-1,X,X+1]:
            for j in [Y-1,Y,Y+1]:
                if(i>49 or i<0 or j>49 or j<0):continue #边界处理
                if(i==X and j==Y):continue
                #如果队友在这一格
                if (i,j)in self.stuposition:continue
                #如果是空地或草地
                if (i,j)in self.mapdict['Land']or (i,j)in self.mapdict['Grass'] :
                    if i!=X and j!=Y:#此格为斜着方向的格子
                        if ((X,j) not in self.mapdict['Land'] and (X,j)not in self.mapdict['Grass'] or (X,j) in self.closeddoors.keys())\
                        and ((i,Y) not in self.mapdict['Land'] and (i,Y)not in self.mapdict['Grass'] or (i,Y) in self.closeddoors.keys()):
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


    # 从当前点到周边某点的代价
    def cost(self, current, next)->float:
        x,y=current
        x1,y1=next
        # 斜着走
        if abs(x-x1)==1 and abs(y-y1)==1:
            return 1.4
        # 翻窗
        elif (x1,y1)in self.mapdict['Window']:
            return 1.2 # 这里翻窗的数值需要重新调
        # 水平或竖直移动
        else: return 1
        

    # 评估函数，先写的欧几里得距离
    def heuristic(self,a:tuple,b:tuple):
        (x,y)=a
        (x1,y1)=b
        return math.sqrt((x-x1)**2+(y-y1)**2)
    

    # A*算法
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
        while came_from[i]!=None:
            path.append(came_from[i])
            i=came_from[i]
        path.reverse()
        return path

    
    # 移动函数  沿给定路径移动一格
    def MoveTo(self,path,api:IAPI)->bool:
        x=api.GetSelfInfo().x
        y=api.GetSelfInfo().y
        X=AssistFunction.GridToCell(x)
        Y=AssistFunction.GridToCell(y)
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
                if (X-1,Y)not in self.mapdict['Land']and (X-1,Y)not in self.mapdict['Grass'] or (X-1,Y)in self.stuposition:#上面是墙，就先往左再往上
                    api.Move(int(1000*1000/speed),1.5*math.pi)
                    time.sleep(1100/speed)
                    api.Move(int(1000*1000/speed),math.pi)
                    time.sleep(1100/speed)
                elif (X,Y-1)not in self.mapdict['Land']and (X,Y-1)not in self.mapdict['Grass'] or (X,Y-1)in self.stuposition:#左边是墙，就先往上再往左
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
                if (X-1,Y)not in self.mapdict['Land']and(X-1,Y)not in self.mapdict['Grass'] or (X-1,Y) in self.stuposition:#上面是墙，就先往右再往上
                    api.Move(int(1000*1000/speed),0.5*math.pi)
                    time.sleep(1100/speed)
                    api.Move(int(1000*1000/speed),math.pi)
                    time.sleep(1100/speed)
                elif (X,Y+1)not in self.mapdict['Land']and(X,Y+1)not in self.mapdict['Grass'] or(X,Y+1) in self.stuposition:#右边是墙，就先往上再往右
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
                if (X+1,Y)not in self.mapdict['Land'] and (X+1,Y)not in self.mapdict['Grass'] or(X+1,Y) in self.stuposition:#下面是墙，就先往左再往下
                    api.Move(int(1000*1000/speed),1.5*math.pi)
                    time.sleep(1100/speed)
                    api.Move(int(1000*1000/speed),0)
                    time.sleep(1100/speed)
                elif (X,Y-1)not in self.mapdict['Land'] and (X,Y-1)not in self.mapdict['Grass'] or(X,Y-1) in self.stuposition:#左边是墙，就先往下再往左
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
                if (X+1,Y)not in self.mapdict['Land'] and (X+1,Y)not in self.mapdict['Grass'] or(X+1,Y) in self.stuposition:#下面是墙，就先往右再往下
                    api.Move(int(1000*1000/speed),0.5*math.pi)
                    time.sleep(1100/speed)
                    api.Move(int(1000*1000/speed),0)
                    time.sleep(1100/speed)
                elif (X,Y+1)not in self.mapdict['Land'] and (X,Y+1)not in self.mapdict['Grass'] or(X,Y+1)in self.stuposition:#右边是墙，就先往下再往右
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





    #自身到某机子的评估距离 计算方式为欧几里得距离+（10000000-修机进度）乘移动速度除以10倍修机速度
    def classroomdis(self,target,api:ITrickerAPI)->float:
       x=api.GetSelfInfo().x
       y=api.GetSelfInfo().y
       x=AssistFunction.GridToCell(x)
       y=AssistFunction.GridToCell(y)
       (x1,y1)=target
       speed=api.GetSelfInfo().speed
       if api.GetClassroomProgress(x1,y1)==-1:#机子没见过
           return math.sqrt((x-x1)**2+(y-y1)**2)*1000+10000000*speed/1200000  
       
       else:#机子已经见过
           classprogress=api.GetClassroomProgress(x1,y1)
           if classprogress==10000000:#机子修完了，不要去找了
               return math.inf
           else:
               return math.sqrt((x-x1)**2+(y-y1)**2)*1000+(10000000-classprogress)*speed/1200000   





    #返回离自身最近的进度非零的机子坐标,以修机进度优先
    def Searchclassroom(self,api:ITrickerAPI)->tuple:
        classlist=self.notsearchedclassroom
        classdistlist=[]
        for classroom in classlist:
            #if api.GetClassroomProgress(classroom[0],classroom[1])>10 and api.GetClassroomProgress(classroom[0],classroom[1])!=10000000:
            classdistlist.append(self.classroomdis(classroom,api))
        if len(classdistlist)>0:
            api.Print(str(classdistlist))
            return classlist[classdistlist.index(min(classdistlist))]      
        return None
    
     #返回离自身最近的大门坐标
    def Searchgate(self,api:ITrickerAPI)->tuple:
        classlist=self.notsearchedclassroom
        classdistlist=[]
        for classroom in classlist:
            #if api.GetClassroomProgress(classroom[0],classroom[1])>10 and api.GetClassroomProgress(classroom[0],classroom[1])!=10000000:
            classdistlist.append(self.Distance(classroom,api))
        if len(classdistlist)>0:
            api.Print(str(classdistlist))
            return classlist[classdistlist.index(min(classdistlist))]      
        return None






    #返回离自身最近的草的坐标
    def Search(self, targetType, api:IAPI) -> tuple:
        """
        target是PlaceType字符串，比如"Grass"、"Wall"
        返回最近的该地形的坐标
        """
        placeList = self.mapdict[targetType]
        for place in  placeList:
            if place in self.stuposition:
                placeList.remove(place)
        placeDistList = []
        for place in placeList:
            placeDistList.append(self.Distance(place, api))
        return placeList[placeDistList.index(min(placeDistList))]





    #返回下一个最近的未探索过的草丛
    def SearchnextGrass(self, x, y, api:IAPI) -> tuple:
        self.path=[]
        grassList = self.mapdict['Grass']
        self.AddNeighGrass(X=x,Y=y)
        next=(0,0)
        mindist=10000000
        for grass in grassList:
            if self.Distance(grass, api) < mindist and grass not in self.neighbor_grass and grass not in self.stuposition:
                next = grass
                mindist = self.Distance(next, api)
        return next





    #获取视野，发送信息
    def getviewrange(self,api:IAPI)->None:
        pass





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
            neighborWeight = 0  # 可走格数权值
            
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
                if self.Distance((stud.x,stud.y),api)+stud.determination/500<mindist and stud.determination>0 and stud.studentType.name!='Teacher':
                    stu = stud
                    mindist = self.Distance((stud.x, stud.y), api)+stud.determination/500
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
            for (i,j)in [(-400,-400),(-400,0),(-400,400),(0,-400),(0,400),(400,-400),(400,0),(400,400)]:#考虑到坐标不在中心，加入其他的格子
                if (AssistFunction.GridToCell(s.x+i),AssistFunction.GridToCell(s.y+j))not in self.stuposition:
                    self.stuposition.append((AssistFunction.GridToCell(s.x+i),AssistFunction.GridToCell(s.y+j)))
        for i in self.hidingrass:
            self.stuposition.append(i)
        api.Print("现有学生位置"+str(self.stuposition))




    #更新本轮未搜过的教室列表
    def updatesearchedclass(self,api:ITrickerAPI)->None:
        self.notsearchedclassroom=[]
        for classroom in self.mapdict['ClassRoom']:
            if api.GetChestProgress(classroom[0],classroom[1])!=10000000:#已修完的不去找
                self.notsearchedclassroom.append(classroom)

    #更新本轮未搜过的大门列表
    def updatesearchedgate(self,api:ITrickerAPI)->None:
        self.notsearchedclassroom=[]
        for gate in self.mapdict['Gate']:
            self.notsearchedclassroom.append(gate)                
    


    def StudentPlay(self, api: IStudentAPI) -> None:
        # 公共操作
        if self.initial:#初始化操作
            self.mapInfo=api.GetFullMap()
            self.mapdict=self.MapProcess(self.mapInfo)
            self.initial=False
            self.state=1#置为修机模式
        if self.__playerID == 0:
            # 玩家0执行操作
            if self.state==1:#处于修机模式
                if api.GetSelfInfo().dangerAlert>0:#警戒半径内有鬼出现
                    api.EndAllAction()
                    self.state=2 #进入溜鬼模式
                    #重置操作
                    self.path=[]
                    self.learnstate=0 
                
                else:
                    if self.learnstate==0:#未修机
                        if self.finishnum==7:
                            self.learnstate=3
                            return
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
                                if (classroom[0]+i,classroom[1]+j)in self.mapdict['Land']:
                                    target=(classroom[0]+i,classroom[1]+j)
                                    break
                        #获取从当前位置到目标的最优路径
                        if len(self.path)==0:
                            self.path=self.a_star_search((X,Y),target,api)
                        #小格移动
                        if abs(X-classroom[0])>1 or abs(Y-classroom[1])>1:
                            self.MoveTo(self.path, api)
                            self.path.pop(0)
                        else:#到了目标点：开始修机
                            learning=api.StartLearning()
                            actuallearning=learning.result()#获得是否在修机
                            if actuallearning:self.learnstate=1#修机状态置为1
                            self.classroom=classroom#保存正在修的电机
                            self.classroomprogress[classroom]=0#保存修机进度
                            self.path=[]
                    elif self.learnstate==1:#在修机状态
                        if api.GetClassroomProgress(self.classroom[0],self.classroom[1])==10000000:#机子修完
                            self.classroomprogress[self.classroom]=10000000
                            self.finishnum+=1
                            if self.finishnum==7:#修完七台
                                self.learnstate=2#置为找大门状态
                            else:
                                self.learnstate=0#置为未修机状态
                        else:
                            self.classroomprogress[self.classroom]=api.GetClassroomProgress(self.classroom[0],self.classroom[1])
                    elif self.learnstate==2:#在找大门状态
                        gate=self.Search("Gate",api)
                        x=api.GetSelfInfo().x
                        y=api.GetSelfInfo().y
                        X=AssistFunction.GridToCell(x)
                        Y=AssistFunction.GridToCell(y)
                        target=()
                        #找到目标周围九宫格内某一格
                        for i in [-1,0,1]:
                            for j in [-1,0,1]:
                                #边界处理：待写
                                if (gate[0]+i,gate[1]+j)in self.mapdict['Land']:
                                    target=(gate[0]+i,gate[1]+j)
                                    break
                        #获取从当前位置到目标的最优路径
                        if len(self.path)==0:
                            self.path=self.a_star_search((X,Y),target,api)
                        #小格移动
                        if abs(X-gate[0])>1 or abs(Y-gate[1])>1:
                            self.MoveTo(self.path, api)
                            self.path.pop(0)
                        else:#到了目标点：开始开门
                            opening=api.StartOpenGate()
                            actualopening=opening.result()#获得是否在开门
                            if actualopening:self.learnstate=3#修机状态置为3
                            self.gate=gate
                            self.gateprogress[gate]=0#保存开门进度
                            self.path=[]                        
                    elif self.learnstate==3:#在开大门状态
                        if api.GetGateProgress(self.gate[0],self.gate[1])==18000:#门开完
                            self.gateprogress[self.gate]=18000
                            api.Graduate()#申请毕业

                        else:
                            self.gateprogress[self.gate]=api.GetGateProgress(self.gate[0],self.gate[1])#更新开门进度
                    


            return
        elif self.__playerID == 1:
            # 玩家1执行操作
            selfInfo=api.GetSelfInfo()
            mapInfo=api.GetFullMap()
            api.PrintSelfInfo()
            api.Print("x=10,y=10,placetype:{}".format(mapInfo[10][10].name))
            api.SendMessage(2,"x={},y={}".format(selfInfo.x,selfInfo.y))

            return
        elif self.__playerID == 2:
            # 玩家2执行操作
            if api.HaveMessage():
                mes=api.GetMessage()
                api.Print("get message from {},message={}".format(mes[0],mes[1]))

            
            return
        elif self.__playerID == 3:
            # 玩家3执行操作
            sinfo=api.GetStudents()
            api.PrintStudent()

            return
        # 可以写成if self.__playerID<2之类的写法
        # 公共操作
        return

    def TrickerPlay(self, api: ITrickerAPI) -> None:
        selfInfo = api.GetSelfInfo()
        api.PrintSelfInfo()
        api.Print(str(selfInfo.trickDesire))


        #初始化操作
        if self.initial:
            self.mapInfo=api.GetFullMap()
            self.mapdict=self.MapProcess(self.mapInfo)
            self.trickerstate=0#置为未遇到人状态
            self.path=[]
            self.initial=False
            self.updatesearchedclass(api)



        # 状态0：没看到人也没感受到人，去最近的机子
        if self.trickerstate==0:
            api.Print("我在状态0")


            if api.GetGameInfo().subjectFinished>=7:#机子修完
                if len(self.notsearchedclassroom)==0:#全搜完一遍了
                    self.updatesearchedgate(api)#更新未找完的大门
                else:#初始化
                    for i in self.notsearchedclassroom:
                        if i not in self.mapdict['Gate']:
                            self.updatesearchedgate(api)
                            break
                self.updatestuposition(api)
                api.Print("寻找大门")                
                classroom=self.Searchgate(api)#找大门
            else:
                if len(self.notsearchedclassroom)==0:#全搜完一遍了
                    self.updatesearchedclass(api)
                self.updatestuposition(api)
                api.Print("寻找机子")
                classroom=self.Searchclassroom(api)
            
            # 状态转移逻辑
            # 若已经到了最近的机子，感受到人但没看到人则进入状态1
            # 若看到人且最近的人没死则进入状态2
            if self.arriveclass and api.GetSelfInfo().trickDesire>1.3 and len(api.GetStudents())==0: #and classroom==None:
                api.EndAllAction()
                self.path=[]
                self.neighbor_grass=[]
                self.trickerstate=1
                #self.updatesearchedclass(api)
                self.arriveclass=False
                return
            if self.SearchNextStu(api)!=None:
                if len(api.GetStudents())!=0 and self.SearchNextStu(api).determination>0:
                    api.EndAllAction()
                    self.path=[]
                    self.trickerstate=2
                    #self.updatesearchedclass(api)
                    self.arriveclass=False
                    return

            x=api.GetSelfInfo().x
            y=api.GetSelfInfo().y
            X=AssistFunction.GridToCell(x)
            Y=AssistFunction.GridToCell(y)
            target=(25, 25)
            if classroom!=None:

                #找到目标周围九宫格内某一格
                for i in [-1,0,1]:
                    for j in [-1,0,1]:
                        #边界处理：待写
                        if(classroom[0]+i>49 or classroom[0]+i<0 or classroom[1]+j>49 or classroom[1]+j<0):continue # 边界处理
                        if (classroom[0]+i,classroom[1]+j)in self.mapdict['Land']and (classroom[0]+i,classroom[1]+j)not in self.stuposition :
                            target=(classroom[0]+i,classroom[1]+j)
                            break
                #获取从当前位置到目标的最优路径
                if len(self.path)==0:
                    self.path=self.a_star_search((X,Y),target,api)
                    self.arriveclass=False
                #小格移动
                if abs(X-target[0])!=0 or abs(Y-target[1])!=0:
                    moveresult=self.MoveTo(self.path, api)
                    self.path.pop(0)
                    if moveresult==False:
                        self.path=[]
                else:
                    #搜过这个教室，将其从列表中剔除
                    api.Print(str(self.notsearchedclassroom))
                    api.Print(str(classroom))
                    if classroom in self.notsearchedclassroom:
                        self.notsearchedclassroom.remove(classroom)
                    self.path=[]

                    self.arriveclass=True

                    #if api.GetClassroomProgress(cellX=classroom[0], cellY=classroom[1])>0:
                        #api.Attack(angle=self.XYtoAngle(Y=classroom[1]-Y, X=classroom[0]-X))
            else:
                self.path=self.a_star_search((X,Y),target,api)
                x=api.GetSelfInfo().x
                y=api.GetSelfInfo().y
                X=AssistFunction.GridToCell(x)
                Y=AssistFunction.GridToCell(y)
                if X-target[0]!=0 and Y-target[1]!=0:
                    self.MoveTo(self.path, api)
                    self.path.pop(0)





        # 状态1：感到有人，但没看到，进入隐身模式，并进入草丛
        if self.trickerstate==1:
            api.Print("我在状态1")
            if api.GetSelfInfo().timeUntilSkillAvailable[0]==0:
                api.UseSkill(0)
            self.updatestuposition(api)
            # 状态转移逻辑
            # 若看到人且人没死就进入状态2
            # 若没感到人也没看到人就进入状态0
            if self.SearchNextStu(api)!=None:
                if self.SearchNextStu(api)!=None and self.SearchNextStu(api).determination>0:# and api.GetStudents()[0].studentType.name != 'Teacher':
                    api.EndAllAction()
                    self.path=[]
                    self.trickerstate=2
                    self.framecount=0
                    return
            if api.GetSelfInfo().trickDesire<1.3:# or self.Searchclassroom(api)!=None:
                api.EndAllAction()
                self.path=[]
                self.trickerstate=0
                self.framecount=0
                return
            
            if self.losesight==True:#追人时丢失视野进入了1状态，则很有可能是翻窗或走门或进草丛了：
                if len(self.stulastposition)<2:
                    self.losesight=False
                    self.path=[]
                    return
                api.Print("丢失视野")
                x=api.GetSelfInfo().x
                y=api.GetSelfInfo().y
                X=AssistFunction.GridToCell(x)
                Y=AssistFunction.GridToCell(y) 
                if len(self.path)==0:
                    door3=self.mapdict['Door3']
                    door5=self.mapdict['Door5']
                    door6=self.mapdict['Door6']
                    doorlist=door3+door5+door6
                    windowlist=self.mapdict['Window']
                    grasslist=self.mapdict['Grass']
                    list=doorlist+windowlist+grasslist
                    for i in list:
                        if i in self.stuposition:
                            list.remove(i)
                    dislist=[]#距离列表
                    for i in list:
                        dislist.append(self.heuristic(i,(self.stulastposition[0],self.stulastposition[1])))#学生最后位置 到某地形的距离
                    target=list[dislist.index(min(dislist))]#找到目标位置
                    if target in self.mapdict['Grass']:
                        self.path=self.a_star_search((X,Y),target,api)
                        self.losesight=False
                        return

                    if target in self.mapdict['Window']:#如果是窗，找到其前后某一格
                        for (i,j) in [(0,-1),(-1,0),(1,-0),(0,1)]:
                            if (target[0]+i,target[1]+j)in self.mapdict['Land']and (target[0]+i)!=self.stulastposition[0]and(target[1]+j)!=self.stulastposition[1]:
                                target=(target[0]+i,target[1]+j)
                                break
                    api.Print("“目标是"+str(target))
                    self.path=self.a_star_search((X,Y),target,api)

                    api.Print(str(self.path))
               
                if len(self.path)>1:                
                    if X-self.path[-1][0]!=0 or Y-self.path[-1][1]!=0:#没到目的地
                        api.Print(str(self.path))
                        self.MoveTo(self.path,api)
                        self.path.pop(0)
                        return
                    else:#到了目的地
                        api.Print("到了目的地")
                        
                        self.path=[]
                        self.losesight=False
                if len(self.path)==1:
                    api.Print("到了目的地1")
                    self.path=[]
                    self.losesight=False



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
                    #x=api.GetSelfInfo().x
                    #y=api.GetSelfInfo().y
                    #X=AssistFunction.GridToCell(x)
                    #Y=AssistFunction.GridToCell(y)
                    #if (X,Y) == self.path[0]:
                    #    for i in range(3):
                    #        if not api.UseSkill(1):
                    #            api.Attack(angle=self.XYtoAngle(Y=self.path[0][1]-y,X=self.path[0][0]-x))
                    #        else:
                    #            api.Attack(angle=self.XYtoAngle(Y=self.path[0][1]-y,X=self.path[0][0]-x))
                    self.path.pop(0)
                else:
                    self.path=[]
            # 如果自己不在草丛里，就去最近的草丛
            else:
                if len(self.path)!=0:
                    grass=self.path[-1]
                else:
                    grass=self.Search("Grass",api)
                x=api.GetSelfInfo().x
                y=api.GetSelfInfo().y
                X=AssistFunction.GridToCell(x)
                Y=AssistFunction.GridToCell(y)
                #获取从当前位置到目标的最优路径
                #api.Print(str(self.path))
                if len(self.path)==0 and grass not in self.stuposition:
                    self.path=self.a_star_search((X,Y),grass,api)
                #小格移动
                #api.Print(str(self.path))
                x=api.GetSelfInfo().x
                y=api.GetSelfInfo().y
                X=AssistFunction.GridToCell(x)
                Y=AssistFunction.GridToCell(y)
                #api.Print(f"我在({X}, {Y})")
                if X-grass[0]!=0 or Y-grass[1]!=0:
                    if abs(X-grass[0])<=1 and abs(Y-grass[1])<=1:
                        if self.framecount>=60:#过了60帧还在这没动

                            api.Attack(angle=self.XYtoAngle(Y=grass[1]-Y,X=grass[0]-X))
                            self.framecount+=1
                            if self.framecount>150:#过了150帧了
                                self.hidingrass.append((grass))
                                self.trickerstate=0#回到找机子状态
                                self.framecount=0
                                self.path=[]
                                return
                        else:
                            self.framecount+=1
                    if len(self.path)!=0:
                        self.MoveTo(self.path, api)
                    #x=api.GetSelfInfo().x
                    #y=api.GetSelfInfo().y
                    #X=AssistFunction.GridToCell(x)
                    #Y=AssistFunction.GridToCell(y)
                    #if (X,Y) == self.path[0]:
                    #    for i in range(3):
                    #        if not api.UseSkill(1):
                    #            api.Attack(angle=self.XYtoAngle(Y=self.path[0][1]-y,X=self.path[0][0]-x))
                    #        else:
                    #            api.Attack(angle=self.XYtoAngle(Y=self.path[0][1]-y,X=self.path[0][0]-x))
                        self.path.pop(0)
                else:
                    self.path=[]





        # 状态2：发现人，进入攻击状态
        if self.trickerstate==2:
            api.Print("我在状态2")
            if api.GetSelfInfo().timeUntilSkillAvailable[0]==0:
                api.UseSkill(0)            
            stuInfo=self.SearchNextStu(api)

            # 状态转移逻辑
            # 若附近还有没死的人，则继续状态2
            # 若附近没有活人，但能感受到人，进入状态1
            # 若附近没有活人，且感受不到人，进入状态0

            #如果打死老师，守尸 
        # stulist=api.GetStudents()
        # if len(stulist)>0:
        #     for stu in stulist:
        #         if len(stulist)==1 and stu.determination<=0 and stu.playerState.value!=4 and stu.studentType.name=='Teacher':#附近只有一个老师，倒地但没死
        #             return #守尸
            if stuInfo==None and len(api.GetStudents())!=0:#如果视野内有人，但要么倒地了要么是教师，直接进入状态0
     
                api.Print("人已倒地")
                api.EndAllAction()
                self.trickerstate=0
                self.path=[]
                return                

            if stuInfo==None and api.GetSelfInfo().trickDesire>1.3: #and self.Searchclassroom(api)==None:
                api.EndAllAction()
                self.path=[]
                self.neighbor_grass=[]
                self.trickerstate=1
                if len(api.GetStudents())==0:#抓人状态突然无视野
                    self.losesight=True
                return
                
            if stuInfo==None:
                api.Print('没人了')
                api.EndAllAction()
                self.trickerstate=0
                self.path=[]
                return

            self.updatestuposition(api)
            if api.GetSelfInfo().timeUntilSkillAvailable==0:
                api.UseSkill(0)
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
                    self.stulastposition=[AssistFunction.GridToCell(stuInfo.x),AssistFunction.GridToCell(stuInfo.y)]

        return