# 第 1 讲（扩展版）：环境就绪 —— ROS 2、MoveIt 与 UR5 工作流（小白向）

> **课程**：EN.601.663 Assignment 3  
> **目标**：零基础也能跟上 —— 装依赖、理解「工作空间 + 编译 + 启动」、第一次把 UR5 + MoveIt + RViz 跑起来。  
> **MoveIt**：若你**第一次见** MoveIt，请先读 **第 2 节**（从框架层面讲清它管什么、不管什么，以及与作业的关系）；按钮级操作见 [第 2 讲](./lecture02_MoveIt界面与场景规划.md)。  
> **注意**：下文中的 Linux 命令与 PDF 一致；若你在 **Windows** 上学习，通常需 **WSL2 Ubuntu 22.04** 或学校提供的 **Linux 虚拟机 / 机房机** 来完成本作业。概念部分在任意系统都适用。

---

## 0. 读前须知：你会遇到什么？

- **ROS 2**：一套「机器人软件中间件」——很多独立程序（节点）通过话题、服务、动作协同工作。
- **MoveIt**：建立在 ROS 上的「运动规划框架」，负责机器人模型、场景障碍、碰撞检测、规划器接口等。**细节见本文第 2 节。**
- **UR5**：优傲（Universal Robots）六轴机械臂的常用型号；作业里用它的 **数字模型 + MoveIt 配置** 做仿真与规划。
- **RViz**：三维可视化工具；你看到的手臂、障碍、规划轨迹多半在这里显示。

**本讲结束时，你应该能**：说出「工作空间里哪一层该改、哪一层是生成的」，并成功运行一次 `ur_moveit.launch.py`（或等价启动方式），在 RViz 里看到 MoveIt 面板。

---

## 1. 把比喻说清楚：餐厅后厨

想象一家餐厅：

| 现实 | 对应到本作业 |
|------|----------------|
| 菜谱、食材、灶台规则 | **机器人模型（URDF/SRDF）**、关节限制、碰撞几何 |
| 服务员点单 | **规划请求（Planning Request）**：从当前姿态到目标姿态 |
| 厨师能不能把菜端过去不撞到人 | **碰撞检测**：这条路径会不会碰到桌子（障碍） |
| 传菜路线 | **轨迹 / 路径**：关节随时间怎么动 |

**MoveIt** 大致扮演「协调模型 + 场景 + 碰撞 + 规划器」的角色；**ROS 2** 负责把这些模块连起来、启动、通信。

你后面的作业是实现 **PRM 规划算法**；但 **碰撞检测、场景、机器人状态** 很多已由 MoveIt / 底层库完成。所以第 1 讲先保证 **整个 pipeline 能启动** —— 否则后面没法调试。

下面用一整节，把 **MoveIt** 从「第一次见」讲到 **够你理解作业里它在干什么**；细节操作（点哪个按钮）放在 [第 2 讲](./lecture02_MoveIt界面与场景规划.md)。

---

## 2. MoveIt 第一次见：它是什么、管什么、不管什么

### 2.1 一句话 + 官方定位

- **MoveIt**（现多指 **MoveIt 2**）是一个运行在 **ROS 2** 上的 **开源运动规划框架**（motion planning framework）。  
- 官网自我描述大意是：它把 **运动学、轨迹处理、碰撞检测、规划器接口、可视化** 等拼成一条可用的 **规划管线**，让你能回答：**「机械臂从当前姿态到目标姿态，有没有一条不碰障碍的轨迹？」**

你可以把它想成：**ROS 2 是「公路网」**，MoveIt 是路上专门服务机械臂的 **「导航 + 验车中心」** —— 车能不能过限高、绕不绕得开障碍物，它有一套标准流程可查。

### 2.2 MoveIt「管」的核心几件事（第一次见先记这五个词）

| 概念 | 小白理解 | 在本作业里你会在哪碰到 |
|------|-----------|-------------------------|
| **机器人模型（Robot Model）** | 手臂有几节、每节怎么转、限位多少 —— 来自 **URDF**（以及 MoveIt 用的 **SRDF** 等配置） | 关节限制 ±π、哪些链参与规划 |
| **机器人状态（Robot State）** | 某一时刻 **所有相关关节的角度** 组成一个向量，常叫 **构型（configuration）** | PRM 的每个顶点就是一个（无碰撞的）状态 |
| **规划场景（Planning Scene）** | 环境里 **障碍物的形状与位姿**（桌子、导入的 mesh、车等） | 你在 RViz 里加板子、`Publish Scene` 后规划器看到的障碍 |
| **规划组（Planning Group / Move Group）** | 「这次要动哪几条链」—— 例如只动 **手臂** 还是带 **夹爪** | Launch / SRDF 里配置；界面里要选对手臂的规划组 |
| **规划管线（Planning Pipeline）** | 从 **规划请求** 进来，到 **碰撞检测 + 规划器** 算完，再输出 **轨迹** 的一整条流水线 | 作业里 OMPL 被换成 **ASBR**，就是你的 PRM 接在这条管线上 |

**不需要第一次就背 API 名字**，但要建立直觉：**MoveIt 手里始终握着「模型 + 当前/目标状态 + 场景」这三样东西**，任何规划器想算路径，都得在这三样东西一致的前提下问碰撞、搜空间。

### 2.3 MoveIt「不管」或「不单独负责」的事（避免误解）

- **不替你写具体规划算法**：OMPL 里 RRT、PRM 等是 **规划器**；MoveIt 负责 **调用接口、传场景、收轨迹**。本作业要你 **自己实现 PRM**，正是接在「规划器」这一环。  
- **不默认替你做完视觉识别**：相机可以接进 ROS，但「物体在哪」通常要别的节点算好，再以障碍或目标的形式喂给 MoveIt。  
- **不替代真实机器人驱动**：仿真或真机控制还有 **控制器、驱动、ros2_control** 等；MoveIt 输出轨迹后，由别的栈去跟踪执行（本课多数时间在仿真与规划层面）。

知道边界后，你不会问出：**「MoveIt 为什么不会自动认识桌子」** —— 桌子要么来自仿真场景，要么来自感知/人工发布的 **Planning Scene**。

### 2.4 从「点 Plan」到内部：发生了什么？（抽象流水线）

下面用 **非代码** 顺序描述一次典型规划（与 RViz 里你将要做的操作一致）：

1. **当前状态**：从 joint state 或仿真得到 **Start**（起点构型）。  
2. **目标**：你指定末端位姿或关节目标，得到 **Goal**。  
3. **场景**：障碍在 **Planning Scene** 里；若界面改了没 **Publish**，内部仍用旧场景（见第 2 讲）。  
4. **规划请求**：Start + Goal + 场景 + 规划组名 + 规划器名 → 送给 **Planning Pipeline**。  
5. **碰撞检测**：沿候选路径或采样点时，用 **碰撞世界**（常见底层如 **FCL**）问：**这段几何会不会与障碍/自碰干涉**。  
6. **轨迹**：若成功，得到一串随时间或按路径索引的关节指令，可在 RViz 里动画预览或交给控制器。

**本作业的贡献**：第 5 步里「怎么在关节空间里搜出一条路径」由你的 **PRM** 完成；**单点是否碰撞**、**两点之间插值** 等，课程骨架往往通过 MoveIt 提供的 **`state_collides`、`interpolate`** 帮你接上碰撞检测。

### 2.5 URDF / SRDF 是什么？（只建立印象即可）

- **URDF**：描述连杆、关节、视觉/碰撞几何（mesh、简化为圆柱盒子等）。没有它，MoveIt 不知道手臂「长什么样」。  
- **SRDF**：MoveIt 额外配置，例如 **规划组** 包含哪些关节、哪些连杆之间默认 **禁用碰撞**（adjacent links 等），减少误报。

你第一次见不用会写这两个文件；**官方 `ur_moveit_config` 已配好 UR5e**。只要知道：**模型不对，规划一定怪**。

### 2.6 RViz 与 MoveIt：你眼睛看到的是哪一层？

- **RViz**：通用 3D 可视化，可显示机器人、点云、网格等。  
- **MoveIt 的 RViz 插件**（常叫 **MotionPlanning** 一类）：在 RViz 里多出 **Planning、Scene、Context** 等标签，把规划请求和场景编辑 **接到 MoveIt 节点**。

所以：**不是「MoveIt = RViz」**，而是 **MoveIt 在后台跑服务/节点，RViz 是前台控制台之一**。

### 2.7 和 ROS 2 的关系（为什么总一起装）

MoveIt 2 的节点用 ROS 2 的 **话题、服务、动作、参数** 与系统其它部分通信。你 `apt install ros-humble-moveit*`，装的是 **可被 launch 拉起的二进制与库**；你工作空间里的 **`assignment3`** 则是 **你自己的规划器插件源码**，编进 `install` 后也被 ROS 找到。

### 2.8 本讲启动命令里，MoveIt 在哪？

```bash
ros2 launch ur_moveit_config ur_moveit.launch.py ur_type:=ur5e
```

- 包 **`ur_moveit_config`** 里放的是 **针对 UR 的 MoveIt 配置 + launch**。  
- 这条命令会拉起 **多个节点**（含 MoveIt 相关与 RViz），并把 **UR5e 的模型与规划组** 设对。

你成功后看到的手臂 + 侧栏规划面板，就是 **「MoveIt 已接入这条 ROS 2 图」** 的证据。

### 2.9 第一次见 MoveIt 的自检（不要求会写代码）

能口头答出以下问题，说明本节有效：

1. **Planning Scene** 和 **Robot State** 差在哪？  
2. **规划组** 大概解决什么问题？  
3. **MoveIt** 与 **规划器（如 PRM）** 谁决定「怎么搜路」？谁提供碰撞与模型？  
4. 为什么作业仍要你学 MoveIt，而不是「纯手写一个 main」？

---

## 3. ROS 2 工作空间：src、build、install 是什么？

官方推荐的工作空间布局（名字可以不同，结构类似）：

```text
你的工作空间根目录/          ← 你经常在这里打开终端
├── src/                      ← 只改这里：源码与功能包（package）
├── build/                    ← colcon 生成：中间编译文件（可删）
├── install/                  ← colcon 生成：安装结果、环境脚本
└── log/                      ← 编译日志
```

**要点**：

1. **`src`**：你下载的作业包、自己改的代码，都在这里的子文件夹里（例如 `assignment3`）。
2. **`build` / `install`**：不要手改；出问题时可删掉后重新编译（见后文「常见问题」）。
3. 每次新开终端要 **source 环境**，否则系统找不到 ROS 和你编译好的包。典型做法（路径按你机器为准）：

```bash
source /opt/ros/humble/setup.bash
cd ~/你的工作空间
source install/setup.bash
```

> **小白记忆法**：`src` = 人类写的；`install` = 编译好的「给 ROS 用」的成品。

---

## 4. `colcon build` 是干什么的？

**colcon** = **COL**lective **CON**struction，ROS 2 里常用的编译工具。

- 它读每个包里的 **`package.xml`**（依赖声明）和 **`CMakeLists.txt`**（怎么编译），生成可执行文件与库到 `install/`。
- 作业要求：提交的包必须能 **`colcon build`** 通过，否则扣分。

**常用命令**（在**工作空间根目录**执行）：

```bash
# 只编译 assignment3 包（改作业时节省时间）
colcon build --packages-select assignment3

# 编译整个工作空间（第一次或依赖大变时）
colcon build
```

编译成功后，**记得** `source install/setup.bash`（在新终端里尤其重要）。

---

## 5. 本作业要装的软件（与 PDF 对齐）

PDF 建议在 **Ubuntu + ROS 2 Humble** 下安装（机房 Wyman 可能已预装）：

```bash
sudo apt-get -y update
sudo apt-get -y upgrade
sudo apt-get -y install ros-humble-moveit* ros-humble-ur-*
```

**这些包大概提供什么？**

- `ros-humble-moveit*`：MoveIt 2 相关组件（规划场景、插件、消息等）。
- `ros-humble-ur-*`：UR 机械臂在 ROS 2 里的驱动/描述/MoveIt 配置等（具体子包名随发行版略有不同，用通配符安装即可）。

**若安装报错**：

- 先确认 **ROS 2 Humble 已正确安装** 且 `source /opt/ros/humble/setup.bash` 无报错。
- 网络或镜像问题可换源或稍后重试；机房环境优先问课程 TA。

---

## 6. 获取作业骨架：脚本在做什么？

PDF 示例：

```bash
wget www.cs.jhu.edu/~sleonard/sbr-a3.sh
chmod 755 sbr-a3.sh
./sbr-a3.sh
```

**小白理解**：

- 脚本多半会：创建/更新工作空间、把 **`assignment3` 等功能包** 放进 `src`、可能拉取依赖说明。
- 你仍需在**工作空间根**执行 **`colcon build`**，把源码变成可运行组件。

> 若 `wget` 下载失败：检查网络、VPN，或从课程提供的其它链接/压缩包取得相同内容（以课程最新说明为准）。

---

## 7. 第一次启动：UR5 + MoveIt + RViz

PDF 中的启动命令（注意 `ur_type`）：

```bash
ros2 launch ur_moveit_config ur_moveit.launch.py ur_type:=ur5e
```

**逐段解释**：

| 部分 | 含义 |
|------|------|
| `ros2 launch` | ROS 2 的启动器：按「launch 文件」同时启动多个节点 |
| `ur_moveit_config` | 功能包名：里面放了 MoveIt 配置与 launch |
| `ur_moveit.launch.py` | 启动脚本：打开 RViz、加载机器人、加载 MoveIt |
| `ur_type:=ur5e` | **启动参数**：指定 UR 型号为 UR5e（与课程一致；写错可能加载错模型） |

**正常运行时，你通常会看到**：

- 弹出的 **RViz** 窗口里有机械臂模型；
- 一侧有 **MotionPlanning** / MoveIt 相关面板（不同版本界面略有差异）；
- 终端里可能有大量 INFO，但不应有持续无法忽略的致命错误。

结束运行时，在启动该 launch 的终端按 **`Ctrl+C`**。

---

## 8. Launch 文件到底启动了什么？（概念即可）

你不需要会写 launch 也能做作业，但理解这一点有助于排错：

- 一个 launch 文件会启动多个 **节点**（例如机器人状态发布、RViz、MoveIt 相关服务等）。
- **参数**（如 `ur_type:=ur5e`）会传给内部节点，用于选择正确的 URDF、关节名等。

**本作业后半段**你会启动课程包里的 launch，例如：

```bash
ros2 launch assignment3 ur5e_robotiq.launch.py
```

那时 **规划器** 会换成作业里的 **ASBR planner**，与第 1 讲里「官方 MoveIt demo」略有不同；但 **ROS 2 + 工作空间 + colcon** 的流程完全一样。

---

## 9. 本讲「动手清单」（建议按顺序打勾）

- [ ] 确认操作系统与环境：**Linux + ROS 2 Humble**（或学校等价环境）。
- [ ] 安装 `ros-humble-moveit*` 与 `ros-humble-ur-*`。
- [ ] 用 `sbr-a3.sh`（或课程提供方式）把作业包放入工作空间 `src`。
- [ ] 在工作空间根执行：`colcon build`（或先全量，以后可 `--packages-select assignment3`）。
- [ ] `source install/setup.bash`。
- [ ] 运行：`ros2 launch ur_moveit_config ur_moveit.launch.py ur_type:=ur5e`。
- [ ] RViz 中能看到 UR 机械臂与 MoveIt 相关 UI；用 `Ctrl+C` 正常退出。

---

## 10. 自检问题（能回答就算真懂）

1. **为什么改代码后必须重新 `colcon build`？**  
   *提示：源码在 `src`，运行用的是 `install` 里的产物。*

2. **为什么新终端里 `ros2` 命令找不到包？**  
   *提示：没 source ROS 与工作空间的 `setup.bash`。*

3. **`ur_type:=ur5e` 写错可能发生什么？**  
   *提示：模型/关节与 MoveIt 配置不一致，规划或显示异常。*

---

## 11. 常见问题（小白向排错）

### 11.1 编译失败：找不到依赖

- 看终端报错里的 **package 名**，用 apt 安装对应的 `ros-humble-xxx`。
- 检查 `package.xml` 里是否声明了依赖（作业骨架一般已写好；你若改过要一致）。

### 11.2 RViz 黑屏、闪退、OpenGL 报错

- 虚拟机里常与生透传 3D 有关；优先用 **物理机 Linux** 或学校机房。
- WSL2 图形取决于 **WSLg** 或第三方 X Server，配置不当会失败。

### 11.3 `ros2 launch` 提示找不到包

- 是否在该工作空间执行了 `source install/setup.bash`？
- 包名是否拼写正确？`ros2 pkg list | grep ur` 可粗查。

### 11.4 想「干净重编」

在工作空间根（**慎用，会删编译缓存**）：

```bash
rm -rf build install log
colcon build
source install/setup.bash
```

---

## 12. 与后续讲义的关系

- **[第 2 讲](./lecture02_MoveIt界面与场景规划.md)**：在能启动 MoveIt 的前提下，用 RViz **加障碍、发场景、点 Plan**（PDF §3）。
- **[第 3 讲](./lecture03_读懂骨架与PRM对应.md)**：阅读 `assignment3_context` 骨架；**§0 任务实现总览**（数据流表 + 推荐编码顺序）是 **写代码时的总目录**。
- **第 4～9 讲**：依次实现 **PRM、联调、专家挑战与提交**；**完整目录表**见 **[第 9 讲](./lecture09_专家挑战与提交.md)** 末尾「系列讲义索引」。
- **补充（非 Assignment 3 核心）**：课堂 **`week08.pdf`** 主题为 **卡尔曼滤波、定位与 SLAM**，与 **PRM 运动规划** 不同模块；已写 **[supplement_week08_Kalman滤波与定位SLAM.md](./supplement_week08_Kalman滤波与定位SLAM.md)** 供对照。
- **第 4 讲起**：实现 **PRM 代码**；若第 1 讲环境不稳，后面会大量浪费时间。

---

## 13. 参考链接（自学用）

- ROS 2 Humble 文档：<https://docs.ros.org/en/humble/>  
- MoveIt 2 文档（Humble）：<https://moveit.picknik.ai/humble/index.html>  

---

## 附录：Windows 用户怎么走？

本作业按 **ROS 2 + MoveIt** 标准流程，**官方主力支持 Linux**。常见做法：

1. **WSL2 + Ubuntu 22.04**，安装 ROS 2 Humble，配置图形界面显示 RViz；或  
2. 学校 **虚拟机 / 远程 Linux / 机房机**。

不建议在「纯 Windows 不装 ROS」的环境下硬做 —— 你会缺少 `colcon`、`ros2 launch` 等整套工具链。

---

*讲义版本：扩展第 1 讲（含 MoveIt 入门专节）· 与课程 PDF Assignment 3 结构对齐 · 仅供学习梳理使用。*
