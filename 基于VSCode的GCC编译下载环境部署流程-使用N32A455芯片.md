### 1. 前言

本文档构建了基于VSCode平台的嵌入式C项目的编译、下载调试环境，可完全不依赖Keil软件。

文档主要参考国民技术的文档[《CN_AN_Universal_MCU_GCC_Development_V1.2.0》](https://www.nationstech.com/product/general/n32a/n32a455/)（文件在`技术资源/固件和软件/IDE环境包/GCC-N32A455`下），并针对N32A455芯片的部署进行了补充和修正。



### 2. 开发工具

#### 2.1 软件

1. 编辑器Visual Studio Code 1.5x.x 或以上
2. 编译工具链 arm-none-eabi-gcc 6.3.1 或以上
3. Make for Windows
4. 下载调试工具 JLink_V6.40（需不高于硬件支持版本）或以上

#### 2.2 硬件

1. 开发板 N32A455VEL7-EVB
2. JLink 下载器V9.2(需不低于软件支持版本)或以上

### 3. 开发环境搭建

#### 3.1 安装VSCode 软件
- 下载软件：https://code.visualstudio.com/
  VSCode 用作代码查看和编辑，它还提供了powershell 和bash 终端用于命令行操作，我们的整个开发过程都要用到命令行终端。

<a id="part3.2"></a>

#### 3.2 安装gcc编译工具链

- 下载地址（示例版本：10-2020-q4-major）：
  - 线路1（较快）：https://developer.arm.com/downloads/-/gnu-rm
  - 线路2：https://launchpad.net/gcc-arm-embedded/+announcement/28093


下载后缀名为.exe的文件，完成之后安装**（需要管理员权限①）**

检查是否安装成功：打开终端，输入`arm-none-eabi-gcc –-version`，选择合适的路径安装（默认为`C:\Program Files (x86)\GNU Arm Embedded Toolchain\10-2020-q4-major\`）

如下表示安装成功：

![image-20251111143022485](.\images\image-20251111143022485.png)

若不成功

1. 检查环境变量是否添加好。添加路径：`gcc工具链安装路径\bin`**（需要管理员权限②）**
2. 进入安装目录“bin”目录下, 检查arm-none-eabi-gcc.exe 文件名是否正确

#### 3.3 安装Make for Windows

此工具用于解析Makefile脚本。

- 下载地址：http://www.equation.com/servlet/equation.cmd?fa=make，可选64位版本

下载得到make.exe，无需安装我们需要手动存放这个文件，将其放到合适的位置。例如：`D:\Applications\Make`。

将make的存放目录添加至环境变量**（需要管理员权限③）**

检查是否安装成功：打开终端，输入`make -v`,应如下：

![image-20251111145946367](.\images\image-20251111145946367.png)

#### 3.4 安装JLink工具

下载JLINK 安装包, V6.90a 或其他版本，安装**（需要管理员权限④）**

- 下载地址：https://www.segger.com/downloads/jlink/#-LinkSoftwareAndDocumentationPack

<img src=".\images\image-20251111150144509.png" alt="image-20251111150144509" style="zoom:33%;" />

#### 3.5 添加芯片支持
安装好 JLink 之后需要向JLink 中添加国民技术的芯片补丁包，以便在下载、调试时正确获取到下载算法。

具体请参考文档`jlink工具添加Nations芯片流程 V1.3`，执行至2.1即可，2.2节可用于测试



- 下载地址：https://www.nationstech.com/product/general/n32a/n32a455/，在技术资源栏选择`固件和软件/IDE环境包/JLink`

#### 3.6 JLink 下载测试

- 测试JLink 环境安装

  1. 连接好PC 和J-Link 调试器，连接好开发板，上电；

  2. 打开cmd.exe 命令行工具，进入JLink 安装目录（默认为`C:\Program Files (x86)\SEGGER\JLink_V640`）下，输入“JLink.exe”；

     <img src=".\images\image-20251111150938930.png" alt="image-20251111150938930" style="zoom:50%;" />

  3. 如上图表示 PC 连接JLink 调试器成功。

  4. 然后根据提示依次输入：“connect”， “**N32A455VE**”， “SWD”， “4000”，如果前面的操作成功，则会看到下面的输出信息，JLink 下载调试环境就可以正常使用了。

     ![image-20251111151009125](.\images\image-20251111151009125.png)

### 4. SDK目录

SDK 沿用已发的SDK 版本，当前使用v0.1.0，在此基础上做如下修改以适应GCC 开发环境。

#### 4.0 下载官方例程及固件

- 下载地址：https://www.nationstech.com/product/general/n32a/n32a455/

在`技术资源`中，选择`固件和软件/SDK包/N32A455系列软件开发包`，下载后解压，放到合适的位置。例程在`Nations.N32A455_Library.1.4.0\projects\n32a455_EVAL\applications\GCC_demo`文件夹中

#### 4.1 Makefile
在 SDK 包中的GCC_demo 例程目录下“GCC”文件夹：

![image-20251111151618810](.\images\image-20251111151618810.png)

其中“Makefile”文件是GCC 编译脚本文件。需要额外修改两行：

1. 166行，PATH_WINPC，按照本机JLink的实际安装位置填写
2. 172行，CHIP_TYPE，本项目改为N32A455VE

#### 4.2 .s文件

在 SDK 包中`Nsing.n32g455_Library.1.4.0\firmware\CMSIS\device\ startup`路径下有对应gcc编译器的.s 文件`startup_n32g033_gcc.s`。**无需另外修改**

![image-20251111151858442](.\images\image-20251111151858442.png)

#### 4.3 .ld 文件
在 SDK 包中`Nsing.n32g455_Library.1.4.0\firmware\CMSIS\ device`路径下有对应的.ld 文件`n32g033_flash.ld`。**无需另外修改**

![image-20251111151951886](.\images\image-20251111151951886.png)

#### 4.4 打印重映射
在 SDK 包的`bsp/src`目录下增加了`print_remap.c`文件用于串口打印重定向。无需另外修改

![image-20251111152037002](.\images\image-20251111152037002.png)

#### 4.5 J-Link 脚本
在 SDK 包主目录下增加了`jlink`文件夹，文件夹中有一个jlink 下载脚本，用于通过J-Link工具下载固件。无需另外修改

![image-20251111152041177](.\images\image-20251111152041177.png)

### 5. 编译和下载

#### 5.1 工作区

在下载的SDK文件夹（Nsing.N32A455_Library.1.4.0）的空白部分右键，在VSCode中打开，打开工作区。

> 部分Win11的电脑可能没有`在VSCode中打开`选项，此时可以选择【右键/在终端中打开/输入`code .`】，实现同样的效果

通常情况下，下载的SDK中会包括.vscode文件，此时可以直接进行下一步

如没有，则在VSCode中选择将打开的目录另存为工作区。此时在SDK 文件夹下会生成“.vscode”文件夹用于放置工作区配置文件。工作区的结构示例如下所示：

```
applications/
├── .git/
├── .vscode/
├── GCC/
│   └── Makefile
├── inc/
│   └── xxx.h	# 头文件
└── src/
    └── xxx.c	# 源文件
```



#### 5.2 工作目录

输入指令`cd Nsing.N32A455_Library.1.4.0\projects\n32a455_EVAL\applications\GCC_demo ` 进入到工程目录下。



#### 5.3 代码编译
在 VScode 编辑器的终端中，切换到输入指令：`cd .\GCC`“GCC”文件夹目录下，输入“make”开始编译

![image-20251111152702615](.\images\image-20251111152702615.png)

编译完成无错误会生成.elf、.bin 和.hex 文件。

![image-20251111152716215](.\images\image-20251111152716215.png)

若弹出错误：`'arm-none-eabi-gcc' 不是内部或外部命令，也不是可运行的程序
`,则说明arm的工具链没有装好，或未添加至环境变量中。此时请返回[3.2节](#part3.2)



#### 5.4 固件下载

> ⚠️注意：在下载时，**必须关闭JLinkDBGServer**，否则将会导致下载失败

1. 连接好PC→JLink→开发板

2. 在终端输入“make download”

   ![image-20251111153228080](.\images\image-20251111153228080.png)

3. 中间会输出一些信息…，最后下载完成

![image-20251111153236711](.\images\image-20251111153236711.png) 

4. 下载完成后会自动复位，系统开始运行
5. 如果下载不成功，请检查JLink 配置

#### 5.5 清除中间文件
在终端输入`make clean`可以清除编译生成的中间文件。



### 6 代码调试

注意：调试之前须下载一次（待确定）

#### 6.1 VSCode 设置
在SDK 工作路径下有一个“.vscode”文件夹，里面有“launch.json”工作区配置文件，进行代码调试需要配置`launch.json`：

![image-20251111172135710](.\images\image-20251111172135710.png)

这是vscode 调试器配置文件，下面几个地方要根据自己的项目路径来修改：

1. 指定gdb 调试器的路径：（绝对路径）
    ![image-20251114155110841](.\images\image-20251114155110841.png)

2. 指定调试代码output.elf 文件路径：(注意：路径不能太长)

  ![image-20251114155141121](.\images\image-20251114155141121.png)

#### 6.2 Makefile设置

打开例程中的“GCC/Makefile”文件：

![image-20251114160441344](.\images\image-20251114160441344.png)

1. 可以看到有一个debug 的启动配置，指向JLink 安装目录的JLinkGDBserver 服务程序。
2. 编译命令make 默认就是在调试模式，会带有一些调试信息。如果想要切换到发布版本，则编译代码时需要用如下命令：make release=y

#### 6.3 调试示例

以GCC_demo 工程作为示例，看看如何开始代码调试：

1. 在vscode 中打开SDK 工程，在终端中切换到GCC_demo/GCC 目录，输入make 编译代码

   ![image-20251114160710060](.\images\image-20251114160710060.png)

   ![image-20251114160716095](.\images\image-20251114160716095.png)

   生成的output.elf、output.bin、output.hex 文件在GCC/build 文件夹中。

2. 请参考6.1、6.2 章节配置好launch.json 文件中的路径。
3. 连接JLink 调试器到开发板，上电准备好。
4. 找到你的JLink 安装目录，双击运行JLinkGDBServer.exe 程序

![image-20251114160726383](.\images\image-20251114160726383.png)

5. 配置端口、协议、芯片型号等。其中芯片型号需根据实际项目确定，如`N32A455VE`；端口和协议可以参照下图，点击OK

   <img src=".\images\image-20251114160803857.png" alt="image-20251114160803857" style="zoom:33%;" />

6. 如下，表示JLink 调试器连接到芯片成功：

   ![image-20251114161029664](.\images\image-20251114161029664.png)

7. 在VSCode环境下，打开主函数所在代码文件，右上角点击调试，编译器选择gdb-arm，启动调试。此时可以看到下图标签变为了绿色，表示gdb 工具连接JLinkGDBserver 成功。

   <img src=".\images\image-20251114161510142.png" alt="image-20251114161510142" style="zoom:50%;" />

8. 此时VSCode自动切换到了调试窗口，调试窗口上方的调试按钮包括单步、连续执行、重启、停止等。现在就可以执行单步调试、全速运行等功能了

### 7 配置修改

#### 7.1 芯片型号
如果使用的芯片不是N32A455 系列，则需要修改makefile 文件中的变量`TARGET_PLATFORM`和`DEFS`

<img src=".\images\image-20251114162210555.png" alt="image-20251114162210555" style="zoom:50%;" />

不同的芯片型号需要根据数据手册更改.ld 文件中的Flash&SRAM 容量，栈顶的位置及堆栈大小。

<img src=".\images\image-20251114162333975.png" alt="image-20251114162333975" style="zoom:50%;" />

#### 7.2 固件下载算法
需要输入完整的芯片型号，以便JLink 可以正确匹配下载算法。

配置下载工具路径：根据你的安装目录来配置。注意路径的最后要加`/`，末尾不能有空格

<img src=".\images\image-20251114162439008.png" alt="image-20251114162439008" style="zoom:50%;" />

#### 7.3 使用SDK 算法库
默认不使用算法库，使用算法库请修改变量“USELIB = 1”

#### 7.4 调试配置

默认的“make”编译是带有“-g”调试信息的，如果要编译release 版本，请使用“make release=y”。
#### 7.5 优化等级

默认使用“-O0”优化等级，不做代码优化。



### 8 总结

#### 8.1 代码编译下载流程

在环境配置好后，项目的编译下载流程如下所示

1. 关闭JLinkDBGServer
2. 用VSCode打开项目文件夹，进入GCC文件夹`cd GCC`，输入`make`，实现编译
3. 编译成功后，输入`make download`，实现下载

#### 8.2 代码调试流程

1. 打开JLinkDBGServer
2. 用VSCode打开项目文件夹，进入GCC文件夹`cd GCC`，输入`make`，实现编译
3. 编译成功后，点击调试



### 9 版本历史

|    日期    | 版本 |   修改   |
| :--------: | :--: | :------: |
| 2025/11/14 | V1.0 | 初始版本 |

contact：

