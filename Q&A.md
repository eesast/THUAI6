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

