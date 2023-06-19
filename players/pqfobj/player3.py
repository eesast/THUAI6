import PyAPI.structures as THUAI6
from PyAPI.Interface import *
from typing import Union, Final, cast, List
from PyAPI.constants import *
import queue

import time

# add
import numpy as np
import math
import copy
from collections import deque


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
        return THUAI6.TrickerType.ANoisyPerson


# 辅助函数
numOfGridPerCell: Final[int] = 1000


class AssistFunction:

    @staticmethod
    def CellToGrid(cell: int) -> int:
        return cell * numOfGridPerCell + numOfGridPerCell // 2

    @staticmethod
    def GridToCell(grid: int) -> int:
        return grid // numOfGridPerCell

# additional function_tricker

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

class precise_cel:
    x:int
    y:int
    def __init__(self,x1:int, y1:int):
        self.x = x1
        self.y = y1
        

def gridtocell(x:int,y:int) -> cel:#返回坐标对应的cell
    return cel(AssistFunction.GridToCell(x),AssistFunction.GridToCell(y))

def distocell(x:int,y:int,dis:float,angle:float) -> List[cel]:#返回以一个点为中心长度为dis的cell
    allcell=[]
    for i in range(0,2*pi,angle):
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

def findway_DFS(grid, start:cel, end:cel) -> List[cel]:
    row, col = 50,50
    visited = set()
    path =[]
    
    class anan:
        angle:float
        to:int#0,1,2,3，下右上左
        def __init__(self,angel:float,to:int) -> None:
            self.angle = angel
            self.to = to
            pass

    def azhe(jjj:anan)->float:
        return jjj.angle

    def help(now:cel,path:List[cel])->List[cel]:
        if now == end:
            return path
        if now in visited:
            oo = []
            return oo
        visited.add(now)
        angle = toangle_cel(now,end)
        directions = [(1, 0), (0, 1), (-1, 0), (0, -1)]
        chae = [anan(abs(angle),0),anan(abs(angle-pi/2),1),anan(abs(angle-pi),2),anan(abs(angle-pi*1.5),3)]
        chae.sort(key = azhe)
        for i in chae:
            dx,dy = directions[i.to]
            new_x, new_y = now.x + dx, now.y + dy
            new =cel(new_x, new_y)
            if (grid[new_x][new_y]).value in [1,3,7,8,9,10]:
                path.append(new)
        visited.remove(now)
        return help(path[-1], path)
    
    return help(start,path = path)


# additional function_tricker

# additional function_student

pi=3.1415926535
class Cel:
    x:int
    y:int
    def __init__(self,x1:int,y1:int):
        self.x=x1
        self.y=y1

#计算距离
def distance(target:Cel,api:IStudentAPI,ai)->float:
    selfinfo=api.GetSelfInfo()
    return math.sqrt((target.x-selfinfo.x)**2+(target.y-selfinfo.y)**2)

movetime=50
#为避免没到格子中心卡墙角，用cel规划路径，用grid精确地走
def Goto(target:Cel,api:IStudentAPI,ai)->None:
        selfinfo=api.GetSelfInfo()
        #当前格子坐标
        #if self.nowpath==None:
        selfx=selfinfo.x
        selfy=selfinfo.y
        cur=gridtocell(selfx,selfy)
        nowpath=ai.BFSpath(selfinfo,cur,target,api)#规划路径
        #print('self.nowpath')
        #for k in nowpath:
        #    print(k.x,k.y)
        
        while len(nowpath)!=0:#能否这样判断不空
            first=nowpath[0]
            x1=1000*first.x+500
            y1=1000*first.y+500
            #print('curgrid',selfx,selfy)
            #print('first ',x1,y1)
            if cur.x==first.x and cur.y==first.y:
                a=nowpath.pop(0)
            else:
                #print('not the same')
                #print('first',x1,y1)
                #print('cur',cur.x,cur.y)
                if x1==selfx and y1<selfy:api.MoveLeft(movetime)
                elif x1==selfx and y1>selfy:api.MoveRight(movetime)
                elif x1<selfx and y1==selfy:api.MoveUp(movetime)
                elif x1>selfx and y1==selfy:api.MoveDown(movetime)
                elif x1<selfx and y1<selfy:api.Move(movetime,-pi*3/4)
                elif x1<selfx and y1>selfy:api.Move(movetime,pi*3/4)
                elif x1>selfx and y1<selfy:api.Move(movetime,-pi/4)
                elif x1>selfx and y1>selfy:api.Move(movetime,pi/4)
                #api.PrintSelfInfo()
                selfinfo1=api.GetSelfInfo()
                selfx1=selfinfo1.x
                selfy1=selfinfo1.y
                #print('aftermove',selfx1,selfy1)
                #cur1=gridtocell(selfinfo1.x,selfinfo1.y)
                #print('info',cur1.x,cur1.y)
                if(selfx==selfx1 and selfy==selfy1):
                    #print('not moved')
                    api.MoveRight(movetime)
                #    api.MoveRight(movetime)
                selfinfo=api.GetSelfInfo()
                #print('State ',selfinfo.playerState.name)
                #api.PrintSelfInfo()
                selfx=selfinfo.x
                selfy=selfinfo.y
                cur=gridtocell(selfx,selfy)

# additional function_student

class AI(IAI):
    map = None


    # tricker
    allgates_tr=[]
    allwindows_tr = []
    alldoor3_tr = []
    alldoor5_tr = []
    alldoor6_tr = []
    allclassrooms_tr = []
    allboxes_tr = []
    allhiddengates_tr = []

    studentposition_previous_frame=None#precise_cel
    studentposition_this_frame=None#precise_cel
    my_previous_frame=None#precise_cel
    my_this_frame=None#precise_cel

    now_path=None#List[cel]
    move_along_path=None#bool


    def closest_chest(self,x:int,y:int,api:ITrickerAPI) ->cel:
        now = cel(x,y)
        dis = 1000000000000
        the:cel
        for i in self.allboxes_tr:
            k = dist(now.x,now.y,i.x,i.y)
            jindu = api.GetChestProgress(i.x,i.y)
            if k<dis and jindu < Constants.maxChestProgress:
                dis = k
                the = i
        return the
    
    def closest_classroom(self,x:int,y:int,api:ITrickerAPI) ->cel:
        now = cel(x,y)
        dis = 1000000000000
        the:cel
        for i in self.allclassrooms_tr:
            k = dist(now.x,now.y,i.x,i.y)
            jindu = api.GetChestProgress(i.x,i.y)
            if k<dis and jindu < Constants.maxClassroomProgress:
                dis = k
                the = i
        return the

    def wall_exist_precise(self,now:precise_cel,dest:precise_cel)->bool:
        k = (dest.y-now.y)/(dest.x-now.x)
        # y = k(x-now.x)+now.y
        long  = dest.x-now.x
        for i in range(0,long,801):
            y1= k*i+now.y
            ok = cel(now.x+i,y1)
            if self.map[ok.x][ok.y].value in [2,4,5,6,11]:
                return True
        return False

    def near_chest(self,now:cel,api)->bool:
        eight = [cel(now.x,now.y+1),cel(now.x,now.y-1),cel(now.x+1,now.y),cel(now.x-1,now.y),cel(now.x+1,now.y+1),cel(now.x-1,now.y-1),cel(now.x+1,now.y-1),cel(now.x-1,now.y+1)]
        for i in eight:
            jindu = api.GetChestProgress(i.x,i.y)
            if self.map[i.x][i.y].name == 'Chest' and  jindu < Constants.maxChestProgress:
                return True
        return False
    # student

    
    allgates=[]
    allwindows = []
    alldoors3 = []
    alldoors5 = []
    alldoors6 = []
    allclassrooms = []
    allboxes = []
    allhiddengates = []

    last_cel:Cel
    cur_cel:Cel
    #movetime=10#移动时间是多少？
    #nowpath:deque[Cel]#存BFS得出的路径直到路径走�??
    #move_along_path=0#直到走完路径，避免每帧调用一�??

    def Todestin(self,api:IStudentAPI):
        return 

    def Iscanreach(self,point:Cel,target:Cel)->bool:#判断两个点能否直接到
        if(target.x<0 or target.x>50 
           or target.y<0 or target.y>50
           or self.map[target.x][target.y].name=='Wall'
           or (target.x==point.x and target.y==point.y)):#或超出地图、墙壁、与初始点重�??
            return False
        else:#上下左右�??
            if(abs(point.x-target.x)+abs(point.y-target.y)==1):
                return True
            else:#斜对角线是门的话先不�??
                name1=self.map[target.x][target.y].name
                if(name1=='Door3' or name1=='Door5' or name1=='Door6'):
                    return False
                else:
                    name2=self.map[target.x][point.y].name
                    name3=self.map[point.x][target.y].name
                    if name2=='Wall' or name3=='Wall':
                        return False
                    else:
                        return True
                #对角线的两侧有墙壁的话也不行�??45度的时候走不过�??
                
    #隐藏门和大校门？
    def getSurroundCell(self,info:THUAI6.Student,point:Cel)->deque[Cel]:#找到该点周围8个点中能到的
        surroundCell=[]#存能到的
        curprop=info.prop
        global next
        for xi in range(point.x-1,point.x+2):
            for yj in range(point.y-1,point.y+2):
                other=Cel(xi,yj)
                if(self.Iscanreach(point,other)):
                    sname=self.map[other.x][other.y].name
                    if(sname=='Door3'):#是门的话，判断有无对应钥匙？
                        if 1 in curprop:
                            next=Cel(2*other.x-point.x,2*other.y-point.y)
                    elif(sname=='Door5'):
                        if 2 in curprop:
                            next=Cel(2*other.x-point.x,2*other.y-point.y)
                    elif(sname=='Door6'):
                        if 3 in curprop:
                            next=Cel(2*other.x-point.x,2*other.y-point.y)
                    elif(sname=='Window'):#是窗的话爬出�??
                        next=Cel(2*other.x-point.x,2*other.y-point.y)
                    else:
                        next=other
                    surroundCell.append(next)
        return surroundCell
    
    #判断坐标是否在list里，searched用到，也是数字相同但不同对象
    def InList(self,point:Cel,llist:List)->bool:
        for k in llist:
            if point.x==k.x and point.y==k.y:
                return True
        return False
    
    #坐标是否在字典的key里，每一次初始化类的对象，尽管数字相同但还是不同对象，可以重�??=？还可以？（后面简化一�??
    def InDic(self,point:Cel,DDix:dict)->bool:
        for k in DDix.keys():
            if point.x==k.x and point.y==k.y:
                return True
        return False

    #广度搜索找路径，如果
    #路径不存在的情况�??
    def BFSpath(self,info:THUAI6.Student,point:Cel,target:Cel,api:IStudentAPI)->List[Cel]:
        #self_info=api.GetSelfInfo(self)
        searched=[]#已经搜过的点
        q=deque()
        q.append(point)#初始点加�??
        
        #定义搜索过的点的父节�??
        parents=dict()
        path=[target]

        while q:
            cur_=q.popleft()
            #如果点没有被搜过
            if not self.InList(cur_,searched):
                #api.Print('not serached'+'cur_ '+str(cur_.x)+' '+str(cur_.y))
                if(cur_.x !=target.x or cur_.y!=target.y):
                    #print('!=here')
                    sur=self.getSurroundCell(info,cur_)
                    for k in sur:
                        if not self.InList(k,searched):
                            q.append(k)
                            if not self.InDic(k,parents):
                                parents[k]=cur_
                    searched.append(cur_)
                else:
                    #print('cur_=target')
                    key=Cel(target.x,target.y)
                    while key.x!=point.x or key.y!=point.y:
                        
                        for k in parents:
                            if k.x==key.x and k.y==key.y:
                                father=parents[k]
                        path.append(father)
                        key=father
                    #往回找到路径后，将列表反转
                    path.reverse()
                    
                    break
        
        return path

    #�?7间教室学�?
    def Study(self,api:IStudentAPI)->None:
        leng=len(self.allclassrooms)
        ifdone=np.zeros((leng,1),dtype=bool)
        
        class_count=0#已经学的教室的间�?
        #找到距离self最近的教室并前往，途中捡道�?
        while class_count<7:
            my_filter=filter(lambda x:ifdone[self.allclassrooms.index(x)]==0,self.allclassrooms)#已经学过的教室就不要去了
            result=list(my_filter)
            dis_to_class=[]
            for k in result:
                dis=distance(k,api,self)
                dis_to_class.append(dis)
            num=dis_to_class.index(min(dis_to_class))#result中距离最近的教室的编�?
            which_class=result[num]
            x1=which_class.x
            y1=which_class.y
            #print('nearest ',x1,y1)
            #在classroom列表中找回这个点的位置，并在ifdone中修改标�?
            index_of_class=next((i for i,xx in enumerate(self.allclassrooms) if xx.x==x1 and xx.y==y1),None)
            ifdone[index_of_class]=1

            Goto(which_class,api,self)
            api.StartLearning()#开始学�?

            class_count=class_count+1



    # function

    def __init__(self, pID: int):
        self.__playerID = pID

    def StudentPlay(self, api: IStudentAPI) -> None:
        return
        # 鍏叡鎿嶄綔
        if self.map==None:
            self.map=api.GetFullMap()
            xi=0
            yj=0
            for i in self.map:
                yj=0
                for j in i:
                    if j.name=='ClassRoom':
                        self.allclassrooms.append(Cel(xi,yj))
                    if j.name=='Gate':
                        self.allgates.append(Cel(xi,yj))
                    if j.name=='HiddenGate':
                        self.allhiddengates.append(Cel(xi,yj))
                    if j.name=='Window':#��һ���������ĺ�������
                        self.allwindows.append(Cel(xi,yj))
                    if j.name=='Door3':
                        self.alldoors3.append(Cel(xi,yj))
                    if j.name=='Door5':
                        self.alldoors5.append(Cel(xi,yj))
                    if j.name=='Door6':
                        self.alldoors6.append(Cel(xi,yj))
                    if j.name=='Chest':
                        self.allboxes.append(Cel(xi,yj))
                    yj=yj+1
                xi=xi+1
            print(self.map)


        if self.__playerID == 0:
            selfinfo=api.GetSelfInfo()#获取自身信息
            Props=api.GetProps()#获取场上所有可见道�?
            students=api.GetStudents()#获取其他所有学生的信息
            
            api.PrintSelfInfo()
            box1=self.allboxes[0]
          
            Goto(box1,api,self)
            #api.StartLearning()
            #api.PrintSelfInfo()
            #print('end')
            #self.Study(api)
           
            
            
            
            
            
        elif self.__playerID == 1:
            # 鐜╁�??1鎵ц鎿嶄�??
            selfinfo=api.GetSelfInfo()#获取自身信息
            Props=api.GetProps()#获取场上所有可见道�?
            students=api.GetStudents()#获取其他所有学生的信息
            
            api.PrintSelfInfo()
            box1=self.allboxes[0]
          
            Goto(box1,api,self)
            return
        elif self.__playerID == 2:
            # 鐜╁�??2鎵ц鎿嶄�??
            selfinfo=api.GetSelfInfo()#获取自身信息
            Props=api.GetProps()#获取场上所有可见道�?
            students=api.GetStudents()#获取其他所有学生的信息
            
            api.PrintSelfInfo()
            box1=self.allboxes[0]
          
            Goto(box1,api,self)
            return
        elif self.__playerID == 3:
            # 鐜╁�??3鎵ц鎿嶄�??
            selfinfo=api.GetSelfInfo()#获取自身信息
            Props=api.GetProps()#获取场上所有可见道�?
            students=api.GetStudents()#获取其他所有学生的信息
            
            api.PrintSelfInfo()
            box1=self.allboxes[0]
          
            Goto(box1,api,self)
            return
        # 鍙互鍐欐垚if self.__playerID<2涔嬬被鐨勫啓娉�
        # 鍏叡鎿嶄綔
        return
    

    def TrickerPlay(self, api: ITrickerAPI) -> None:
        
        #api.EndAllAction()

        if self.map is None:
            self.map = api.GetFullMap()
            yj = 0
            xi = 0
            for i in self.map:
                yj = 0
                for j in i:
                    if j.name == 'ClassRoom':
                        self.allclassrooms_tr.append(cel(xi,yj))
                    elif j.name == 'Gate':
                        self.allgates_tr.append(cel(xi,yj))
                    elif j.name == 'HiddenGate':
                        self.allhiddengates_tr.append(cel(xi,yj))
                    elif j.name == 'Window':
                        self.allwindows_tr.append(cel(xi,yj))
                    elif j.name == 'Door3':
                        self.alldoor3_tr.append(cel(xi,yj))
                    elif j.name == 'Door5':
                        self.alldoor5_tr.append(cel(xi,yj))
                    elif j.name == 'Door6':
                        self.alldoor6_tr.append(cel(xi,yj))
                    elif j.name == 'Chest':
                        self.allboxes_tr.append(cel(xi,yj))
                    yj = yj + 1
                xi = xi + 1

        if self.my_previous_frame is None:
            self.my_previous_frame = precise_cel(0,0)
        
        selfInfo = api.GetSelfInfo()
        self.my_this_frame = precise_cel(selfInfo.x,selfInfo.y)        


        if selfInfo.trickerType.name == 'ANoisyPerson':
            student_see = api.GetStudents()
            now = cel(selfInfo.x,selfInfo.y,1)
            dest= cel(0,0)
            this_dest:cel
            if self.near_chest(now,api):
                api.StartOpenChest()
            if selfInfo.playerState.name == 'Idle':
                if len(student_see) == 0:
                    self.move_along_path = True
                    dest = cel(0,0)
                    if selfInfo.trickDesire == 0:
                        #找教室
                        dest = self.closest_classroom(now.x,now.y)
                    else:
                        dis = float(ANoisyPerson.alertnessRadius)/selfInfo.trickDesire
                        a=1
                else:
                    self.move_along_path = False
                    closestudent:THUAI6.Student
                    closestudent = None
                    cloest = 1000000000000
                    attacklong = Constants.basicAttackShortRange
                    for i in student_see:
                        dis = dist(i.x,i.y,selfInfo.x,selfInfo.y)
                        if dis < cloest :
                            cloest = dis
                            if i.playerState.name != 'Addicted':
                                closestudent = i
                    if closestudent is not None:
                        dest = cel(closestudent.x,closestudent.y,1)
                    else:
                        dest = self.closest_chest(now.x,now.y,api)
                        #self.move_along_path = True
                    if closestudent is not None:
                        if closestudent.studentType.name == 'Athlete':
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
                        elif closestudent.studentType.name == 'Teacher':
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
            # if self.now_path is None and self.move_along_path == True: 
            #     self.now_path = findway_DFS(self.map,now,dest)
            # if self.move_along_path == True:
            #     dest = self.now_path[1]
            this_dest = dest
            angle = toangle(selfInfo.x,selfInfo.y,this_dest.gridx,this_dest.gridy)
            if self.my_previous_frame.x == self.my_this_frame.x and self.my_previous_frame.y == self.my_this_frame.y:
                if selfInfo.playerState.name == 'Idle':
                    api.MoveUp(10)
                    return
            self.my_previous_frame = precise_cel(selfInfo.x,selfInfo.y)
            if selfInfo.playerState.name == 'Idle':
                api.Move(60,angle)
            # hide in grass
            # attack
            # trick around
            # find chest
            # find classroom
        elif selfInfo.trickerType.name == 'Idol':
            a=1
        elif selfInfo.trickerType.name == 'Assassin':
            a=1

        return

#  playerID=4, GUID=678, x=21500, y=25500
#  speed=3960, view range=15600, place=Land, radius=400
#  score=0, facing direction=0.0, skill time=[0.0, 0.0, -1.0]
#  state=Idle, bullet=CommonAttackOfTricker, prop=NullPropType, NullPropType, NullPropType,
#  type=Assassin, trick desire=2.4405367634838275, class volume=0.0
#  buff=

#  playerID=2, GUID=694, x=12500, y=26500
#  speed=2880, view range=9000, place=Land, radius=400
#  score=0, facing direction=0.0, skill time=[0.0, -1.0, -1.0]
#  state=Idle, bullet=NullBulletType, prop=NullPropType, NullPropType, NullPropType,
#  type=StraightAStudent, determination=3300000, addiction=0, danger alert=0.0
#  learning speed=135, encourage speed=100, encourage progress=0, rouse progress=0
#  buff=
