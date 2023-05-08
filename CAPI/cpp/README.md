# CAPI: cpp

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

## 开发纪实

### C++ SDK Windows 环境配置

#### 配置方法

起初 C++ 开发时使用 Linux 开发，但在选手包发布 Windows SDK 时遇到诸多问题。Windows 平台的 C++ 接口使用 Visual Studio 进行环境配置。期间遇到诸多问题，配置环境方法如下：

1. 更新 Visual Studio：由于届时要求选手与开发组使用**完全相同**版本的 Visual Studio，否则可能会出现找不到一些标准库内置的符号的问题，因此在选手包发布前夕，将开发组的 Visual Studio 升级到最新版后执行步骤 2
2. 编译 gRPC：分别编译 Debug 和 Release 的 gRPC 依赖库各一份。方法有二：
  - 方法一：根据 [gRPC Windows 生成教程](https://github.com/grpc/grpc/blob/0b2609a61d50f2dbba4604978ef9243543a2675a/BUILDING.md#windows-using-visual-studio-2019-or-later) 在本地分别编译一份 `Debug` 和 `Release`（即在 CMake config 参数中分别指定 Debug 和 Release 各生成一份 Visual Studio 工程）的 x64 平台的依赖库。注意使用静态编译，根据[此原则](https://github.com/grpc/grpc/blob/0b2609a61d50f2dbba4604978ef9243543a2675a/BUILDING.md#windows-a-note-on-building-shared-libs-dlls)**不生成** gRPC 和 Protocol Buffers 本身的 DLL，即只有 `.lib` 无 `.dll`（但是可能会有一些其依赖的第三方库的 `.dll`），将 Debug 和 Release 生成的 `.lib` 和 `.dll`（如果有的话）均分别保存
  - 方法二：根据 [gRPC vcpkg 安装教程](https://github.com/grpc/grpc/tree/master/src/cpp#install-using-vcpkg-package) 安装 **`x64-windows-static`** 的 gRPC，即执行 `vcpkg install grpc:x64-windows-static`，以生成静态库（注意 vcpkg 需要调用本地的 Visual Studio 进行现场编译和生成，所以所耗时间可能较长，请耐心等待；并且此步骤由于需要从 GitHub 上现场拉源代码，因此可能需要科学上网）。然后进入 vcpkg 的安装目录，将里面安装的 gRPC 的所有 Debug 和 Release 的 `.lib` 和 `.dll`（如果有的话）分别保存一份
3. 提取 gRPC 头文件：参考 [gRPC 所有所需头文件](https://github.com/eesast/THUAI6/tree/217d87aeedf6735e3257acb1e2c70574df45b823/CAPI/cpp/grpc)将头文件放入本仓库的相应目录内，以供使用（记得放该版本对应的 LICENSE x）。
4. 配置 `.vcxproj` 和 `.vcxproj.filters`：`.vcxproj` 用于配置项目的编译与链接，`.vcxproj.filters` 用于配置 Vsiual Studio 的筛选器的结构（即解决方案资源管理器的结构）。参考 THUAI6 的 [`API.vcxproj`](https://github.com/eesast/THUAI6/blob/c8e1fbe299c67a6e101fa02e85bcc971acd0f48b/CAPI/cpp/API/API.vcxproj) 和 [`API.vcxproj.filters`](https://github.com/eesast/THUAI6/blob/c8e1fbe299c67a6e101fa02e85bcc971acd0f48b/CAPI/cpp/API/API.vcxproj.filters)。基本上可以直接无脑复制过来，不用做什么修改。如果非要在 Visual Studio 里手动配置，需要配置以下几项（以下的链接均以 X64 Release 为例，Debug 的相应配置在相应链接的上面一些的位置）：
  - 添加头文件和源文件：[将**自己编写**的 `.h` 和 `.cpp` 文件加入到项目中](https://github.com/eesast/THUAI6/blob/c8e1fbe299c67a6e101fa02e85bcc971acd0f48b/CAPI/cpp/API/API.vcxproj#L169-L197)（包括 protoc 生成的那些 `.pb.h` 和 `.pb.cc`，但这些最好参照 `API.vcxproj.filters` 的组织结构[各单独开一个子筛选器](https://github.com/eesast/THUAI6/blob/c8e1fbe299c67a6e101fa02e85bcc971acd0f48b/CAPI/cpp/API/API.vcxproj.filters#L16-L21)放进去以使得 Visual Studio 打开时更美观）
  - 在项目属性中的 Debug 和 Release 的“C/C++”的“语言”中[均设置 C++ 语言标准为 `/std:c++17`](https://github.com/eesast/THUAI6/blob/c8e1fbe299c67a6e101fa02e85bcc971acd0f48b/CAPI/cpp/API/API.vcxproj#L154)、[C 语言标准为 `/std:c17`](https://github.com/eesast/THUAI6/blob/c8e1fbe299c67a6e101fa02e85bcc971acd0f48b/CAPI/cpp/API/API.vcxproj#L155)
  - 在项目属性中的 Debug 和 Release 的[附加编译选项（即在“C/C++”的“命令行”）里均增加 `/source-charset:utf-8` 选项](https://github.com/eesast/THUAI6/blob/c8e1fbe299c67a6e101fa02e85bcc971acd0f48b/CAPI/cpp/API/API.vcxproj#L157)，以保证编译时编译器使用 UTF-8 编码进行源文件读取（注意这个与 Visual Studio 的高级保存选项不同，高级保存选项决定了文件的保存时的保存编码，而非编译器编译时读取使用的编码）
  - 设置头文件的搜索路径：在项目属性的“C/C++”的首页的“附加包含目录”中设置 Debug 和 Release 下的[搜索头文件的根路径](https://github.com/eesast/THUAI6/blob/c8e1fbe299c67a6e101fa02e85bcc971acd0f48b/CAPI/cpp/API/API.vcxproj#L156)，注意 Windows 下的路径分隔符最好使用 `\` 而非 `/`，负责可能会出现一些玄学问题
  - 设置生成方式为静态生成：在项目属性的“C/C++”的“代码生成”的“运行库”中，[Debug 下设置其为“多线程调试（`/MTd`）”](https://github.com/eesast/THUAI6/blob/c8e1fbe299c67a6e101fa02e85bcc971acd0f48b/CAPI/cpp/API/API.vcxproj#L137)，[Release 下设置其为“多线程（`/MT`）”](https://github.com/eesast/THUAI6/blob/c8e1fbe299c67a6e101fa02e85bcc971acd0f48b/CAPI/cpp/API/API.vcxproj#L158)
  - 将之前提取的 Debug 和 Release 的 `.lib` 分别放在项目中的单独的文件夹里（THUAI6 使用的是 `CAPI\cpp\lib\debug` 和 `CAPI\cpp\lib\release`），并[使用 `.gitignore` 忽略掉](https://github.com/eesast/THUAI6/blob/c8e1fbe299c67a6e101fa02e85bcc971acd0f48b/CAPI/cpp/.gitignore#L502)
  - 在项目属性的“链接器”的首页的“附加库目录”中分别配置 Debug 和 Release 的 [`.lib` 文件的相应路径](https://github.com/eesast/THUAI6/blob/c8e1fbe299c67a6e101fa02e85bcc971acd0f48b/CAPI/cpp/API/API.vcxproj#L166)
  - 在项目属性中的“链接器”的“输入”的“附加依赖库”中分别配置 Debug 和 Release [所需要链接的库的文件名](https://github.com/eesast/THUAI6/blob/c8e1fbe299c67a6e101fa02e85bcc971acd0f48b/CAPI/cpp/API/API.vcxproj#L165)。注意 Debug 和 Release 链接的库可能并不完全相同，建议在 cmd 中使用 `dir /b` 将其自动列举并复制。还需要注意需要手动指定链接一些 Windows 自带的 `lib`，例如 `Ws2_32.lib`、`Crypt32.lib`、`Iphlpapi.lib` 等。如果生成过程中不通过，表示找不到一些函数，则在 Google 中搜索该函数，如果发现是 Windows 系统的 API 函数则会搜到[微软官方文档](https://learn.microsoft.com) 的对应链接的页面，则在页面最下方会表明它所在的 `.lib`（例如 [`CreateProcessA` 的页面](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa#requirements)），加进去即可
  - 然后进行生成解决方案。如果感觉编译的速度过慢，可以在项目属性的 `C/C++` 的“所有选项”中搜索并行编译，并开启之（`/Qpar`）。不过由于 THUAI6 的疏忽，忘记开启了并行编译
  - 然后开始运行。如果提示缺少一些 DLL，可以把之前保存的 `.dll` 文件（如果有的话）放在与 `.exe` 相同的目录下。该目录为**与 `.sln` 相同目录的**（不是与 `.vcxproj` 相同目录的）`x64\Debug` 和 `x64\Release`
  - 如果 x64 的 Debug 和 x64 的 Release 均生成成功，那么找一台没配过的电脑再试一次
  - 随便写点 AI 代码，重新生成解决方案，确认成功后发布选手包
  - 有兴趣也可以支持一下 x86 的 Windows（x
  - 有兴趣还可以支持一下 Cygwin（x
  
### Q&A

#### 为什么不用 CMake

1. 由于 CMake 生成的 Visual Studio 工程是绝对路径，因此不能将 CMake 生成的 Visual Studio 工程直接给选手，需要选手使用 CMake，增加了选手尤其是大一小白选手的负担
2. 对 Debug 和 Release 分别编译时，需要 CMake 分别生成 Debug 和 Release 的 Visual Studio 工程，而难以使用 Visual Studio 和 MSBuild 自带的 Debug 和 Release 的切换方式，这就需要生成两份 Visual Studio 工程，对选手的测试来说也是一种麻烦

#### 为什么不让选手用 vcpkg

1. vcpkg 是将代码拉下来本地编译。拉代码过程需要科学上网，这对一些大一小白选手来说是个不小的负担
2. vcpkg 编译过程极其漫长，会极大消磨选手的耐心

#### 为什么不用 CLion 配合 MinGW、MinGW-w64 等

1. 贵校程设课大多数使用 Visual Studio。对大多数选手来说，这是最易上手的
2. 开箱即用，方便
3. Visual Studio 已使用多年，坑全部踩过了一遍，突然变更又要重新踩坑
4. MinGW 已停止维护，正在维护的是 MinGW-w64，两者名称很像，选手尤其是使用百度等劣质搜索引擎或查阅 CSDN 等劣质博客安装时极易混淆
5. MinGW 需要附加更多的其自己的依赖库如 glibc 等，此 glibc 依然依赖 msvcrt，多一层依赖库便增加了一些版本不兼容问题的风险
6. 其与 MSVC 的 Name Mangling 的规则、ABI 等并不一致或不完全一致，增加各种不兼容问题的风险

## 开发人员

- ......（自己加）
