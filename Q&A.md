# Q&A
[TOC]

## 常见简单问题

Q: Windows找不到文件：\CAPI\cpp\x64\Debug\APl.exe？

A: 
应该是还没有编译，打开CAPI\cpp目录，在里面打开CAPI.sln，然后点击生成，对代码进行编译

Q：为什么CAPI.sln打开后会找不到项目
![zip](.\resource\zip.png)

A:要先解压

Q: 怎么修改.cmd参数？

A:
见选手包中的使用文档部分


Q: 怎么开始游戏？

A: 
需要确保学生阵营和捣蛋鬼阵营的人数都达到Server.cmd中设定的值。人数不足也可以打开WPF，参考使用文档，修改RunGUIClient.cmd的参数，然后运行RunGUIClient.cmd，这样可以通过WPF运行部分客户端，来达到人数限制。


Q: Mac怎么用？

A:
安装Windows虚拟机

## C++ 

Q:显示API项目已卸载
![Nocplus](.\resource\Nocplus.png)

A:可能是没有安装C++

Q:CAPI.sln编译不通过
情况1：
![std_find_trivial](.\resource\std_find_trivial.jpg)
情况2：
![LNK1000](.\resource\LNK1000.png)

A:
1. 应该是没使用VS2022
2. 对于VS2022依旧报错，先考虑是否版本过旧（应当在 17.5.x 版本）
   用Visual Studio Installer（在开始菜单搜索）更新
   ![VsUpdate](.\resource\VsUpdate.png)
3. 
   ![项目属性](.\resource\项目属性.png)
   确保上图项目属性中平台工具集在V143及以上，C++17标准

## Python 

### grpc版本更新失败

Q：运行GeneratePythonProto.cmd报错
![grpcUpdate](.\resource\Q&A_grpc.png)

A：
- 可能措施1.
首先保证Python版本在3.9及以上
- 可能措施2. 更换为国内镜像源
在终端输入 
        `pip config set global.index-url https://pypi.tuna.tsinghua.edu.cn/simple`
- 可能措施3. 更新pip
`python -m pip install --upgrade pip` （pip 版本最好为23.1）

## 比赛相关
Q：职业数值会修改吗？

A：初赛结束会调数值及机制，增加新角色