# Assignment 1 第三部分解析：SBR 包安装与 GUI 动画

**章节**: 3. Installation of the SBR package  
**重要性**: ⭐⭐⭐⭐⭐ 这是实际开始使用机器人仿真的第一步

---

## 📋 概述

第三部分教你如何：
1. 安装 SBR（Systems for Robotics）包
2. 启动机器人仿真
3. 使用 GUI 控制机器人
4. 理解 ROS 节点和话题
5. 使用 TF 查看坐标系变换

---

## 📦 3.0 SBR 包安装

### 安装步骤

**下载并运行安装脚本**：

```bash
$ wget www.cs.jhu.edu/~sleonard/sbr-a1.sh
$ chmod 755 sbr-a1.sh
$ ./sbr-a1.sh
```

### 脚本功能

这个脚本会：
- ✅ 创建 ROS 工作空间（workspace）
- ✅ 下载必要的依赖包
- ✅ 编译所有代码

**工作空间位置**：`~/sbr_workspace`

---

## 🎮 3.1 GUI 动画控制机器人

### 启动仿真

**步骤1：打开终端**
- 快捷键：`Ctrl + Alt + T`

**步骤2：进入工作空间并启动仿真**

```bash
$ cd ~/sbr_workspace
$ source install/setup.bash
$ ros2 launch assignment1 assignment1.launch.py
```

### 关键命令解释

| 命令 | 作用 |
|------|------|
| `cd ~/sbr_workspace` | 进入工作空间目录 |
| `source install/setup.bash` | **必须执行**：设置 ROS 环境变量 |
| `ros2 launch ...` | 启动 ROS2 启动文件 |

**⚠️ 重要**：每次打开新终端都需要执行 `source install/setup.bash`！

### 启动后的界面

启动后会看到**两个窗口**（如图2所示）：

#### 窗口1：RViz（左侧大窗口）

**功能**：3D 可视化机器人

**界面元素**：
- **左侧面板（Displays）**：
  - Global Options
  - Grid（网格）
  - RobotModel（机器人模型，已勾选）
  - **TF（变换，已勾选）** - 用于查看坐标系

- **中央3D视图**：
  - 显示机器人模型
  - 可以看到各个 link（连杆）：
    - `base_link`：基座
    - `upper_arm_link`：上臂
    - `forearm_link`：前臂
    - `wrist_1_joint`, `wrist_2_joint`, `wrist_3_joint`：腕关节
    - `gripper_pick`：末端执行器（夹爪）

#### 窗口2：rqt_joint_trajectory_controller（右侧小窗口）

**功能**：关节轨迹控制器 GUI

**界面元素**：
1. **下拉菜单**：选择控制器
   - 默认：`ur5e_controller/joint_trajectory`
   - **必须选择正确的控制器**

2. **Enabled 按钮**（中间）
   - 启用/禁用控制器
   - **必须启用才能控制机器人**

3. **6个关节滑块**：
   - `shoulder_pan_joint`：肩部旋转关节
   - `shoulder_lift_joint`：肩部抬升关节
   - `elbow_joint`：肘关节
   - `wrist_1_joint`：腕关节1
   - `wrist_2_joint`：腕关节2
   - `wrist_3_joint`：腕关节3

4. **数值输入框**：显示当前关节角度（弧度）

5. **按钮**：
   - `Send`：发送命令
   - `Stop`：停止

### 使用建议

**推荐的初始配置**（让机器人直立）：
- 使用图中显示的数值
- 通过滑块调整关节角度
- 观察 RViz 中机器人的变化

**熟悉 RViz**：
- 展开左侧 "Displays" 面板的下拉菜单
- 特别关注 **TF** 元素
- 可以监控每个坐标系的位置和方向

**为什么重要？**
- 可以**验证你的计算结果是否正确**
- 如果计算错误，TF 会显示错误的位置
- **技术上没有借口出错**（因为可以可视化验证）

---

## 🔍 3.2 rqt_joint_trajectory_controller 详解

### 节点功能

**这个节点做什么？**
- 与右侧小窗口（GUI）关联
- **发布关节位置命令**
- 每次移动滑块，就发布新的关节位置命令给机器人

### 查看节点信息

**命令**：
```bash
$ ros2 node info /rqt_joint_trajectory_controller
```

**输出示例**：
```
Publishers:
/joint_trajectory_controller/joint_trajectory:
  trajectory_msgs/msg/JointTrajectory
```

**解释**：
- **发布者（Publisher）**：这个节点发布消息
- **话题（Topic）**：`/joint_trajectory_controller/joint_trajectory`
- **消息类型**：`trajectory_msgs/msg/JointTrajectory`

### 查看实际发布的消息

**命令**：
```bash
$ ros2 topic echo /ur5e_controller/joint_trajectory
```

**输出示例**：
```yaml
joint_names:
- shoulder_pan_joint
- shoulder_lift_joint
- elbow_joint
- wrist_1_joint
- wrist_2_joint
- wrist_3_joint
points:
- positions:
  - -0.0
  - -1.57
  - 0.0
  - -1.57
  - -0.0
  - -0.0
  velocities: []
  accelerations: []
  effort: []
  time_from_start:
    sec: 1
    nanosec: 0
```

**消息内容解释**：

| 字段 | 说明 |
|------|------|
| `joint_names` | 6个关节的名称列表 |
| `positions` | 每个关节的目标位置（**弧度**） |
| `velocities` | 关节速度（通常为空） |
| `accelerations` | 关节加速度（通常为空） |
| `effort` | 关节力矩（通常为空） |
| `time_from_start` | 从开始的时间 |

**重要提示**：
- 移动滑块时会实时打印当前关节位置命令
- 数值单位是**弧度（radian）**
- 例如：`-1.57` ≈ `-90°`

---

## 📊 3.3 robot_state_publisher 详解

### 节点功能

**这个节点做什么？**
1. **订阅** `/joint_states` 话题
   - 接收**测量到的**（measured）关节角度
   - 与命令角度不同，这是实际角度

2. **计算前向运动学**
   - 对机器人的每个 link（连杆）计算前向运动学
   - 确定每个坐标系的位置和姿态

3. **发布变换**
   - 将所有坐标系和3D几何体变换到当前位置和方向
   - 这样可以在 RViz 中正确显示

### 查看节点信息

**命令**：
```bash
$ ros2 node info /robot_state_publisher
```

**输出（部分）**：

**订阅者（Subscribers）**：
- `/clock: rosgraph_msgs/msg/Clock` - 仿真时钟
- `/joint_states: sensor_msgs/msg/JointState` - **关节状态（重要）**
- `/parameter_events: rcl_interfaces/msg/ParameterEvent` - 参数事件

**发布者（Publishers）**：
- `/parameter_events: rcl_interfaces/msg/ParameterEvent`
- `/robot_description: std_msgs/msg/String` - 机器人描述（URDF）
- `/rosout: rcl_interfaces/msg/Log` - 日志
- `/tf: tf2_msgs/msg/TFMessage` - **变换消息（重要）**
- `/tf_static: tf2_msgs/msg/TFMessage` - 静态变换

### 工作流程

```
仿真器发布关节状态
    ↓
/joint_states 话题
    ↓
robot_state_publisher 订阅
    ↓
计算前向运动学
    ↓
发布到 /tf 话题
    ↓
RViz 订阅并显示
```

**关键点**：
- `robot_state_publisher` 订阅**测量值**（不是命令值）
- 计算前向运动学
- 发布变换到 `/tf` 话题

---

## 🔧 3.4 使用 tf2_echo 查询变换

### 什么是 TF？

**TF（Transform）**：坐标系之间的变换关系

**用途**：
- 查找机器人某部分相对于参考坐标系的位置和方向
- 验证前向运动学计算结果

### 基本命令

**查找 gripper_pick 相对于 world 的变换**：

```bash
$ ros2 run tf2_ros tf2_echo world gripper_pick
```

**命令格式**：
```bash
ros2 run tf2_ros tf2_echo <父坐标系> <子坐标系>
```

### 输出示例

**Translation（平移）**：
```
[0.001, 0.331, 1.001]
```
- X, Y, Z 坐标（米）
- 表示 `gripper_pick` 在 `world` 坐标系中的位置

**Rotation in Quaternion（四元数）**：
```
[-0.707, 0.001, 0.001, 0.707]
```
- [x, y, z, w] 格式
- 表示 `gripper_pick` 相对于 `world` 的旋转

**Rotation in RPY（弧度）**：
```
[-1.571, 0.002, -0.000]
```
- Roll, Pitch, Yaw（滚转、俯仰、偏航）
- 单位：弧度

**Rotation in RPY（度）**：
```
[-90.000, 0.091, -0.000]
```
- 转换为角度制
- 例如：-90° 表示绕某个轴旋转 -90 度

**Matrix（齐次变换矩阵）**：
```
1.000 -0.002  0.000  0.001
-0.000 -0.000  1.000  0.331
-0.002 -1.000 -0.000  1.001
0.000  0.000  0.000  1.000
```
- 4×4 齐次变换矩阵
- 包含旋转（左上3×3）和平移（右上3×1）
- 这是**前向运动学的输出**！

### 另一个例子

**查找 forearm_link 相对于 upper_arm_link 的变换**：

```bash
$ ros2 run tf2_ros tf2_echo upper_arm_link forearm_link
```

**用途**：
- 查看相邻 link 之间的变换
- 验证单个关节的变换是否正确

---

## 🎨 3.5 使用 RViz 可视化

### TF 显示

**在 RViz 中查看 TF**：

1. 在左侧 "Displays" 面板中
2. 找到 "TF" 选项（应该已勾选）
3. 展开 "TF" 下拉菜单

**可以看到**：
- 每个坐标系的**坐标轴**（X=红色，Y=绿色，Z=蓝色）
- 坐标系之间的**连接线**
- 坐标系名称标签

### RViz vs tf2_echo

| 特性 | RViz | tf2_echo |
|------|------|----------|
| **可视化** | ✅ 3D图形显示 | ❌ 文本输出 |
| **精度** | ✅ 更高精度 | ⚠️ 文本精度有限 |
| **实时性** | ✅ 实时更新 | ✅ 实时更新 |
| **调试** | ✅ 直观 | ⚠️ 需要理解数值 |

**建议**：
- **调试时**：使用 RViz 可视化
- **验证数值**：使用 tf2_echo 获取精确值
- **两者结合**：最佳调试方式

### 为什么重要？

**TF 信息对调试非常有用**：

1. **验证前向运动学**
   - 如果计算错误，TF 会显示错误的位置
   - 可以立即看到问题

2. **验证雅可比矩阵**
   - 观察速度变换是否正确
   - 检查奇异性

3. **验证轨迹规划**
   - 检查末端执行器是否按预期移动
   - 验证路径是否正确

**关键点**：
- **技术上没有借口出错**
- 因为可以可视化验证所有计算
- 如果 TF 显示正确，计算就正确

---

## 📝 3.6 查看所有 ROS 节点

### 列出所有节点

**命令**：
```bash
$ ros2 node list
```

**输出示例**（16个节点）：
```
/controller_manager
/get_controller_managers_node
/gz_ros2_control
/robot_state_publisher
/robotiq_gripper_controller
/robotiq_joint_state_broadcaster
/ros_gz_bridge
/rqt_gui_py_node_80127
/rqt_joint_trajectory_controller
/rviz2
/static_transform_publisher_RTBx2f4mxP6U7iLi
/trajectory
/transform_listener_impl_63d338b1d250
/ur5e_controller
/ur_joint_state_broadcaster
```

### 重点节点

**本作业重点关注**：

1. **`/rqt_joint_trajectory_controller`**
   - GUI 控制节点
   - 发布关节位置命令

2. **`/robot_state_publisher`**
   - 计算并发布 TF 变换
   - 关键的前向运动学节点

3. **`/ur5e_controller`**
   - UR5 机器人控制器
   - 接收关节命令并执行

4. **`/trajectory`**
   - 轨迹生成节点（**这是你要实现的**）
   - 后续会详细讲解

---

## ✅ 实践检查清单

完成本节后，你应该能够：

### 基本操作
- [ ] 成功启动机器人仿真
- [ ] 看到 RViz 和控制器 GUI 两个窗口
- [ ] 使用滑块移动机器人
- [ ] 在 RViz 中看到机器人移动

### ROS 理解
- [ ] 理解节点的概念
- [ ] 理解话题的概念
- [ ] 能够列出所有节点
- [ ] 能够查看节点信息
- [ ] 能够查看话题内容

### TF 使用
- [ ] 理解 TF 的作用
- [ ] 能够使用 `tf2_echo` 查询变换
- [ ] 能够在 RViz 中查看 TF 坐标系
- [ ] 理解齐次变换矩阵的含义

### 调试能力
- [ ] 知道如何验证前向运动学
- [ ] 知道如何查看坐标系变换
- [ ] 理解可视化调试的重要性

---

## 🎯 关键概念总结

### ROS 核心概念

1. **Node（节点）**
   - 执行特定功能的程序
   - 例如：`robot_state_publisher` 计算前向运动学

2. **Topic（话题）**
   - 节点间通信的通道
   - 例如：`/joint_states` 传递关节角度

3. **Publisher/Subscriber（发布者/订阅者）**
   - 节点通过话题通信
   - Publisher 发布消息
   - Subscriber 订阅消息

4. **TF（Transform）**
   - 坐标系变换系统
   - 管理所有坐标系之间的关系
   - 用于前向运动学可视化

### 前向运动学流程

```
关节角度（测量值）
    ↓
/joint_states 话题
    ↓
robot_state_publisher 节点
    ↓
计算前向运动学
    ↓
发布到 /tf 话题
    ↓
RViz 可视化显示
```

---

## 🚀 下一步

完成本节后：

1. **熟悉操作**：多练习使用 GUI 和 RViz
2. **理解概念**：确保理解 ROS 节点、话题、TF
3. **准备实现**：开始理解前向运动学的数学原理
4. **继续阅读**：第4-5节（运动学推导）

---

## 💡 学习建议

### 对于初学者

1. **多动手**
   - 不要只看文档，要实际操作
   - 尝试不同的关节角度组合
   - 观察机器人的运动

2. **理解数据流**
   - 追踪数据从 GUI → 话题 → 节点 → TF → RViz
   - 理解每个环节的作用

3. **使用调试工具**
   - 熟练使用 `ros2 topic echo`
   - 熟练使用 `tf2_echo`
   - 熟练使用 RViz 的 TF 显示

### 常见问题

**Q: 为什么看不到机器人？**
- A: 检查 RViz 中 "RobotModel" 是否勾选
- 检查 "Fixed Frame" 是否设置为 "world"

**Q: 滑块移动但机器人不动？**
- A: 检查控制器是否启用（Enabled 按钮）
- 检查是否选择了正确的控制器

**Q: tf2_echo 没有输出？**
- A: 确保仿真正在运行
- 检查坐标系名称是否正确（区分大小写）

---

## 📖 相关命令速查

### 基本命令

```bash
# 列出所有节点
ros2 node list

# 查看节点信息
ros2 node info <节点名>

# 列出所有话题
ros2 topic list

# 查看话题内容
ros2 topic echo <话题名>

# 查询TF变换
ros2 run tf2_ros tf2_echo <父坐标系> <子坐标系>
```

### 工作空间命令

```bash
# 进入工作空间
cd ~/sbr_workspace

# 设置环境（每次新终端都需要）
source install/setup.bash

# 启动仿真
ros2 launch assignment1 assignment1.launch.py
```

---

**解析完成！** 现在你应该能够启动仿真、控制机器人，并使用 TF 进行调试了。

---

*解析完成时间：2024年*  
*如有疑问，请参考原始PDF文档或联系课程助教*

