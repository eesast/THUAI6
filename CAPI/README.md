# CAPI

## 简介

C++ 通信组件与选手接口

## 目标

### 基本目标

- 基于 Protobuf 与 gRPC，为客户端提供 C++ 通信组件
- 为选手提供游戏接口

### 重要目标

- 理解 RPC 的工作原理，使用 gRPC 完善通信逻辑
- 将通信逻辑与游戏逻辑分离开，便于日后复用
- 客户端不对游戏人数、观战人数做出任何限制，这些方面全都由服务器决定
- 改进选手接口，思考如何强制禁止选手一直占用 CPU 而导致 CPU 占用过大的问题。

### 提高目标

- 提供其他语言的接口：Python、Java、Rust ......

## 统一约定

- 主要使用现代 C++ 进行编程
- 代码应当能够同时运行在 Windows 10 平台和 Linux 平台上。Windows 平台下采用 MSVC 作为编译工具，Linux 平台采用 GCC 作为编译工具
- Windows 下的开发工具使用 Visual Studio 2019 或 Visual Studio 2022，语言标准采用 C++17 和 C17 （MSVC 编译选项 `/std:c++17; /std:c17`），并且应同时在 x64 平台的 Debug 与 Release 模式下正确编译并运行  
- Linux 下 C 语言编译工具使用 gcc，语言标准为 `-std=c17`；C++ 编译工具使用 g++，语言标准为 `-std=c++17`。优化选项为 `-O2`，生成 64 位程序 `-m64`，并编写相应的 Makefile  

## 注意事项

- 与逻辑组共同商议通信协议
- Visual Studio 新建的 C++ 代码文件默认字符编码是 GB2312、默认缩进方式是 TAB，应该注意手动改成 UTF-8 和 空格缩进  
- 了解 Visual Studio 的项目属性配置，例如第三方库的链接、预定义宏等  
- 使用现代 C++ 特性进行编程，避免不安全的旧特性，例如：
  + 尽量避免裸指针，多使用智能指针
  + 禁止使用 `::operator new`、`::operator new[]`；使用 `std::make_unique`、`std::make_shared`、`std::vector` 等代替；
  + 禁止使用宏作为编译期常量；使用 `constexpr` 代替；
  + 善用 attribute，例如 `[[nodiscard]]`、`[[fallthrough]]`、`[[noreturn]]` 等；
  + C 语言接口需要基于 RAII 原则进行封装，通过对象的生命周期来关系资源的申请和释放；
  + ……
- 了解 C、C++ 预处理、编译、链接的基本概念  
- 注意模块化、单元化，降低各个类、各个模块之间的耦合。特别注意避免相互依赖、环形依赖的问题  
- 遵循头文件（`.h`、`.hpp`）的编写规范  
  + 杜绝头文件相互包含与环形包含  
  + 头文件中最好同时写 `#pragma once` 以及保护宏，而 `cpp` 中不要写这两个东西  
  + 头文件中**禁止** `using namespace std`！！！也不允许在任何自定义的名字空间中 `using namespace std`！！！  
  + 头文件和 `cpp` 文件各司其职，代码写在改写的位置  
  + 禁止 include .cpp 或 .c 文件
- 避免忙等待，注意线程安全，做好线程同步  
- 善于使用 [Google](https://www.google.com/) 并使用[**英文**](https://en.wikipedia.org/wiki/American_English)搜索，善于查阅 [Microsoft Learn](https://learn.microsoft.com/)、[cppreference](https://en.cppreference.com/)、[StackOverflow](https://stackoverflow.com/) 以及第三方库官方文档等；不应轻信 [CSDN](https://www.csdn.net/) 等劣质博客社区以及[博客园](https://www.cnblogs.com/)、[简书](https://www.jianshu.com/)等质量参差不齐的博客社区，对其内容需全方位多角度仔细求证方可相信

## 开发人员

- ......（自己加）
