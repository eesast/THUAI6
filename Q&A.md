# Q&A

##  grpc版本更新失败

Q：运行GeneratePythonProto.cmd报错
![grpcUpdate](.\resource\Q&A_grpc.png)

A：
- 可能措施1.
    首先保证Python版本在3.9及以上
- 可能措施2. 更换为国内镜像源
    在终端输入 pip config set global.index-url https://pypi.tuna.tsinghua.edu.cn/simple
- 可能措施3. 更新pip
    python -m pip install --upgrade pip
    （pip 版本最好为23.1）

Q: Windows找不到文件：\CAPI\cpp\x64\Debug\APl.exe？

A: 
解决办法：打开CAPI\cpp目录，在里面打开CAPI.sln，然后点击生成，对代码进行编译

Q: 怎么开始游戏？

A: 
需要确保学生阵营和捣蛋鬼阵营的人数都达到Server.cmd中设定的值。现有代码只能保证一个Client对应一个阵营，如果想要两侧同时运行，有三种办法：对于C++，可以先对一个阵营编译，然后修改API.exe的文件名，然后再更改AI.cpp中指定的阵营，对另一个阵营编译，最后修改RunCpp.cmd中的目标文件；对于Python，可以将整个python接口复制一份（CAPI\python文件夹），在第二份代码中修改阵营，然后同样修改RunPython.cmd中的目标文件；也可以打开WPF，方法是参考问题4中的教程，修改RunGUIClient.cmd的参数，然后运行RunGUIClient.cmd，这样可以通过WPF运行部分客户端，来达到人数限制。

Q: Mac怎么用？

A:
解决办法：安装Windows虚拟机

Q: 怎么修改.cmd参数？

A:
见选手包中的使用文档部分
