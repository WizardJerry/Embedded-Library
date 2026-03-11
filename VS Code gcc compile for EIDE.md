### 1. 前言

本文档构建了基于VSCode平台的嵌入式C项目的编译、下载调试环境，配合 VS Code 的插件可以使用一套完全免费的方案代替 Keil。

看到第4节为止

### 2. 准备

#### 2.1 软件

可以自己安装的：
1. 编辑器 Visual Studio Code 最新版

需要IT安装的
1. 下载调试工具 JLink_V810e （联系 IT 即可）

#### 2.2 硬件

1. JLink 下载器V9.2(需不低于软件支持版本)或以上
2. 可供测试的硬件开发板，例如 `MPF52350` 或 `N32A455` 开发板

#### 2.3 相关资料

1. 固件库参考国民技术的文档[《CN_AN_Universal_MCU_GCC_Development_V1.2.0》](https://www.nationstech.com/product/general/n32a/n32a455/)（文件在`技术资源/固件和软件/IDE环境包/GCC-N32A455`下）。

### 3. 开发环境搭建

#### 3.1 安装 JLink 工具

提交工单联系 IT 进行 Jlink 的安装（备注安装在 `D盘`，因为有内容需要手动修改，安装在 `C盘` 权限不够）

Jlink 驱动在 `[Jlink 安装位置]\SEGGER\JLink_V810e\USBDriver\InstDrivers.exe` 下，找 IT 安装完 Jlink 后可以请他们继续帮忙安装一个驱动

- 如果需要自己下载安装包，可以参考路径：https://www.segger.com/downloads/jlink/#-LinkSoftwareAndDocumentationPack

#### 3.2 添加芯片支持

由于没有原生的 JLink 支持，安装好 JLink 之后需要向 JLink 配置中添加国民技术的芯片补丁包，以便在下载、调试时正确获取到下载算法。

具体请参考文档`jlink工具添加Nations芯片流程 V1.3`，执行至2.1即可，2.2节可用于测试

- 下载地址：https://www.nationstech.com/product/general/n32a/n32a455/，在技术资源栏选择`固件和软件/IDE环境包/JLink`

#### 3.3 JLink 下载测试


- 测试JLink 环境安装

  1. 连接好 PC 和J-Link 调试器，连接好开发板，上电；

  2. 在开始菜单中应该有一个 `SEGGER - J-link Vxxx` 软件文件夹，打开其中的 `J-link Commander` 软件，或直接在 Windows 搜索栏中搜索软件名

     <img src=".\images\image-20251111150938930.png" alt="image-20251111150938930" style="zoom:50%;" />

  3. 如上图表示 PC 连接JLink 调试器成功。

  4. 然后根据提示依次输入：“connect”， “**N32A455VE**”， “SWD”， “4000”，如果前面的操作成功，则会看到下面的输出信息，JLink 下载调试环境就可以正常使用了。

     ![image-20251111151009125](.\images\image-20251111151009125.png)

#### 3.4 安装 VS Code 软件

- 下载软件：https://code.visualstudio.com/
- 为自己安装，作为一款编辑器，VS Code 不需要管理员权限安装，依旧建议安装在 `D盘`

<a id="part3.2"></a>

#### 3.5 使用 VS Code 插件来进行工具链配置

点开 VScode 左侧的 `Extensions` 栏，搜索并安装下列插件：
- Embedded IDE （之后简称 **EIDE**）
- Cortex-Debug
- C/C++ Extension Pack


### 4. 项目实战

完成了所有前置项目的安装后我们就可以使用 EIDE 进行项目的初始化了，以新建一个 MPF52350 项目为例

#### 4.1 拉取模板

- 打开 VS Code，点击安装好的 EIDE 插件，点击 `New Project`
   <img src=".\images\vscode-eide-1.png">
- 接下来在弹出的选项框中依次选择 `From Remote Repository` -> `MCU` -> `Nations (国民技术)` -> `M4` -> `N32A455 Demo`
- 输入你的项目名称
- VS Code会自动下载软件模板，选择一个位置放置你的项目，会在你选择的位置上创建一个 `[你的项目名]` 文件夹，里面包含模板的内容
- 此时 VS Code 会有弹窗，点击确认切换到对应的 `工作区/Workspace`，VS Code会打开你新创建的项目
- 此时再点开 EIDE 的插件，已经可以看到页面上已经显示了具体的项目文件和相关配置

#### 4.2 配置项目

由于我们下载的模板默认使用的是 `ARMCC(AC5/AC6)` 编译器，这其实是用于 Keil 的商业闭源编译器，我们需要使用的是开源免费的 `arm-none-eabi-gcc` 编译器，本示例中的 Nation 模板本身就提供了这两种编译器的切换选项。右键点击项目名称，使用 `Switch Target`，使用后缀为 GCC 的选项

<img src=".\images\vscode-eide-2.png">

修改完成后可以发现项目的名称变化了，并且内部的启动引导文件自动切换为了 gcc 的选项

<img src=".\images\vscode-eide-3.png">

切换到 ARMGCC 后，如果之前没有安装，右下角会弹出有建议安装的软件，可以直接在弹窗中确认安装 `arm-none-eabi-gcc` ，这是我们使用 VS code 进行嵌入式开发的主要编译器，点击 `Install Now` 之后 VS code 会自动完成编译器的安装，以及 EIDE 插件中的编译器路径配置

![alt text](.\images\armgcc-install.png)

检查是否安装成功：打开 VS Code 中的终端，点击下方的错误和警告标识可以打开右下方的栏目，在下方栏目中选择 `终端/Terminal`

在终端中输入`arm-none-eabi-gcc –v` 或 `arm-none-eabi-gcc –-version`

如下输出版本号等信息表示安装成功：

![armgcc](.\images\armgcc.png)


接下来可以尝试点击右上角的编译、烧录、调试选项来测试配置是否成功了

<img src=".\images\vscode-eide-4.png">


contact：
Rion.Lou@moniolithicpower.com
