import PyAPI.structures as THUAI6
from PyAPI.Interface import IStudentAPI, ITrickerAPI, IAI
from typing import Union, Final, cast, List
from PyAPI.constants import *
import queue
import math
import time
import copy

class Setting:
    # 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
    @staticmethod
    def asynchronous() -> bool:
        return True

    # 选手需要依次将player0到player4的职业都定义
    @staticmethod
    def studentType() -> List[THUAI6.StudentType]:
        return [THUAI6.StudentType.Athlete, THUAI6.StudentType.StraightAStudent, THUAI6.StudentType.Teacher, THUAI6.StudentType.Athlete]

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

def with_student_nearby(x:int,y:int,api:IStudentAPI):
    student_info_list=api.GetStudents()
    student_place_list=[]
    for students in student_info_list:
        student_place_list.append((students.x,students.y))
    xlist=[x,max(x-1,0),min(x+1,49),max(x-2,0),min(x+2,49),max(x-3,0),min(x+3,49)]
    ylist=[y,max(y-1,0),min(y+1,49),max(y-2,0),min(y+2,49),max(y-3,0),min(y+3,49)]
    for xx in xlist:
        for yy in ylist:
            if (xx,yy) in student_place_list:
                return 1
    return 0

def get_student(x:int,y:int,api:IStudentAPI):
    student_info_list=api.GetStudents()
    student_place_list=[]
    for students in student_info_list:
        student_place_list.append((students.x,students.y))
    for (xx,yy) in student_place_list:
        if((x,y) == (xx,yy)):
            return 1
    return 0

def found_gate(x:int,y:int,api:IStudentAPI):#查找九宫格内有没有校门
    xlist=[x,max(x-1,0),min(x+1,49)]
    ylist=[y,max(y-1,0),min(y+1,49)]
    for xx in xlist:
        for yy in ylist:
            if api.GetPlaceType(xx,yy).value==5:
                return 1,xx,yy
    return 0,0,0
def found_work(x:int,y:int,api:IStudentAPI):#查找九宫格内有没有校门
    xlist=[x,max(x-1,0),min(x+1,49)]
    ylist=[y,max(y-1,0),min(y+1,49)]
    print("xlist:",xlist)
    print("ylist:",ylist)
    for xx in xlist:
        for yy in ylist:
            if api.GetPlaceType(xx,yy).value==4:
                return 1,xx,yy
    return 0,0,0
def found_chest(x:int,y:int,api:IStudentAPI):
    # 查找九宫格内有没有箱子
    xlist=[x,max(x-1,0),min(x+1,49)]
    ylist=[y,max(y-1,0),min(y+1,49)]
    print("xlist:",xlist)
    print("ylist:",ylist)
    for xx in xlist:
        for yy in ylist:
            if api.GetPlaceType(xx,yy).value==11:
                return 1,xx,yy
    return 0,0,0
def search_classroom_direction(x:int,y:int,api:IStudentAPI):
    q=queue.Queue(maxsize=-1)
    q.put((x,y,[0]))
    visited=[]
    while(q.empty()==False):
        temp=q.get()
        #print("getnode:",temp[0],temp[1])
        if api.GetPlaceType(temp[0],temp[1]).value==4 and api.GetClassroomProgress(temp[0],temp[1])<10000000 :#and (not with_student_nearby(temp[0],temp[1])):
            print("found!",temp[2][1])
            return temp[2][1]
        if temp[0]>0 and api.GetPlaceType(temp[0]-1,temp[1]).value!=2 and api.GetPlaceType(temp[0]-1,temp[1]).value!=5 and api.GetPlaceType(temp[0]-1,temp[1]).value!=6 and api.GetPlaceType(temp[0]-1,temp[1]).value!=11 and get_student(temp[0]-1,temp[1],api)==0 and ((temp[0]-1,temp[1]) not in visited):
            visited.append((temp[0]-1,temp[1]))
            temp2=copy.copy(temp[2])
            temp2.append(1)
            #print("add1",temp[0]-1,temp[1])
            q.put((temp[0]-1,temp[1],temp2))
        if temp[0]<49 and api.GetPlaceType(temp[0]+1,temp[1]).value!=2 and api.GetPlaceType(temp[0]+1,temp[1]).value!=5 and api.GetPlaceType(temp[0]+1,temp[1]).value!=6 and api.GetPlaceType(temp[0]+1,temp[1]).value!=11 and get_student(temp[0]+1,temp[1],api)==0 and ((temp[0]+1,temp[1]) not in visited):
            visited.append((temp[0]+1,temp[1]))
            temp2=copy.copy(temp[2])
            temp2.append(2)
            #print("add2",temp[0]+1,temp[1])
            q.put((temp[0]+1,temp[1],temp2))
        if temp[1]>0 and api.GetPlaceType(temp[0],temp[1]-1).value!=2 and api.GetPlaceType(temp[0],temp[1]-1).value!=5 and api.GetPlaceType(temp[0],temp[1]-1).value!=6 and api.GetPlaceType(temp[0],temp[1]-1).value!=11 and get_student(temp[0],temp[1]-1,api)==0 and ((temp[0],temp[1]-1) not in visited):
            visited.append((temp[0],temp[1]-1))
            temp2=copy.copy(temp[2])
            temp2.append(3)
            #print("add3",temp[0],temp[1]-1)
            q.put((temp[0],temp[1]-1,temp2))
        if temp[1]<49 and api.GetPlaceType(temp[0],temp[1]+1).value!=2 and api.GetPlaceType(temp[0],temp[1]+1).value!=5 and api.GetPlaceType(temp[0],temp[1]+1).value!=6 and api.GetPlaceType(temp[0],temp[1]+1).value!=11 and get_student(temp[0],temp[1]+1,api)==0 and ((temp[0],temp[1]+1) not in visited):
            visited.append((temp[0],temp[1]+1))
            temp2=copy.copy(temp[2])
            temp2.append(4)
            #print("add4",temp[0],temp[1]+1)
            q.put((temp[0],temp[1]+1,temp2))
def search_gate_direction(x:int,y:int,api:IStudentAPI):
    q=queue.Queue(maxsize=-1)
    q.put((x,y,[0]))
    visited=[]
    while(q.empty()==False):
        temp=q.get()
        #print("getnode:",temp[0],temp[1])
        if api.GetPlaceType(temp[0],temp[1]).value==5:
            print("found!",temp[2][1])
            return temp[2][1]
        if temp[0]>0 and api.GetPlaceType(temp[0]-1,temp[1]).value!=2 and api.GetPlaceType(temp[0]-1,temp[1]).value!=5 and api.GetPlaceType(temp[0]-1,temp[1]).value!=6 and api.GetPlaceType(temp[0]-1,temp[1]).value!=11 and get_student(temp[0]-1,temp[1],api)==0 and ((temp[0]-1,temp[1]) not in visited):
            visited.append((temp[0]-1,temp[1]))
            temp2=copy.copy(temp[2])
            temp2.append(1)
            #print("add1",temp[0]-1,temp[1])
            q.put((temp[0]-1,temp[1],temp2))
        if temp[0]<49 and api.GetPlaceType(temp[0]+1,temp[1]).value!=2 and api.GetPlaceType(temp[0]+1,temp[1]).value!=5 and api.GetPlaceType(temp[0]+1,temp[1]).value!=6 and api.GetPlaceType(temp[0]+1,temp[1]).value!=11 and get_student(temp[0]+1,temp[1],api)==0 and ((temp[0]+1,temp[1]) not in visited):
            visited.append((temp[0]+1,temp[1]))
            temp2=copy.copy(temp[2])
            temp2.append(2)
            #print("add2",temp[0]+1,temp[1])
            q.put((temp[0]+1,temp[1],temp2))
        if temp[1]>0 and api.GetPlaceType(temp[0],temp[1]-1).value!=2 and api.GetPlaceType(temp[0],temp[1]-1).value!=5 and api.GetPlaceType(temp[0],temp[1]-1).value!=6 and api.GetPlaceType(temp[0],temp[1]-1).value!=11 and get_student(temp[0],temp[1]-1,api)==0 and ((temp[0],temp[1]-1) not in visited):
            visited.append((temp[0],temp[1]-1))
            temp2=copy.copy(temp[2])
            temp2.append(3)
            #print("add3",temp[0],temp[1]-1)
            q.put((temp[0],temp[1]-1,temp2))
        if temp[1]<49 and api.GetPlaceType(temp[0],temp[1]+1).value!=2 and api.GetPlaceType(temp[0],temp[1]+1).value!=5 and api.GetPlaceType(temp[0],temp[1]+1).value!=6 and api.GetPlaceType(temp[0],temp[1]+1).value!=11 and get_student(temp[0],temp[1]+1,api)==0 and ((temp[0],temp[1]+1) not in visited):
            visited.append((temp[0],temp[1]+1))
            temp2=copy.copy(temp[2])
            temp2.append(4)
            #print("add4",temp[0],temp[1]+1)
            q.put((temp[0],temp[1]+1,temp2))
def search_closed_chest_direction(x:int,y:int,api:IStudentAPI):
    # 搜寻到达未打开的箱子的路径
    q=queue.Queue(maxsize=-1)
    q.put((x,y,[0]))
    visited=[]
    while(q.empty()==False):
        temp=q.get()
        if api.GetPlaceType(temp[0],temp[1]).value==11:
            if(api.GetChestProgress(temp[0],temp[1])<10000000):
                return temp[2][1]
        if temp[0]>0 and api.GetPlaceType(temp[0]-1,temp[1]).value!=2 and api.GetPlaceType(temp[0]-1,temp[1]).value!=4 and api.GetPlaceType(temp[0]-1,temp[1]).value!=5 and api.GetPlaceType(temp[0]-1,temp[1]).value!=6 and (not(api.GetPlaceType(temp[0]-1,temp[1]).value==11 and api.GetChestProgress(temp[0]-1,temp[1])==10000000)) and get_student(temp[0]-1,temp[1],api)==0 and ((temp[0]-1,temp[1]) not in visited):
            visited.append((temp[0]-1,temp[1]))
            temp2=copy.copy(temp[2])
            temp2.append(1)
            q.put((temp[0]-1,temp[1],temp2))
        if temp[0]<49 and api.GetPlaceType(temp[0]+1,temp[1]).value!=2 and api.GetPlaceType(temp[0]+1,temp[1]).value!=4 and api.GetPlaceType(temp[0]+1,temp[1]).value!=5 and api.GetPlaceType(temp[0]+1,temp[1]).value!=6 and (not(api.GetPlaceType(temp[0]+1,temp[1]).value==11 and api.GetChestProgress(temp[0]+1,temp[1])==10000000))and get_student(temp[0]+1,temp[1],api)==0 and ((temp[0]+1,temp[1]) not in visited):
            visited.append((temp[0]+1,temp[1]))
            temp2=copy.copy(temp[2])
            temp2.append(2)
            q.put((temp[0]+1,temp[1],temp2))
        if temp[1]>0 and api.GetPlaceType(temp[0],temp[1]-1).value!=2 and api.GetPlaceType(temp[0],temp[1]-1).value!=4 and api.GetPlaceType(temp[0],temp[1]-1).value!=5 and api.GetPlaceType(temp[0],temp[1]-1).value!=6 and (not(api.GetPlaceType(temp[0],temp[1]-1).value==11 and api.GetChestProgress(temp[0],temp[1]-1)==10000000))and get_student(temp[0],temp[1]-1,api)==0 and ((temp[0],temp[1]-1) not in visited):
            visited.append((temp[0],temp[1]-1))
            temp2=copy.copy(temp[2])
            temp2.append(3)
            q.put((temp[0],temp[1]-1,temp2))
        if temp[1]<49 and api.GetPlaceType(temp[0],temp[1]+1).value!=2 and api.GetPlaceType(temp[0],temp[1]+1).value!=4 and api.GetPlaceType(temp[0],temp[1]+1).value!=5 and api.GetPlaceType(temp[0],temp[1]+1).value!=6 and (not(api.GetPlaceType(temp[0],temp[1]+1).value==11 and api.GetChestProgress(temp[0],temp[1]+1)==10000000))and get_student(temp[0],temp[1]+1,api)==0 and ((temp[0],temp[1]+1) not in visited):
            visited.append((temp[0],temp[1]+1))
            temp2=copy.copy(temp[2])
            temp2.append(4)
            q.put((temp[0],temp[1]+1,temp2))
def search_tricker_direction(x:int,y:int,ta_x:int,ta_y:int,api:IStudentAPI):
    q=queue.Queue(maxsize=-1)
    q.put((x,y,[0]))
    visited=[]
    while(q.empty()==False):
        temp=q.get()
        if temp[0]==ta_x and temp[1]==ta_y:
            return temp[2][1]
        if api.GetPlaceType(temp[0]-1,temp[1])!=2 and api.GetPlaceType(temp[0]-1,temp[1])!=4 and api.GetPlaceType(temp[0]-1,temp[1])!=5 and api.GetPlaceType(temp[0]-1,temp[1])!=6 and api.GetPlaceType(temp[0]-1,temp[1])!=11 and get_student(temp[0]-1,temp[1],api)==0 and ((temp[0]-1,temp[1]) not in visited):
            visited.append((temp[0]-1,temp[1]))
            temp2=copy.copy(temp[2])
            temp2.append(1)
            q.put((temp[0]-1,temp[1],temp2))
        if api.GetPlaceType(temp[0]+1,temp[1])!=2 and api.GetPlaceType(temp[0]-1,temp[1])!=4 and api.GetPlaceType(temp[0]+1,temp[1])!=5 and api.GetPlaceType(temp[0]+1,temp[1])!=6 and api.GetPlaceType(temp[0]+1,temp[1])!=11 and get_student(temp[0]+1,temp[1],api)==0 and ((temp[0]+1,temp[1]) not in visited):
            visited.append((temp[0]+1,temp[1]))
            temp2=copy.copy(temp[2])
            temp2.append(2)
            q.put((temp[0]+1,temp[1],temp2))
        if api.GetPlaceType(temp[0],temp[1]-1)!=2 and api.GetPlaceType(temp[0]-1,temp[1])!=4 and api.GetPlaceType(temp[0],temp[1]-1)!=5 and api.GetPlaceType(temp[0],temp[1]-1)!=6 and api.GetPlaceType(temp[0],temp[1]-1)!=11 and get_student(temp[0],temp[1]-1,api)==0 and ((temp[0],temp[1]-1) not in visited):
            visited.append((temp[0],temp[1]-1))
            temp2=copy.copy(temp[2])
            temp2.append(3)
            q.put((temp[0],temp[1]-1,temp2))
        if api.GetPlaceType(temp[0],temp[1]+1)!=2 and api.GetPlaceType(temp[0]-1,temp[1])!=4 and api.GetPlaceType(temp[0],temp[1]+1)!=5 and api.GetPlaceType(temp[0],temp[1]+1)!=6 and api.GetPlaceType(temp[0],temp[1]+1)!=11 and get_student(temp[0],temp[1]+1,api)==0 and ((temp[0],temp[1]+1) not in visited):
            visited.append((temp[0],temp[1]+1))
            temp2=copy.copy(temp[2])
            temp2.append(4)
            q.put((temp[0],temp[1]+1,temp2))
#cym add
pi = 3.1415926535

class cel:
    x:int
    y:int
    gridx:int
    gridy:int
    def __init__(self,x1:int, y1:int,type = 0):#type==0采用0-50赋值
        if type ==0:
            self.x = x1
            self.y = y1
            self.gridx = AssistFunction.CellToGrid(x1)
            self.gridy = AssistFunction.CellToGrid(y1)
        else:
            self.x = AssistFunction.GridToCell(x1)
            self.y = AssistFunction.GridToCell(y1)
            self.gridx = AssistFunction.CellToGrid(self.x)
            self.gridy = AssistFunction.CellToGrid(self.y)

def gridtocell(x:int,y:int) -> cel:#返回坐标对应的cell
    return cel(AssistFunction.GridToCell(x),AssistFunction.GridToCell(y))

def distocell(x:int,y:int,dis:float) -> List[cel]:#返回以一个点为中心长度为dis的cell
    allcell=[]
    for i in range(0,2*pi,0.15):
        x1 = x+dis*math.cos(i)
        y1 = y+dis*math.sin(i)
        if x1>0 and x1<50000 and y1 >0 and y1<50000:
            the = gridtocell(x1,y1)
            if len(allcell) == 0:
                allcell.append(the)
            elif allcell[-1].x != the.x or allcell[-1].y != the.y:
                allcell.append(the)
    return allcell
    
    # Land = 1
    # Wall = 2
    # Grass = 3
    # ClassRoom = 4
    # Gate = 5
    # HiddenGate = 6
    # Window = 7
    # Door3 = 8
    # Door5 = 9
    # Door6 = 10
    # Chest = 11

def findpa(now:cel,dest:cel,map:List[List[THUAI6.PlaceType]]) -> cel:
    return cel(1,1)

def dist(x1:int,y1:int,x2:int,y2:int) ->float:
    return math.sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))

def toangle(nowx:int,nowy:int,tox:int,toy:int) ->float:
    if toy >= nowy:
        return  math.acos((tox-nowx)/dist(nowx,nowy,tox,toy))
    else: 
        return  -math.acos((tox-nowx)/dist(nowx,nowy,tox,toy))

def toangle_cel(now:cel,dest:cel) ->float:
    if dest.y >= now.y:
        return  math.acos((dest.x-now.x)/dist(now.x,now.y,dest.x,dest.y))
    else: 
        return  -math.acos((dest.x-now.x)/dist(now.x,now.y,dest.x,dest.y))

class AI(IAI):
    #cym add
    map = None
    allgates=[]
    allwindows = []
    alldoor3 = []
    alldoor5 = []
    alldoor6 = []
    allclassrooms = []
    allboxes = []
    allhiddengates = []

    # tricker

    studentposition_previous_frame:cel
    studentposition_this_frame:cel
    nowpath:List[cel]
    move_along_path:bool


    def closest_chest(self,x:int,y:int) ->cel:
        now = cel(x,y,1)
        for i in self.allboxes:
            a=1
    #cym add
    def __init__(self, pID: int):
        self.__playerID = pID
    
    def StudentPlay(self, api: IStudentAPI) -> None:
        # 公共操作
        
        
        api.EndAllAction()
        info=api.GetSelfInfo()
        x_grid,y_grid=info.x,info.y
        x_cell,y_cell=AssistFunction.GridToCell(x_grid),AssistFunction.GridToCell(y_grid)
        num_sub_finished=api.GetGameInfo().subjectFinished
        #api.PrintStudent()
        x_grid_up=x_grid-400
        x_grid_down=x_grid+400
        y_grid_left=y_grid-400
        y_grid_right=y_grid+400
        dis_right=y_grid_right-(y_grid_right//1000)*1000
        dis_left=(y_grid_right//1000)*1000-y_grid_left
        dis_down=x_grid_down-(x_grid_down//1000)*1000
        dis_up=(x_grid_down//1000)*1000-x_grid_up
        
        
        
        
        if  self.__playerID == 0:
            # 玩家0执行操作,为athlete
            
            if  api.GetTrickers()==[] or api.GetSelfInfo().timeUntilSkillAvailable[0]>5:#视野里面找不到捣蛋鬼，此时倾向于毕业
                print("notricker")
                isgate,gatex,gatey=found_gate(x_cell,y_cell,api)#检查周围有没有校门
                iswork,workx,worky=found_work(x_cell,y_cell,api)
                print("position:",x_cell,y_cell)
                print("gate:",isgate,gatex,gatey)
                print("work:",iswork,workx,worky)
                print(api.GetPlaceType(x_cell,y_cell).value)
                
                
                
                if  isgate==1:#九宫格内有校门，跑路优先级最高
                    if api.GetGateProgress(gatex,gatey)==18000:#门开了，出去
                        theta=math.atan((gatey-y_cell)/(gatex-x_cell))
                        api.Move(60,theta)
                        print("goout")
                        return
                    elif num_sub_finished>=7:#全队写完七科作业，开始开门
                        api.StartOpenGate() #开门
                        print("openinggate")
                        return
                    else:
                        pass  #无事发生
                
                
                
                
                if  num_sub_finished>=7:
                    gate_direction=search_gate_direction(x_cell,y_cell,api)
                    if gate_direction==1:
                        print("go1")
                        if(y_grid_left//1000<y_grid_right//1000):#左右不在一个格子
                            if dis_left>dis_right:
                                print("adjust to left")
                                api.MoveLeft(50)
                            else:
                                print("adjust to right")
                                api.MoveRight(50)
                        else:
                            if api.GetPlaceType(x_cell-1,y_cell).value==7:
                                api.SkipWindow()
                                return
                            else:
                                api.MoveUp(50)
                        return
                    elif gate_direction==2:
                        print("go2")
                        if(y_grid_left//1000<y_grid_right//1000):#左右不在一个格子
                            if dis_left>dis_right:
                                print("adjust to left")
                                api.MoveLeft(50)
                            else:
                                print("adjust to right")
                                api.MoveRight(50)
                        else:
                            if api.GetPlaceType(x_cell+1,y_cell).value==7:
                                api.SkipWindow()
                                return
                            else:
                                api.MoveDown(50)
                        return
                    elif gate_direction==3:
                        print("go3")
                        if(x_grid_up//1000<x_grid_down//1000):#上下不在一个格子
                            if dis_up>dis_down:
                                print("adjust to up")
                                api.MoveUp(50)
                            else:
                                print("adjust to down")
                                api.MoveDown(50)
                        else:
                            if api.GetPlaceType(x_cell,y_cell-1).value==7:
                                api.SkipWindow()
                                return
                            else:
                                api.MoveLeft(50)
                        return
                    elif gate_direction==4:
                        print("go4")
                        if(x_grid_up//1000<x_grid_down//1000):#上下不在一个格子
                            if dis_up>dis_down:
                                print("adjust to up")
                                api.MoveUp(50)
                            else:
                                print("adjust to down")
                                api.MoveDown(50)
                        else:
                            if api.GetPlaceType(x_cell,y_cell+1).value==7:
                                api.SkipWindow()
                                return
                            else:
                                api.MoveRight(50)
                        return
                
                
                
                if  iswork==1:#所在位置九宫格内有教室，开始做作业
                    print("hwnearby!")
                    if api.GetClassroomProgress(workx,worky)==10000000:#作业写完了，无事发生
                        print("hwfinished!")
                        pass
                    else:
                        api.StartLearning() #开始写
                        print("learning")
                        return
                else:
                    print("nohwnearby!")
                
                
                
                if  api.GetPlaceType(x_cell,y_cell).value==11:#所在位置是箱子，打开
                    if api.GetClassroomProgress(x_cell,y_cell)==10000000:#箱子打开了
                        api.PickProp()
                        prop_list=info.prop
                        for prop in prop_list:
                            if prop.value==4 or prop.value==5 or prop.value==6 or prop.value==7:
                                api.UseProp(prop)
                    else:
                        api.StartOpenChest()
                        print("openingchest")
                        return
                
                
                
                print("start search direction!")
                print("x_cell:",x_cell,"y_cell",y_cell)
                direction=search_classroom_direction(x_cell,y_cell,api)#向着教室前进
                print("direction:",direction)
                
                
                if direction==1:
                    print("go1")
                    if(y_grid_left//1000<y_grid_right//1000):#左右不在一个格子
                        if dis_left>dis_right:
                            print("adjust to left")
                            api.MoveLeft(50)
                        else:
                            print("adjust to right")
                            api.MoveRight(50)
                    else:
                        if api.GetPlaceType(x_cell-1,y_cell).value==7:
                            api.SkipWindow()
                            return
                        else:
                            api.MoveUp(50)
                    return
                
                
                elif direction==2:
                    print("go2")
                    if(y_grid_left//1000<y_grid_right//1000):#左右不在一个格子
                        if dis_left>dis_right:
                            print("adjust to left")
                            api.MoveLeft(50)
                        else:
                            print("adjust to right")
                            api.MoveRight(50)
                    else:
                        if api.GetPlaceType(x_cell+1,y_cell).value==7:
                            api.SkipWindow()
                            return
                        else:
                            api.MoveDown(50)
                    return
                
                
                elif direction==3:
                    print("go3")
                    if(x_grid_up//1000<x_grid_down//1000):#上下不在一个格子
                        if dis_up>dis_down:
                            print("adjust to up")
                            api.MoveUp(50)
                        else:
                            print("adjust to down")
                            api.MoveDown(50)
                    else:
                        if api.GetPlaceType(x_cell,y_cell-1).value==7:
                            api.SkipWindow()
                            return
                        else:
                            api.MoveLeft(50)
                    return
                
                
                elif direction==4:
                    print("go4")
                    if(x_grid_up//1000<x_grid_down//1000):#上下不在一个格子
                        if dis_up>dis_down:
                            print("adjust to up")
                            api.MoveUp(50)
                        else:
                            print("adjust to down")
                            api.MoveDown(50)
                    else:
                        if api.GetPlaceType(x_cell,y_cell+1).value==7:
                            api.SkipWindow()
                            return
                        else:
                            api.MoveRight(50)
                    return
            
            
            
            else:
                print("havetricker")
                tricker=api.GetTrickers()[0]
                g_x,g_y=tricker.x,tricker.y
                t_x,t_y=AssistFunction.GridToCell(g_x),AssistFunction.GridToCell(g_y)

                if abs(t_x-x_cell)+abs(t_y-y_cell)<15 and api.GetSelfInfo().timeUntilSkillAvailable[0]==0:
                    api.UseSkill(0)
                
                direction=search_tricker_direction(x_cell,y_cell,t_x,t_y,api)
                
                if direction==1:
                    if(y_grid_left//1000<y_grid_right//1000):#左右不在一个格子
                        if dis_left>dis_right:
                            print("adjust to left")
                            api.MoveLeft(50)
                        else:
                            print("adjust to right")
                            api.MoveRight(50)
                    else:
                        if api.GetPlaceType(x_cell-1,y_cell).value==7:
                            api.SkipWindow()
                            return
                        else:
                            api.MoveUp(50)
                    return
                
                
                elif direction==2:
                    if(y_grid_left//1000<y_grid_right//1000):#左右不在一个格子
                        if dis_left>dis_right:
                            print("adjust to left")
                            api.MoveLeft(50)
                        else:
                            print("adjust to right")
                            api.MoveRight(50)
                    else:
                        if api.GetPlaceType(x_cell+1,y_cell).value==7:
                            api.SkipWindow()
                            return
                        else:
                            api.MoveDown(50)
                    return
                
                
                elif direction==3:
                    if(x_grid_up//1000<x_grid_down//1000):#上下不在一个格子
                        if dis_up>dis_down:
                            print("adjust to up")
                            api.MoveUp(50)
                        else:
                            print("adjust to down")
                            api.MoveDown(50)
                    else:
                        if api.GetPlaceType(x_cell,y_cell-1).value==7:
                            api.SkipWindow()
                            return
                        else:
                            api.MoveLeft(50)
                    return
                
                
                elif direction==4:
                    if(x_grid_up//1000<x_grid_down//1000):#上下不在一个格子
                        if dis_up>dis_down:
                            print("adjust to up")
                            api.MoveUp(50)
                        else:
                            print("adjust to down")
                            api.MoveDown(50)
                    else:
                        if api.GetPlaceType(x_cell,y_cell+1).value==7:
                            api.SkipWindow()
                            return
                        else:
                            api.MoveRight(50)
                    return
            
            print("error")



        elif self.__playerID == 1:
            # 玩家1执行操作,为straightAstudent
            
            if  api.GetTrickers()==[] or api.GetSelfInfo().timeUntilSkillAvailable[0]>5:#视野里面找不到捣蛋鬼，此时倾向于毕业
                print("notricker")
                isgate,gatex,gatey=found_gate(x_cell,y_cell,api)#检查周围有没有校门
                iswork,workx,worky=found_work(x_cell,y_cell,api)
                print("position:",x_cell,y_cell)
                print("gate:",isgate,gatex,gatey)
                print("work:",iswork,workx,worky)
                print(api.GetPlaceType(x_cell,y_cell).value)
                
                if  isgate==1:#九宫格内有校门，跑路优先级最高
                    if api.GetGateProgress(gatex,gatey)==18000:#门开了，出去
                        theta=math.atan((gatey-y_cell)/(gatex-x_cell))
                        api.Move(50,theta)
                        print("goout")
                        return
                    
                    elif num_sub_finished>=7:#全队写完七科作业，开始开门
                        api.StartOpenGate() #开门
                        print("openinggate")
                        return
                    else:
                        pass  #无事发生
                if  iswork==1:#所在位置九宫格内有教室，开始做作业
                    print("hwnearby!")
                    if api.GetClassroomProgress(workx,worky)==10000000:#作业写完了，无事发生
                        print("hwfinished!")
                        pass
                    else:
                        api.UseSkill(0)
                        api.StartLearning() #开始写
                        print("learning")
                        return
                else:
                    print("nohwnearby!")
                
                if  api.GetPlaceType(x_cell,y_cell).value==11:#所在位置是箱子，打开
                    if api.GetClassroomProgress(x_cell,y_cell)==10000000:#箱子打开了
                        api.PickProp()
                        prop_list=info.prop
                        for prop in prop_list:
                            if prop.value==4 or prop.value==5 or prop.value==6 or prop.value==7:
                                api.UseProp(prop)
                    else:
                        api.StartOpenChest()
                        print("openingchest")
                        return
                
                print("start search direction!")
                direction=search_classroom_direction(x_cell,y_cell,api)#向着教室前进
                print("direction:",direction)
                
                if direction==1:
                    if(y_grid_left//1000<y_grid_right//1000):#左右不在一个格子
                        if dis_left>dis_right:
                            print("adjust to left")
                            api.MoveLeft(50)
                        else:
                            print("adjust to right")
                            api.MoveRight(50)
                    else:
                        if api.GetPlaceType(x_cell-1,y_cell).value==7:
                            api.SkipWindow()
                            return
                        else:
                            api.MoveUp(50)
                    return
                elif direction==2:
                    if(y_grid_left//1000<y_grid_right//1000):#左右不在一个格子
                        if dis_left>dis_right:
                            print("adjust to left")
                            api.MoveLeft(50)
                        else:
                            print("adjust to right")
                            api.MoveRight(50)
                    else:
                        if api.GetPlaceType(x_cell+1,y_cell).value==7:
                            api.SkipWindow()
                            return
                        else:
                            api.MoveDown(50)
                    return
                elif direction==3:
                    if(x_grid_up//1000<x_grid_down//1000):#上下不在一个格子
                        if dis_up>dis_down:
                            print("adjust to up")
                            api.MoveUp(50)
                        else:
                            print("adjust to down")
                            api.MoveDown(50)
                    else:
                        if api.GetPlaceType(x_cell,y_cell-1).value==7:
                            api.SkipWindow()
                            return
                        else:
                            api.MoveLeft(50)
                    return
                elif direction==4:
                    if(x_grid_up//1000<x_grid_down//1000):#上下不在一个格子
                        if dis_up>dis_down:
                            print("adjust to up")
                            api.MoveUp(50)
                        else:
                            print("adjust to down")
                            api.MoveDown(50)
                    else:
                        if api.GetPlaceType(x_cell,y_cell+1).value==7:
                            api.SkipWindow()
                            return
                        else:
                            api.MoveRight(50)
                    return
            else:
                pass
            return
        elif self.__playerID == 2:
            # 玩家2执行操作，为teacher
            if api.GetTrickers()==[]:
                # 视野内没有捣蛋鬼
                isChest,ChestX,ChestY=found_chest(x_cell,y_cell,api)
                if(isChest and api.GetChestProgress(ChestX,ChestY)<10000000):
                    # 身边有箱子且没开启，优先开箱子
                    api.StartOpenChest()                    
                #elif(api.GetChestProgress(ChestX,ChestY)==10000000):
                    # 身边有箱子已开启，检查掉落物
                    #api.PickProp()
                else:
                    # 身边没有箱子，主动寻找箱子
                    direction=search_closed_chest_direction(x_cell,y_cell,api)
                    if direction==1:
                        if(y_grid_left//1000<y_grid_right//1000):
                            # 左右不在一个格子
                            if dis_left>dis_right:
                                api.MoveLeft(50)
                            else:
                                api.MoveRight(50)
                        else:
                            if api.GetPlaceType(x_cell-1,y_cell).value==7:
                                api.SkipWindow()
                                return
                            else:
                                api.MoveUp(50)
                    elif direction==2:
                        if(y_grid_left//1000<y_grid_right//1000):
                            # 左右不在一个格子
                            if dis_left>dis_right:
                                api.MoveLeft(50)
                            else:
                                api.MoveRight(50)
                        else:
                            if api.GetPlaceType(x_cell+1,y_cell).value==7:
                                api.SkipWindow()
                                return
                            else:
                                api.MoveDown(50)
                    elif direction==3:
                        if(x_grid_up//1000<x_grid_down//1000):
                            # 上下不在一个格子
                            if dis_up>dis_down:
                                api.MoveUp(50)
                            else:
                                api.MoveDown(50)
                        else:
                            if api.GetPlaceType(x_cell,y_cell-1).value==7:
                                api.SkipWindow()
                                return
                            else:
                                api.MoveLeft(50)
                    elif direction==4:
                        if(x_grid_up//1000<x_grid_down//1000):
                            # 上下不在一个格子
                            if dis_up>dis_down:
                                api.MoveUp(50)
                            else:
                                api.MoveDown(50)
                        else:
                            if api.GetPlaceType(x_cell,y_cell+1).value==7:
                                api.SkipWindow()
                                return
                            else:
                                api.MoveRight(50)
            else:
                # 视野内有捣蛋鬼
                trickerInfo=api.GetTrickers()[0]
                trickerState,trickerX,trickerY = trickerInfo.playerState,trickerInfo.x,trickerInfo.y
                trickerCellX,trickerCellY = AssistFunction.GridToCell(trickerX),AssistFunction.GridToCell(trickerY)
                if abs(trickerCellX-x_cell)+abs(trickerCellY-y_cell)>9:
                    # 曼哈顿距离大于9，选择主动跟随捣蛋鬼                   
                    direction=search_tricker_direction(x_cell,y_cell,trickerCellX,trickerCellY,api)
                    if direction==1:
                        if(y_grid_left//1000<y_grid_right//1000):
                            # 左右不在一个格子
                            if dis_left>dis_right:
                                api.MoveLeft(50)
                            else:
                                api.MoveRight(50)
                        else:
                            if api.GetPlaceType(x_cell-1,y_cell).value==7:
                                api.SkipWindow()
                                return
                            else:
                                api.MoveUp(50)
                    elif direction==2:
                        if(y_grid_left//1000<y_grid_right//1000):
                            # 左右不在一个格子
                            if dis_left>dis_right:
                                api.MoveLeft(50)
                            else:
                                api.MoveRight(50)
                        else:
                            if api.GetPlaceType(x_cell+1,y_cell).value==7:
                                api.SkipWindow()
                                return
                            else:
                                api.MoveDown(50)
                    elif direction==3:
                        if(x_grid_up//1000<x_grid_down//1000):
                            # 上下不在一个格子
                            if dis_up>dis_down:
                                api.MoveUp(50)
                            else:
                                api.MoveDown(50)
                        else:
                            if api.GetPlaceType(x_cell,y_cell-1).value==7:
                                api.SkipWindow()
                                return
                            else:
                                api.MoveLeft(50)
                    elif direction==4:
                        if(x_grid_up//1000<x_grid_down//1000):
                            # 上下不在一个格子
                            if dis_up>dis_down:
                                api.MoveUp(50)
                            else:
                                api.MoveDown(50)
                        else:
                            if api.GetPlaceType(x_cell,y_cell+1).value==7:
                                api.SkipWindow()
                                return
                            else:
                                api.MoveRight(50)
                elif(abs(trickerCellX-x_cell)+abs(trickerCellY-y_cell)<4):
                    # 曼哈顿距离小于4，需要保持距离
                    direction=search_tricker_direction(x_cell,y_cell,trickerCellX,trickerCellY,api)
                    if direction==1:
                        if(y_grid_left//1000<y_grid_right//1000):
                            # 左右不在一个格子
                            if dis_left>dis_right:
                                api.MoveLeft(50)
                            else:
                                api.MoveRight(50)
                        else:
                            if api.GetPlaceType(x_cell-1,y_cell).value==7:
                                api.SkipWindow()
                                return
                            else:
                                api.MoveDown(50)
                    elif direction==2:
                        if(y_grid_left//1000<y_grid_right//1000):
                            # 左右不在一个格子
                            if dis_left>dis_right:
                                api.MoveLeft(50)
                            else:
                                api.MoveRight(50)
                        else:
                            if api.GetPlaceType(x_cell+1,y_cell).value==7:
                                api.SkipWindow()
                                return
                            else:
                                api.MoveUp(50)
                    elif direction==3:
                        if(x_grid_up//1000<x_grid_down//1000):
                            # 上下不在一个格子
                            if dis_up>dis_down:
                                api.MoveUp(50)
                            else:
                                api.MoveDown(50)
                        else:
                            if api.GetPlaceType(x_cell,y_cell-1).value==7:
                                api.SkipWindow()
                                return
                            else:
                                api.MoveRight(50)
                    elif direction==4:
                        if(x_grid_up//1000<x_grid_down//1000):
                            # 上下不在一个格子
                            if dis_up>dis_down:
                                api.MoveUp(50)
                            else:
                                api.MoveDown(50)
                        else:
                            if api.GetPlaceType(x_cell,y_cell+1).value==7:
                                api.SkipWindow()
                                return
                            else:
                                api.MoveLeft(50)
                if(trickerState.value>=11 and trickerState.value<=15 and api.GetSelfInfo().timeUntilSkillAvailable[0]==0):
                    # 若有技能则施放
                    api.UseSkill(0)
            return
        elif self.__playerID == 3:
            # 玩家3执行操作，为athlete
            
            if  api.GetTrickers()==[] or api.GetSelfInfo().timeUntilSkillAvailable[0]>5:#视野里面找不到捣蛋鬼，此时倾向于毕业
                print("notricker")
                isgate,gatex,gatey=found_gate(x_cell,y_cell,api)#检查周围有没有校门
                iswork,workx,worky=found_work(x_cell,y_cell,api)
                print("position:",x_cell,y_cell)
                print("gate:",isgate,gatex,gatey)
                print("work:",iswork,workx,worky)
                print(api.GetPlaceType(x_cell,y_cell).value)

                if  isgate==1:#九宫格内有校门，跑路优先级最高
                    if api.GetGateProgress(gatex,gatey)==18000:#门开了，出去
                        theta=math.atan((gatey-y_cell)/(gatex-x_cell))
                        api.Move(60,theta)
                        print("goout")
                        return
                    elif num_sub_finished>=7:#全队写完七科作业，开始开门
                        api.StartOpenGate() #开门
                        print("openinggate")
                        return
                    else:
                        pass  #无事发生
                
                if  iswork==1:#所在位置九宫格内有教室，开始做作业
                    print("hwnearby!")
                    if api.GetClassroomProgress(workx,worky)==10000000:#作业写完了，无事发生
                        print("hwfinished!")
                        pass
                    else:
                        api.StartLearning() #开始写
                        print("learning")
                        return
                
                else:
                    print("nohwnearby!")

                if  api.GetPlaceType(x_cell,y_cell).value==11:#所在位置是箱子，打开
                    if api.GetClassroomProgress(x_cell,y_cell)==10000000:#箱子打开了
                        api.PickProp()
                        prop_list=info.prop
                        for prop in prop_list:
                            if prop.value==4 or prop.value==5 or prop.value==6 or prop.value==7:
                                api.UseProp(prop)
                    else:
                        api.StartOpenChest()
                        print("openingchest")
                        return
                
                print("start search direction!")
                print("x_cell:",x_cell,"y_cell",y_cell)
                direction=search_classroom_direction(x_cell,y_cell,api)#向着教室前进
                print("direction:",direction)

                if direction==1:
                    if(y_grid_left//1000<y_grid_right//1000):#左右不在一个格子
                        if dis_left>dis_right:
                            print("adjust to left")
                            api.MoveLeft(50)
                        else:
                            print("adjust to right")
                            api.MoveRight(50)
                    else:
                        if api.GetPlaceType(x_cell-1,y_cell).value==7:
                            api.SkipWindow()
                            return
                        else:
                            api.MoveUp(50)
                    return
                elif direction==2:
                    if(y_grid_left//1000<y_grid_right//1000):#左右不在一个格子
                        if dis_left>dis_right:
                            print("adjust to left")
                            api.MoveLeft(50)
                        else:
                            print("adjust to right")
                            api.MoveRight(50)
                    else:
                        if api.GetPlaceType(x_cell+1,y_cell).value==7:
                            api.SkipWindow()
                            return
                        else:
                            api.MoveDown(50)
                    return
                elif direction==3:
                    if(x_grid_up//1000<x_grid_down//1000):#上下不在一个格子
                        if dis_up>dis_down:
                            print("adjust to up")
                            api.MoveUp(50)
                        else:
                            print("adjust to down")
                            api.MoveDown(50)
                    else:
                        if api.GetPlaceType(x_cell,y_cell-1).value==7:
                            api.SkipWindow()
                            return
                        else:
                            api.MoveLeft(50)
                    return
                elif direction==4:
                    if(x_grid_up//1000<x_grid_down//1000):#上下不在一个格子
                        if dis_up>dis_down:
                            print("adjust to up")
                            api.MoveUp(50)
                        else:
                            print("adjust to down")
                            api.MoveDown(50)
                    else:
                        if api.GetPlaceType(x_cell,y_cell+1).value==7:
                            api.SkipWindow()
                            return
                        else:
                            api.MoveRight(50)
                    return
            else:
                
                print("havetricker")
                tricker=api.GetTrickers()[0]
                g_x,g_y=tricker.x,tricker.y
                t_x,t_y=AssistFunction.GridToCell(g_x),AssistFunction.GridToCell(g_y)

                if abs(t_x-x_cell)+abs(t_y-y_cell)<15 and api.GetSelfInfo().timeUntilSkillAvailable[0]==0:
                    api.UseSkill(0)
                
                direction=search_tricker_direction(x_cell,y_cell,t_x,t_y,api)

                if direction==1:
                    if(y_grid_left//1000<y_grid_right//1000):#左右不在一个格子
                        if dis_left>dis_right:
                            print("adjust to left")
                            api.MoveLeft(50)
                        else:
                            print("adjust to right")
                            api.MoveRight(50)
                    else:
                        if api.GetPlaceType(x_cell-1,y_cell).value==7:
                            api.SkipWindow()
                            return
                        else:
                            api.MoveUp(50)
                    return
                elif direction==2:
                    if(y_grid_left//1000<y_grid_right//1000):#左右不在一个格子
                        if dis_left>dis_right:
                            print("adjust to left")
                            api.MoveLeft(50)
                        else:
                            print("adjust to right")
                            api.MoveRight(50)
                    else:
                        if api.GetPlaceType(x_cell+1,y_cell).value==7:
                            api.SkipWindow()
                            return
                        else:
                            api.MoveDown(50)
                    return
                elif direction==3:
                    if(x_grid_up//1000<x_grid_down//1000):#上下不在一个格子
                        if dis_up>dis_down:
                            print("adjust to up")
                            api.MoveUp(50)
                        else:
                            print("adjust to down")
                            api.MoveDown(50)
                    else:
                        if api.GetPlaceType(x_cell,y_cell-1).value==7:
                            api.SkipWindow()
                            return
                        else:
                            api.MoveLeft(50)
                    return
                elif direction==4:
                    if(x_grid_up//1000<x_grid_down//1000):#上下不在一个格子
                        if dis_up>dis_down:
                            print("adjust to up")
                            api.MoveUp(50)
                        else:
                            print("adjust to down")
                            api.MoveDown(50)
                    else:
                        if api.GetPlaceType(x_cell,y_cell+1).value==7:
                            api.SkipWindow()
                            return
                        else:
                            api.MoveRight(50)
                    return
            print("error")
            return
        # 可以写成if self.__playerID<2之类的写法
        # 公共操作
        return

    def TrickerPlay(self, api: ITrickerAPI) -> None:
        api.EndAllAction()

        if self.map is None:
            self.map = api.GetFullMap()
            yj = 0
            xi = 0
            for i in self.map:
                yj = 0
                for j in i:
                    if j.name == 'ClassRoom':
                        self.allclassrooms.append(cel(xi,yj))
                    elif j.name == 'Gate':
                        self.allgates.append(cel(xi,yj))
                    elif j.name == 'HiddenGate':
                        self.allhiddengates.append(cel(xi,yj))
                    elif j.name == 'Window':
                        self.allwindows.append(cel(xi,yj))
                    elif j.name == 'Door3':
                        self.alldoor3.append(cel(xi,yj))
                    elif j.name == 'Door5':
                        self.alldoor5.append(cel(xi,yj))
                    elif j.name == 'Door6':
                        self.alldoor6.append(cel(xi,yj))
                    elif j.name == 'Chest':
                        self.allboxes.append(cel(xi,yj))
                    yj = yj + 1
                xi = xi + 1
        

        selfInfo = api.GetSelfInfo()
        student_see = api.GetStudents()
        api.PrintStudent()
        now = cel(selfInfo.x,selfInfo.y,1)
        dest:cel
        next:cel
        _move:bool
        if len(student_see) == 0:
            dest = cel(0,0)
            if selfInfo.trickDesire == 0:
                a=1
            else:
                dis = float(Assassin.alertnessRadius)/selfInfo.trickDesire
                a=1
        else:
            closestudent=None
            cloest = 1000000000000
            knifelong = 78000#constants.
            attacklong = Constants.basicAttackShortRange
            for i in student_see:
                dis = dist(i.x,i.y,selfInfo.x,selfInfo.y)
                if dis < cloest :
                    cloest = dis
                    if i.playerState.name != 'Addicted':
                        closestudent = i
            if closestudent is not None:
                dest = cel(closestudent.x,closestudent.y,1)

            if closestudent.studentType.name == 'Athlete':
                if cloest > Athlete.viewRange:
                    if selfInfo.timeUntilSkillAvailable[0] == 0:
                        a=1
                a=1
            elif closestudent.studentType.name == 'Teacher':
                a=1
            elif closestudent.studentType.name == 'StraightAStudent':
                if cloest > StraightAStudent.viewRange:
                    if selfInfo.timeUntilSkillAvailable[0] == 0:
                        api.UseSkill(0)
                        #api.Attack()
                    else:
                        a=1#if selfInfo.playerState = 
                    
                else:
                    if cloest <= attacklong:
                        #if selfInfo.timeUntilSkillAvailable[1] == 0:
                            #api.UseSkill(1)
                        if closestudent.playerState.name != 'Addicted':
                            attckangle = toangle(now.gridx,now.gridy,dest.gridx,dest.gridy)
                            api.Attack(attckangle)
                    if selfInfo.timeUntilSkillAvailable[0] == 0:
                        api.UseSkill(0)
                    
                    else:
                        a=1#if selfInfo.playerState =
            elif closestudent.studentType.name == 'Sunshine':
                a=1
        angle = toangle_cel(now,dest)
        api.Move(60,angle)
        # hide in grass
        # attack
        # trick around
        # find chest
        # find classroom
        api.PrintSelfInfo()
        return