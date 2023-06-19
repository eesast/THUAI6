import PyAPI.structures as THUAI6
from PyAPI.Interface import IStudentAPI, ITrickerAPI, IAI
from typing import Union, Final, cast, List
from PyAPI.constants import Constants
import queue
import math
import time
from queue import PriorityQueue

import time

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

#定义启发函数，使用欧式距离作为估计代价
def heuristic(node, end):
    dx = abs(node[0] - end[0])
    dy = abs(node[1] - end[1])
    return dx + dy

#定义一个函数，用来计算两个节点之间的移动代价，这里假设水平或垂直移动代价为1，斜向移动代价为根号2
def get_cost(node1, node2):
    dx = abs(node1[0] - node2[0])
    dy = abs(node1[1] - node2[1])
    return dx + dy

#定义一个函数，用来获取一个节点的所有邻居节点（8方向）
def get_neighbors(node, node_g, end, map):
    neighbors = {}
    for dx in [-1, 0, 1]:
        for dy in [-1, 0, 1]:
            if dx == 0 and dy == 0:
                continue #跳过自身
            x = node[0] + dx
            y = node[1] + dy
            if x < 0 or x >= len(map[0]) or y < 0 or y >= len(map[0]):
                continue #跳过越界的节点
            if map[x][y] != THUAI6.PlaceType.Land and map[x][y] != THUAI6.PlaceType.Grass and map[x][y] != THUAI6.PlaceType.Window:
                continue #跳过障碍物节点
            if dx == -1 and dy == -1:# 左下方
                if map[x][y] == THUAI6.PlaceType.Window: #如果窗户在对角，则跳过
                    continue
                if map[x][y + 1] != THUAI6.PlaceType.Land and map[x][y + 1] != THUAI6.PlaceType.Grass:
                    continue
                if map[x + 1][y] != THUAI6.PlaceType.Land and map[x + 1][y] != THUAI6.PlaceType.Grass:
                    continue
            elif dx == -1 and dy == 1:# 左上方
                if map[x][y] == THUAI6.PlaceType.Window: #如果窗户在对角，则跳过
                    continue
                if map[x][y - 1] != THUAI6.PlaceType.Land and map[x][y - 1] != THUAI6.PlaceType.Grass:
                    continue
                if map[x + 1][y] != THUAI6.PlaceType.Land and map[x + 1][y] != THUAI6.PlaceType.Grass:
                    continue
            elif dx == 1 and dy == -1:# 右下方
                if map[x][y] == THUAI6.PlaceType.Window: #如果窗户在对角，则跳过
                    continue
                if map[x][y + 1] != THUAI6.PlaceType.Land and map[x][y + 1] != THUAI6.PlaceType.Grass:
                    continue
                if map[x - 1][y] != THUAI6.PlaceType.Land and map[x - 1][y] != THUAI6.PlaceType.Grass:
                    continue
            elif dx == 1 and dy == 1:# 右上方
                if map[x][y] == THUAI6.PlaceType.Window: #如果窗户在对角，则跳过
                    continue
                if map[x][y - 1] != THUAI6.PlaceType.Land and map[x][y - 1] != THUAI6.PlaceType.Grass:
                    continue
                if map[x - 1][y] != THUAI6.PlaceType.Land and map[x - 1][y] != THUAI6.PlaceType.Grass:
                    continue
            G = node_g + get_cost(node, (x, y))
            H = heuristic((x, y), end)
            F = G + H
            neighbors[(x, y)] = (G, H, F)
    return neighbors

# A*寻路
# start: 起点坐标(元组), end: 终点坐标(元组), map: 地图（二维数组）
def AStarSearch(start, end, map):
    #初始化open_set和close_set，分别用来存放待遍历的节点和已遍历的节点
    open_set = set()
    close_set = set()
    #将起点加入open_set，并计算其F值
    start_node = (start[0], start[1], 0, heuristic(start, end), heuristic(start, end))
    open_set.add((start_node[0], start_node[1]))
    #初始化邻居字典和父节点字典，用来存储节点之间的映射关系
    values = {}
    parents = {}
    values[start_node[:2]] = start_node[2:]
    parents[start_node[:2]] = None
    #如果open_set不为空，则循环执行以下步骤：
    while open_set:
        #从open_set中选取F值最小的节点，记为current_node
        current_node = min(open_set, key = lambda x: values[x][2])
        #如果current_node是终点，则从终点开始逐步追踪parent节点，一直达到起点，返回找到的结果路径，算法结束
        if current_node == end:
            path = []
            while current_node:
                path.append(current_node)
                current_node = parents[current_node]
            return path[::-1]
         # 如果current_node不是窗户，将其从open_set中删除，并加入close_set中
        if map[current_node[0]][current_node[1]] != THUAI6.PlaceType.Window:
            open_set.remove(current_node)
            close_set.add(current_node)
        #若是窗户
        else:
            #将其从open_set中删除
            open_set.remove(current_node)
            #将其加入close_set中
            close_set.add(current_node)
            #将其后继节点加入open_set中，后继节点为current_node的父节点与current_node的连线上的下一个节点
            parent_node = parents[current_node]
            next_node = (current_node[0] + (current_node[0] - parent_node[0]), current_node[1] + (current_node[1] - parent_node[1]))
            parents[next_node] = current_node
            open_set.add(next_node)
            #计算next_node的G值和F值
            values[next_node] = (values[current_node][0] + 1, heuristic(next_node, end), values[current_node][0] + 1 + heuristic(next_node, end))
            #跳过本次循环
            continue
        #遍历current_node的所有邻居节点，对于每个邻居节点neighbor，执行以下步骤：
        current_neighbors = get_neighbors(current_node, values[current_node][0], end, map)
        for neighbor in current_neighbors:
            #如果neighbor在close_set中，则跳过该节点
            if neighbor in close_set:
                continue
            #如果neighbor不在open_set中，则将其加入open_set，将current_node设为其parent节点，并更新value值
            if neighbor not in open_set:
                open_set.add(neighbor)
                parents[neighbor] = current_node
                values[neighbor] = current_neighbors[neighbor]
            #如果neighbor已经在open_set中，则比较其原来的G值和通过current_node到达它的G值，如果后者更小，则更新其G值和F值，并将current_node设为其parent节点
            else:
                old_G = values[neighbor][0]
                new_G = values[current_node][0] + get_cost(current_node, neighbor)
                if new_G < old_G:
                    values[neighbor] = (new_G, values[neighbor][1], new_G + values[neighbor][1])
                    parents[neighbor] = current_node
        #重复以上步骤，直到open_set为空或找到终点为止
    #如果循环结束后没有返回路径，则说明没有找到可行的路径，返回空列表
    return []

# 接受AStarSearch返回的路径，计算出总路径长度
def GetPathLength(path) -> int:
    length = 0
    for i in range(len(path) - 1):
        length += get_cost(path[i], path[i + 1])
    return length

# 接受AStarSearch返回的路径，将其转换为极坐标表示的移动列表，其中第一个分量为长度，第二个分量为与x轴夹角，第三个分量为x轴坐标变化值，第四个分量为y轴坐标变化值
def GetMoveList(path):
    moveList = []
    for i in range(len(path) - 1):
        dx = path[i + 1][0] - path[i][0]
        dy = path[i + 1][1] - path[i][1]
        angle = math.atan2(dy, dx)
        moveList.append((math.sqrt(dx*dx + dy*dy), angle, dx, dy))
    return moveList

# 步进函数 每次有20ms的延迟预留
def Step(api, selfInfo, start, move, map):
    # 如果下一步的位置为窗户，则执行翻窗操作
    if map[start[0] + move[0]][start[1] + move[1]] == THUAI6.PlaceType.Window:
            api.SkipWindow()
            time.sleep(2000/2100 + 2)
    # 如果当前的位置为窗户，则跳过本次循环
    elif map[start[0]][start[1]] == THUAI6.PlaceType.Window:
        pass
    # 否则执行移动操作
    else:
        length = math.sqrt(move[0] * move[0] + move[1] * move[1])
        angel = math.atan2(move[1], move[0])
        api.Move(int(length * 1000 * 1000 / selfInfo.speed), angel)
        time.sleep((length * 1000 / selfInfo.speed) + 0.02)
    return

class AssistFunction:

    @staticmethod
    def CellToGrid(cell: int) -> int:
        return cell * numOfGridPerCell + numOfGridPerCell // 2

    @staticmethod
    def GridToCell(grid: int) -> int:
        return grid // numOfGridPerCell

class AI(IAI):

    def __init__(self, pID: int):
        self.__playerID = pID

    # 全局变量


    # 初始变量，用于判断是否是第一次调用
    first = True

    # 当前路径
    currentPath = []

    # 之前的目的地
    preDes = ()

    # 测试计数 
    count = 0

    #用于判断是否在写作业
    iswriting=False
    hwfirst=True
    fix_x=0
    fix_y=0
    realcl_x=0
    realcl_y=0
    hwlist=[]
    curx=0
    cury=0

    fixedhw=[]

    movelist0=[(18,5),(30,7),(22,18)]
    movelist1=[(10, 38),(19,41),(8,31)]
    movelist2=[(8,31),(33,40),(10, 38)]
    movelist3=[(40,12),(44,32),(28,26)]

    gatelist=[(5,5),(6,7),(6,5),(6,6)]

    # 初始变量，用于判断是否是第一次调用
    first = True

    # 当前路径
    currentPath = []

    # 之前的目的地
    preDes = ()

    # 当前追杀目标
    currentTarget = None

    # 辅助函数

# 输入查找类型，获取最近的目标，返回值为元组
    def GetNearest(self,api:IStudentAPI,selfx,selfy,map,searchtype):
        a=(0,0)
        distance=1000
        #fixing=[]
        if searchtype==THUAI6.PlaceType.ClassRoom:
            api.Print("下面打印所有作业坐标")
            api.Print(self.hwlist)
            for i in range(len(self.hwlist)):
                dist=abs(selfx-self.hwlist[i][0])+abs(selfy-self.hwlist[i][1])
                if(dist<distance):
                    distance=dist
                    a=(self.hwlist[i][0],self.hwlist[i][1])
            self.hwlist.remove(a)
        else:        
            for i in range(50):
                for j in range(50):
                    if map[i][j]==searchtype:
                        if i!=selfx and j!=selfy:
                            dist=abs(selfx-i)+abs(selfy-j)
                            if(dist<distance):
                                distance=dist
                                a=(i,j)
                            
        return a

        # if(searchtype==THUAI6.PlaceType.ClassRoom):
        #     # while api.HaveMessage():
        #     #             message=api.GetMessage()
        #     #             xx=str(message[1][0])+str(message[1][1])+str(message[1][2])+str(message[1][3])+str(message[1][4])+str(message[1][5])
        #     #             ix=int(xx, base=2)
        #     #             yy=str(message[1][6])+str(message[1][7])+str(message[1][8])+str(message[1][9])+str(message[1][10])+str(message[1][11])
        #     #             iy=int(yy, base=2)
        #     #             a=(ix,iy)
        #     #             fixing.append(a)
        #     for i in range(50):
        #         for j in range(50):
        #             if map[i][j]==searchtype:
        #                 if i!=selfx and j!=selfy:
        #                     dist=abs(selfx-i)+abs(selfy-j)
        #                     frontier.put((dist,(i,j)))
            

        #     while not frontier.empty():
        #         destt=frontier.get()
        #         # if len(fixing)!=0:
        #         #     for i in range(len(fixing)):
        #         #         if fixing[i][0]==destt[1][0] and fixing[i][1]==destt[1][1]:
        #         #             continue
        #         break
       
        # else:
        #     for i in range(50):
        #         for j in range(50):
        #             if map[i][j]==searchtype:
        #                 if i!=selfx and j!=selfy:
        #                     dist=abs(selfx-i)+abs(selfy-j)
        #                     frontier.put((dist,(i,j)))
        #     destt=frontier.get()
            
        # return destt[1]
    
    #输入某点坐标，判断九宫格内的方块是否是墙，返回一个不是墙的坐标
    def GetNearestNotWall(self,x,y,map):
        for i in range(x-1,x+2):
            for j in range(y-1,y+2):
                distt=math.sqrt((x-i)**2+(y-j)**2)
                if map[i][j]!=THUAI6.PlaceType.Wall and abs(x-i)+abs(y-j)<=1 :
                    if i!=x or j!=y:
                        return (i,j)
        

    #学生去写没人占用，且距离最近的作业
    def StuWriteHomework(self,api: IStudentAPI,movelist):
        api.Print("写作业函数被调用了！")
        selfinfo=api.GetSelfInfo()
        map = api.GetFullMap()
        count=0
        flag=0
        curpos=((int)(selfinfo.x/1000),(int)(selfinfo.y/1000))
        for i in range(3):
            a=float(float(api.GetClassroomProgress(int(movelist[i][0]),int(movelist[i][1])))/10000000.0)
            if a>=1:
                count=count+1
        if count==3 or flag == 1:
            flag=1
            self.Walk(api,selfinfo,curpos,(int(self.gatelist[0][0]),int(self.gatelist[0][1])),map)
            if api.IsDoorOpen(5,6)==False:
                api.StartOpenGate()
            else:
                api.Graduate()
        if len(movelist)!=0:
            a=movelist[0]
            api.Print(movelist)
            b=self.GetNearestNotWall(a[0],a[1],map)
            distt=math.sqrt((int(b[0]-curpos[0]))**2+(int(b[1]-curpos[1]))**2)
            if distt==0:
                if selfinfo.studentType==THUAI6.StudentType.Athlete and selfinfo.timeUntilSkillAvailable[0]==0:
                    api.UseSkill(0)
                api.StartLearning()
                api.Print("start learning")
                time.sleep(0.15)
            else:
                self.Walk(api,selfinfo,curpos,(int(b[0]),int(b[1])),map)
            pro=float(float(api.GetClassroomProgress(int(a[0]),int(a[1])))/10000000.0)
            api.Print("打印pro")
            api.Print(pro)
            if pro>=1:
                pro=0
                movelist.remove(a)
                return
        


            

        # target=()
        # distt=math.sqrt((int(self.fix_x-curpos[0]))**2+(int(self.fix_y-curpos[1]))**2)
        
        # #分割线

        # api.Print(curpos)
        # api.Print([self.fix_x,self.fix_y])
        # api.Print("接下来打印目标点")
        # api.Print([self.realcl_x,self.realcl_y])
        # api.Print("接下来打印我的位置")
        # api.Print([curpos[0],curpos[1]])
        # api.Print("接下来打印dist")
        # api.Print(distt)
        # if distt==0:
        #     if selfinfo.studentType==THUAI6.StudentType.StraightAStudent and selfinfo.timeUntilSkillAvailable[0]==0:
        #         api.UseSkill(0)
        
        #     api.StartLearning()
        #     api.Print("start learning")
        #     time.sleep(0.15)
        # else:
        #     self.Walk(api,selfinfo,curpos,(int(self.fix_x),int(self.fix_y)),map)
        # if self.hwfirst==True:
        #     pro=0
        #     self.hwfirst=False
        #     if stuid==0:
        #         distance=1000
        #         for i in range(50):
        #                 for j in range(50):
        #                     if map[i][j]==THUAI6.PlaceType.ClassRoom and i<50-j :
        #                         dist=abs(self.curx-i)+abs(self.cury-j)
        #                         if(dist<distance):
        #                             distance=dist
        #                             classroom=(i,j)

            # if stuid==1:
            #     distance=1000
            #     for i in range(50):
            #             for j in range(50):
            #                 if map[i][j]==THUAI6.PlaceType.ClassRoom and j>i and j <25 :
            #                     dist=abs(self.curx-i)+abs(self.cury-j)
            #                     if(dist<distance):
            #                         distance=dist
            #                         classroom=(i,j)


            # if stuid==2:
            #     distance=1000
            #     for i in range(50):
            #             for j in range(50):
            #                 if map[i][j]==THUAI6.PlaceType.ClassRoom and j>i :
            #                     dist=abs(self.curx-i)+abs(self.cury-j)
            #                     if(dist<distance):
            #                         distance=dist
            #                         classroom=(i,j)

            # if stuid==3:
            #     distance=1000
            #     for i in range(50):
            #             for j in range(50):
            #                 if map[i][j]==THUAI6.PlaceType.ClassRoom and j>50-i :
            #                     dist=abs(self.curx-i)+abs(self.cury-j)
            #                     if(dist<distance):
            #                         distance=dist
            #                         classroom=(i,j)


            # classroom=self.GetNearest(api,(int)(selfinfo.x/1000),(int)(selfinfo.y/1000),map,THUAI6.PlaceType.ClassRoom)
            # api.Print("接下来打印教室位置")
            # api.Print(classroom)
            # api.Print("接下来打印我的位置")
            # api.Print([selfinfo.x/1000,selfinfo.y/1000])
            # self.realcl_x=classroom[0]
            # self.realcl_y=classroom[1]

        # else:
        #     target=self.GetNearestNotWall(self.realcl_x,self.realcl_y,map)
        #     api.Print(target)
        #     self.fix_x=target[0]
        #     self.fix_y=target[1]
        #     pro=float(float(api.GetClassroomProgress(int(self.realcl_x),int(self.realcl_y)))/10000000.0)
        #     api.Print("打印pro")
        #     api.Print(pro)
        #     if pro>=1:
        #         self.hwfirst=True
        #         pro=0
        #         xxx = "".join(f"{self.realcl_x:06b}")
        #         yyy = "".join(f"{self.realcl_y:06b}")
        #         messagee=str(xxx)+str(yyy)
        #         for i in range(4):
        #             if i!=selfinfo.playerID:
        #                 api.SendMessage(i,messagee)
        #                 api.Print("发送消息成功！")
        #                 api.Print(messagee)
        #         return
        #     else:
        

        


        

    def Stuescape(self,api: IStudentAPI):
        selfinfo=api.GetSelfInfo()
        map = api.GetFullMap()
        curpos=((int)(selfinfo.x/1000),(int)(selfinfo.y/1000))
        target=self.GetNearest(api,(int)(selfinfo.x/1000),(int)(selfinfo.y/1000),map,THUAI6.PlaceType.Grass)
        self.Walk(api,selfinfo,curpos,target,map)

    def mStuGogate(self,api: IStudentAPI):
        selfinfo=api.GetSelfInfo()
        map = api.GetFullMap()
        GATE=self.GetNearest(api,(int)(selfinfo.x/1000),(int)(selfinfo.y/1000),map,THUAI6.PlaceType.Gate)
        Dest=self.GetNearestNotWall(GATE[0],GATE[1],map)
        distt=math.sqrt((int(Dest[0]-selfinfo.x/1000))**2+(int(Dest[1]-selfinfo.y/1000))**2)
        if distt>0:
            self.Walk(api,selfinfo,(selfinfo.x,selfinfo.y),(Dest[0],Dest[1]),map)
            return
        pro=float(float(api.GetGateProgress(int(GATE[0]),int(GATE[1])))/18000.0)
        api.Print("打印pro")
        api.Print(pro)
        # if pro>=0.7:
        #     xxx = "".join(f"{Dest:06b}")
        #     yyy = "".join(f"{Dest:06b}")
        #     messagee=str(xxx)+str(yyy)
        #     for i in range(4):
        #         if i!=selfinfo.playerID:
        #             api.SendMessage(i,messagee)
        if api.IsDoorOpen(GATE[0],GATE[1])==False:
            api.StartOpenGate()
        else:
            api.Graduate()


    # 行走函数
    def Walk(self, api, selfInfo, start, end, map): 
        # 初始化
        if self.first:
            self.first = False
            self.preDes = end
            self.currentPath = AStarSearch(start, end, map)
        # 更新路径
        if end != self.preDes:
            self.currentPath = AStarSearch(start, end, map)
        api.Print(self.currentPath)
        # 执行步进函数
        if len(self.currentPath) > 1:
            Step(api, selfInfo, start, (self.currentPath[1][0] - self.currentPath[0][0], self.currentPath[1][1] - self.currentPath[0][1]), map)
            # 更新路径
            self.currentPath.pop(0)
            # 若下一格是窗户，则再pop一次
            if map[self.currentPath[0][0]][self.currentPath[0][1]] == THUAI6.PlaceType.Window:
                self.currentPath.pop(0)
        self.preDes = end

    # 追击函数，target为Studnet类型
    def Chase(self, api, selfInfo, start, target, map):
        # 更新追击目标
        self.currentTarget = target
        # 隐身距离阈值
        invisibleThreshold = target.viewRange + 1200
        # 普通攻击距离阈值
        attackThreshold = 1500
        # 飞刀攻击距离阈值
        knifeDisThreshold = 2500
        # 与目标的距离
        dis = math.sqrt((target.x - selfInfo.x) * (target.x - selfInfo.x) + (target.y - selfInfo.y) * (target.y - selfInfo.y))
        # 如果距离大于隐身阈值，则接近目标
        if dis > invisibleThreshold:
            self.Walk(api, selfInfo, start, ((int)(target.x/1000), (int)(target.y/1000)), map)
        # 如果可以使用飞刀
        elif selfInfo.timeUntilSkillAvailable[1] == 0 or 30000 - selfInfo.timeUntilSkillAvailable[1] <= 590:
            # 如果距离大于飞刀攻击距离阈值
            if dis > knifeDisThreshold:
                # 如果自身不处于隐身状态且隐身技能CD好了，则使用隐身技能
                if selfInfo.buff != THUAI6.TrickerBuffType.Invisible and selfInfo.timeUntilSkillAvailable[0] == 0:
                    api.UseSkill(0)
                # 向目标移动
                self.Walk(api, selfInfo, start, ((int)(target.x/1000), (int)(target.y/1000)), map)
            # 如果距离小于等于飞刀攻击距离阈值
            else:
                # 计算角度
                angel = math.atan2(target.y - selfInfo.y, target.x - selfInfo.x)
                # 飞刀攻击
                api.UseSkill(1)
                api.Attack(angel)
                time.sleep(0.4)
        # 如果飞刀的CD没好
        else:
            # 如果距离大于攻击距离阈值
            if dis > attackThreshold:
                # 如果自身不处于隐身状态，则使用隐身技能
                if selfInfo.buff != THUAI6.TrickerBuffType.Invisible and selfInfo.timeUntilSkillAvailable[0] == 0:
                    api.UseSkill(0)
                # 向目标移动
                self.Walk(api, selfInfo, start, ((int)(target.x/1000), (int)(target.y/1000)), map)
            # 如果距离小于等于攻击距离阈值
            else:
                # 计算角度
                angel = math.atan2(target.y - selfInfo.y, target.x - selfInfo.x)
                # 普通攻击
                api.Attack(angel)
                time.sleep(4)
        # 如果目标死亡
        students = api.GetStudents()
        for student in students:
            if student.playerID == self.currentTarget.playerID and student.determination == 0:
                self.currentTarget = None
                self.currentPath = None
                return

    # 寻踪函数，用于追击丢失目标时的寻踪，lastStudent为目标上一次出现时的信息，为Student类型
    def Track(self, api, selfInfo, curPos, map):
        # 如果学生再爬窗户，则跟上去爬窗
        if self.currentTarget.playerState == THUAI6.PlayerState.Climbing:
            # 返回窗户坐标
            windowPos = self.findAround((self.currentTarget.x, self.currentTarget.y), 1, THUAI6.PlaceType.Window, map)
            # 如果已经翻过窗户
            if [abs(curPos[0] - windowPos[0]) + abs(curPos[1] - windowPos[1])] == 1:
                self.currentTarget = None
                return
            # 向窗户行进
            self.Walk(api, selfInfo, ((int)(selfInfo.x/1000), (int)(selfInfo.y/1000)), windowPos, map)
        # 如果学生没有爬窗户，但周围有草地
        elif self.findAround((self.currentTarget.x, self.currentTarget.y), 2, THUAI6.PlaceType.Grass, map):
            grassPos = self.findAround((self.currentTarget.x, self.currentTarget.y), 2, THUAI6.PlaceType.Grass, map)
            # 如果已经到达草地
            if curPos == grassPos:
                self.currentTarget = None
                return
            # 向草地行进
            self.Walk(api, selfInfo, ((int)(selfInfo.x/1000), (int)(selfInfo.y/1000)), grassPos, map)
        # 如果学生没有爬窗户，周围也没有草地，那可能是被建筑遮挡了，直接跟上去即可
        else:
            if curPos == ((int)(self.currentTarget.x/1000), (int)(self.currentTarget.y/1000)):
                self.currentTarget = None
                return
            self.Walk(api, selfInfo, ((int)(selfInfo.x/1000), (int)(selfInfo.y/1000)), ((int)(self.currentTarget.x/1000), (int)(self.currentTarget.y/1000)), map)
        return

    # 巡逻函数
    def Patrol(self, api, selfInfo, start, map):
        # 获取self.currentPath的最后一个分量
        # 最近的作业的坐标
        api.Print("Patrol")
        homeworkPos = self.GetNearest(api, selfInfo.x, selfInfo.y, map, THUAI6.PlaceType.ClassRoom)
        api.Print(homeworkPos)
        self.Walk(api, selfInfo, (selfInfo.x, selfInfo.y), (homeworkPos[0] - 1, homeworkPos[1]), map)

    # 找寻周围(2a+1)*(2a+1)的格点范围内是否存在某个东西
    def findAround(self, Pos, a, searchType, map):
        # 搜索范围
        searchRange = []
        # 搜索范围左上角坐标
        searchRange.append((Pos[0] - a, Pos[1] - a))
        # 搜索范围右下角坐标
        searchRange.append((Pos[0] + a, Pos[1] + a))
        # 遍历搜索范围内的格点
        for i in range(searchRange[0][0], searchRange[1][0] + 1):
            for j in range(searchRange[0][1], searchRange[1][1] + 1):
                # 如果该格点上有东西
                # 当map[i][j]未越界时
                if i >= 0 and i < len(map) and j >= 0 and j < len(map[0]):
                    if map[i][j] == searchType:
                        return (i, j)
        return None

    # 探查有无学生，若有非teacher的学生则返回最近学生坐标与studentType，否则返回空
    def findStudent(self, api, students, curPos):
        # students非空时
        if students:
            # 找出students中非空也非teacher，同时距离最近的学生，并返回该学生
            minDis = 100000
            minStudent = students[0]
            for student in students:
                if student.studentType != THUAI6.StudentType.NullStudentType and student.studentType != THUAI6.StudentType.Teacher and student.determination > 0:
                    dis = abs(student.x - curPos[0]) + abs(student.y - curPos[1])
                    if dis < minDis:
                        minDis = dis
                        minStudent = student
            if student.studentType != THUAI6.StudentType.NullStudentType and student.studentType != THUAI6.StudentType.Teacher and student.determination > 0:
                return minStudent
        return None

    def GetNearest(self, api, selfx, selfy, map, searchtype):
        a=(0,0)
        distance=1000
        for i in range(50):
            for j in range(50):
                if map[i][j] == searchtype:
                    if i!=selfx and j!=selfy:
                        dist=abs(selfx-i)+abs(selfy-j)
                        if(dist<distance):
                            distance=dist
                            a=(i,j)           
        return a

    # 操作函数
    def StudentPlay(self, api: IStudentAPI) -> None:
        # 公共操作
        aa=3.1
        flag=0
        map=api.GetFullMap()
        selfinfo=api.GetSelfInfo()
        curpos=((int)(selfinfo.x/1000),(int)(selfinfo.y/1000))
        self.curx=selfinfo.x
        self.cury=selfinfo.y
        cllist=[]
        for i in range(50):
                for j in range(50):
                    if map[i][j]==THUAI6.PlaceType.ClassRoom:
                        cllist.append((i,j))
        api.Print(cllist)
        if self.__playerID == 0:
            # 玩家0执行操作
            # while api.HaveMessage():
            #     message=api.GetMessage()
            #     xx=str(message[1][0])+str(message[1][1])+str(message[1][2])+str(message[1][3])+str(message[1][4])+str(message[1][5])
            #     ix=int(xx, base=2)
            #     yy=str(message[1][6])+str(message[1][7])+str(message[1][8])+str(message[1][9])+str(message[1][10])+str(message[1][11])
            #     iy=int(yy, base=2)
            #     a=(ix,iy)
            #     self.fixedhw.append(a)
            #     api.Print("接下来打印fixedhw")
            #     api.Print(self.fixedhw)
            

            if self.first:
                self.first = False
                map = api.GetFullMap()
                for i in range(50):
                    for j in range(50):
                        if map[i][j]==THUAI6.PlaceType.ClassRoom:
                            self.hwlist.append((i,j))
            
            # api.Print(self.hwlist)

            # if(len(self.fixedhw)==7):
            #     self.mStuGogate(api)
            api.PrintSelfInfo() 
            selfinfo = api.GetSelfInfo()
            if(selfinfo.dangerAlert>aa):
                self.Stuescape(api)
            else:
                self.StuWriteHomework(api,self.movelist0)
            
            
            messagee='Hello'
            for i in range(4):
                if i!=selfinfo.playerID:
                    api.SendMessage(i,messagee)
                    flag=1
            if flag==1:
                self.Walk(api,selfinfo,curpos,(int(self.gatelist[0][0]),int(self.gatelist[0][1])),map)
                if api.IsDoorOpen(5,6)==False:
                    api.StartOpenGate()
                else:
                    api.Graduate()


            
            return
        elif self.__playerID == 1:
            # 玩家1执行操作
            selfinfo=api.GetSelfInfo()
            
            if self.first:
                self.first = False
                map = api.GetFullMap()
                for i in range(50):
                    for j in range(50):
                        if map[i][j]==THUAI6.PlaceType.ClassRoom:
                            self.hwlist.append((i,j))

            if(len(self.fixedhw)==7):
                self.mStuGogate(api)
            api.PrintSelfInfo() 
            selfinfo = api.GetSelfInfo()
            if(selfinfo.dangerAlert>aa):
                self.Stuescape(api)
            else:
                self.StuWriteHomework(api,self.movelist1)
            

            while api.HaveMessage():
                message=api.GetMessage()
                if(message[1]=='Hello'):
                    flag=1
                    break
            if flag==1:
                self.Walk(api,selfinfo,curpos,(int(self.gatelist[0][0]),int(self.gatelist[0][1])),map)
                if api.IsDoorOpen(5,6)==False:
                    api.StartOpenGate()
                else:
                    api.Graduate()


            
            return
        elif self.__playerID == 2:
            # 玩家2执行操作

            if self.first:
                self.first = False
                map = api.GetFullMap()
                for i in range(50):
                    for j in range(50):
                        if map[i][j]==THUAI6.PlaceType.ClassRoom:
                            self.hwlist.append((i,j))


            if(len(self.fixedhw)==7):
                self.mStuGogate(api)
            api.PrintSelfInfo() 
            selfinfo = api.GetSelfInfo()
            if(selfinfo.dangerAlert>aa):
                self.Stuescape(api)
            else:
                self.StuWriteHomework(api,self.movelist2)
            

            while api.HaveMessage():
                message=api.GetMessage()
                if(message[1]=='Hello'):
                    flag=1
                    break
            if flag==1:
                self.Walk(api,selfinfo,curpos,(int(self.gatelist[0][0]),int(self.gatelist[0][1])),map)
                if api.IsDoorOpen(5,6)==False:
                    api.StartOpenGate()
                else:
                    api.Graduate()


            
            return
        elif self.__playerID == 3:
            # 玩家3执行操作

            if self.first:
                self.first = False
                map = api.GetFullMap()
                for i in range(50):
                    for j in range(50):
                        if map[i][j]==THUAI6.PlaceType.ClassRoom:
                            self.hwlist.append((i,j))

            if(len(self.fixedhw)==7):
                self.mStuGogate(api)
            api.PrintSelfInfo() 
            selfinfo = api.GetSelfInfo()
            if(selfinfo.dangerAlert>aa):
                self.Stuescape(api)
            else:
                self.StuWriteHomework(api,self.movelist2)
            

            while api.HaveMessage():
                message=api.GetMessage()
                if(message[1]=='Hello'):
                    flag=1
                    break
            if flag==1:
                self.Walk(api,selfinfo,curpos,(int(self.gatelist[0][0]),int(self.gatelist[0][1])),map)
                if api.IsDoorOpen(5,6)==False:
                    api.StartOpenGate()
                else:
                    api.Graduate()

        # 可以写成if self.__playerID<2之类的写法
        # 公共操作
        return

    def TrickerPlay(self, api: ITrickerAPI) -> None:
        selfInfo = api.GetSelfInfo()
        api.PrintSelfInfo()
        # 获取地图信息
        map = api.GetFullMap()
        # 获取视野内学生信息
        students = api.GetStudents()
        # 获取当前坐标
        curPos = ((int)(selfInfo.x/1000), (int)(selfInfo.y/1000))
        api.Print(curPos)

        # 探查有无可攻击的学生，student为Student类型
        stu = self.findStudent(api, students, curPos)
        api.Print(stu)
        # 发现学生，追击
        if stu:
            api.Print("Chasing Target")
            self.Chase(api, selfInfo, curPos, stu, map)
        # 未发现学生，但有追击目标，寻踪
        elif self.currentTarget:
            api.Print("Tracking Target")
            api.Print(self.currentTarget.x)
            api.Print(self.currentTarget.y)
            self.Track(api, selfInfo, curPos, map)
        # 未发现学生，且无记录最后一次学生出现的信息，巡逻
        else:
            api.Print("Nothing")
            self.Patrol(api, selfInfo, curPos, map)
        return
