# WarfarmeShawzinProduce
This is an independent plugin that helps play the Shawzin in a way easier than direct playing it. Created on 20250930.
## `Warframe` 三弦琴音乐制作
## `Warframe` ShawZin Music Script Produce
20250928 Edit

#### 需要准备的项目
#### Preparations
`python 3.6` 及以上版本，检查 `python -m tkinter` 弹出小窗口即可
`python 3.6` or newer version, use command `python -m tkinter` to check if able by noticing a window pop up.

`g++` 编译环境，可选用 `MinGW64`
Compile environment for `g++`: can work with `MinGW64` set.

`Interception`框架，关注[GitHub - oblitum/Interception: The Interception API aims to build a portable programming interface that allows one to intercept and control a range of input devices.](https://github.com/oblitum/Interception)获取技术支持与最新版的Interception框架，并将以下文件置于 `Release` 文件夹（或对应目录，只要你知道你在做什么）下：
Please follow `Interception` frame on [GitHub - oblitum/Interception: The Interception API aims to build a portable programming interface that allows one to intercept and control a range of input devices.](https://github.com/oblitum/Interception) to get technical support and place the following file in the `Release` folder (or else, as long as you know what you're doing):

1. `interception.h`（已有，可更新版本）
2. `interception.lib`（已有，可更新版本）

#### 产品框架
#### Production Frame

**本品包含：**
**This includes:**

`consumer.py`：音乐制作器，编译**（同 `consumer_helper2.cpp` 一并）**并打开 `Warframe 肖申克作曲机`
`consumer.py`: music script producer, compile `consumer_helper2.cpp` and this to open the `Warframe Shawshank Songmaker`

`replay.exe`（源码`replay.cpp`） 若希望从源码编译，编译时 `cmd` 进到 `Release` 目录下，Windows用户请用
`replay.exe`(compile from `replay.cpp`). If you want to compile from the original code, for user on Windows system, please type

```bash
g++ -std=c++11 replay.cpp interception.lib -o replay.exe
```

`Tools.h` 等：必要的头文件，编译 `replay.cpp` 时将之置于同一目录下（或对应目录，只要你知道你在做什么）
`Tools.h` and other header file: necessities, put them at the same menu with `replay.cpp`(or else, as long as you know what you are doing).
附带：提供某些示例键位信息，每行对应一条记录，每条记录格式为：

```
<delay/ms> <command>
```

**示例文件：**
**Sample File:**

```
Aurora.txt                    -   Aurora (By K - 391,RORY ) 节选
DaoMuBiJiShiNianRenJian.txt   -   盗墓笔记·十年人间 (By 李常超（Lao乾妈）) 节选
```

#### 演奏须知
#### Notice when playing

请将三弦琴（`Shawzin`）调至 `大调` 模式，并选用合适的三弦琴演奏对应乐曲。
Choose the right type of Shawzin and put Shawzin to `Major` mode to play

#### 笔者消息
#### Note from author

因 `大调` 模式囊括 `C3` 至 `G4` 所有音，便于编曲，目前所有组件均基于 `大调` 模式开发，后续可能关注其它模式，但工作将仍主要围绕 `大调` 模式进行。
Due to the integrity of the `Major` mode from C3 to G4, all components are based on `Major`. I'll focus on other mode if possible, but mainly process around `Major` mode.
