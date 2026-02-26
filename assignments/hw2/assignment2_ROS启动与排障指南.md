# Assignment2 ROS 仿真启动与排障指南

**适用**：663 版本，需在 ROS 环境下采集数据、验证手眼标定  
**常见问题**：launch 后左侧 "No Image"、相机画面不显示

---

## 一、首次环境搭建（有 ROS 的电脑）

### 1. 安装课程 ROS 包

```bash
cd ~
wget www.cs.jhu.edu/~sleonard/sbr-a2.sh
chmod 755 sbr-a2.sh
./sbr-a2.sh
```

脚本会创建 ROS 工作空间并编译 UR5、aruco_ros 等依赖。

### 2. 克隆 ASBR 仓库（含你的 Matlab 代码）

```bash
cd ~
git clone https://github.com/18244241528jm-cpu/ASBR.git
```

### 3. 把 Matlab 代码放到 assignment2

```bash
# 假设 sbr-a2.sh 创建了 ~/sbr_workspace
SBR_SRC=~/sbr_workspace/src/sbr
# 若路径不同，按实际修改

# 复制 matlab 文件到 assignment2
cp ~/ASBR/assignment2/matlab/*.m $SBR_SRC/assignment2/matlab/
# 若 ASBR 里是 assignments/hw2/matlab/，则：
# cp ~/ASBR/assignments/hw2/matlab/*.m $SBR_SRC/assignment2/matlab/
```

### 4. 启动仿真

```bash
cd ~/sbr_workspace
source install/setup.bash
ros2 launch assignment2 assignment2.launch.py
```

---

## 二、启动后应看到什么

| 区域 | 正常情况 |
|------|----------|
| **左侧** | 摄像头实时画面，能看到 AR tag 时会有 tag 名称和坐标系 |
| **右侧** | 3D 视图：机械臂、camera_link、marker 等坐标系 |
| **机械臂** | 可通过 GUI 拖动到不同位姿 |

---

## 三、左侧 "No Image" 白屏的排查

### 步骤 1：确认节点和话题

```bash
# 新开终端，source 后执行
source ~/sbr_workspace/install/setup.bash

# 查看节点
ros2 node list

# 查看话题（找 camera / image 相关）
ros2 topic list
```

预期应有类似 `/camera/image_raw` 或 `/camera/camera_info` 等话题。

### 步骤 2：检查 Rviz Image 插件配置

1. 在 Rviz 左侧 Displays 面板找到 **Image** 或 **Camera** 显示
2. 展开，检查 **Topic** 是否与真实话题一致
3. 若话题名不对，改为实际发布的 topic（如 `/camera/image_raw`）

### 步骤 3：手动查看图像

```bash
ros2 topic echo /camera/image_raw --once
```

若有输出，说明图像在发布；若没有，说明相机节点未正常启动。

### 步骤 4：移动机械臂

- 有时需要把末端移到能看到地上 AR tag 的位姿，相机画面才会稳定
- 用 GUI 拖动机械臂，使摄像头对准地面标定板

### 步骤 5：重新编译与 launch

```bash
cd ~/sbr_workspace
colcon build --packages-select assignment2
source install/setup.bash
ros2 launch assignment2 assignment2.launch.py
```

---

## 四、数据采集流程（663）

### 1. 每个位姿的操作

1. 移动机械臂，确保 AR tag 在画面中
2. 终端 1：
   ```bash
   ros2 run tf2_ros tf2_echo world gripper_pick
   ```
   复制 translation 和 rotation (qx qy qz qw)
3. 终端 2：
   ```bash
   ros2 run tf2_ros tf2_echo camera_link marker
   ```
   复制 translation 和 rotation
4. 按格式写入 `data.txt`（见 Day4 指导）

### 2. data.txt 格式

```
# gripper
tx1 ty1 tz1 qx1 qy1 qz1 qw1
...
# marker
tx1 ty1 tz1 qx1 qy1 qz1 qw1
...
```

---

## 五、验证标定结果

用 Matlab 跑 `axxb(e_bh, e_sc)` 得到 X 后，在 Rviz 中发布 result 帧：

```bash
ros2 run tf2_ros static_transform_publisher --x TX --y TY --z TZ --qx QX --qy QY --qz QZ --qw QW --frame-id gripper_pick --child-frame-id result
```

将 TX TY TZ QX QY QZ QW 替换为 X 的平移和四元数。`result` 应与 `camera_link` 接近。

---

## 六、常见错误速查

| 现象 | 可能原因 | 处理 |
|------|----------|------|
| 左侧 No Image | 相机话题未发布或 Rviz 订阅错误 | 检查 `ros2 topic list`，修正 Rviz Image 的 Topic |
| 完全 launch 失败 | 未 source、路径错误 | `source install/setup.bash`，确认在 sbr_workspace 下 |
| 找不到 assignment2 | 未运行 sbr-a2.sh 或路径不对 | 重新运行 sbr-a2.sh，确认 assignment2 在 src 下 |
| AR tag 不显示 | 机械臂位姿不对、光线不好 | 调整末端位姿，使摄像头对准地面标定板 |
