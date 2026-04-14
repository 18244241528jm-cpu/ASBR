# HW4（Assignment 4）Day 1 详细计划：EKF 基础 + 环境 + 过程模型

**对应作业**：JHU ASBR Assignment 4 - Jackal 6D EKF（BFL），截止日见课程 Canvas / PDF。  
**Day 1 目标**：把 **预测步** 所需的 **\(f\)** 与 **\(A=\partial f/\partial s\)** 在代码里落地初版；仿真能跑通并完成一次遥控初始化；为 Day 2 观测模型留出清晰接口理解。  
**预计总时长**：约 **7-9 小时**（可分上/下午两段；环境若已装好可省 1-2 h）。

---

## 0. 开始前：确认状态与路径

- [ ] PDF 已读：`assignment4-663.pdf` 中 **第 4 节** 状态定义、\(R=R_X R_Y R_Z\)、\(u=[v,\omega]^T\) 含义、只改 `ekf_models.*`。
- [ ] 本机：**ROS 2 Humble** + 合理 CPU（作业说明：**单核虚拟机不推荐**）。
- [ ] 工作区约定：作业源码一般在 `~/sbr_workspace/src/sbr/assignment4/`（以你实际 `sbr-a4.sh` 解压路径为准）。

---

## 上午块 A：理论复习（约 2-2.5 h）

### A1. EKF 流程（必须能口述）

用 5 句话自检（写进笔记即可）：

- [ ] **预测**：用 \(f\) 把 \(s_t\) 推到 \(\hat s_{t+1}\)，协方差用 \(A\) 与过程噪声传播。
- [ ] **校正**：用 \(h(s)\) 预测观测量，用 \(H\) 做线性化，融合 GPS/IMU。
- [ ] **为何需要 Jacobian**：非线性时一阶展开；**错 Jacobian 的典型症状**是滤波很快发散或估计"粘住"不动。

### A2. 本作业的状态与控制（与 PDF 对齐）

| 符号 | 含义 |
|------|------|
| \(s=[x,y,z,r,p,\psi]^T\) | 世界系位置 \((x,y,z)\) + **RPY**；\(\psi\) 为 **yaw（航向角）**，与位置分量 **\(y\)** 区分；作业 PDF 里航向角常写作 \(y\) |
| \(u=[v,\omega]^T\) | \(v\)：沿 **Jackal 车体 X** 的线速度；\(\omega\)：绕 **车体 Z** 的角速度 |
| 旋转约定 | **\(R = R_X(r) R_Y(p) R_Z(\psi)\)**：由 RPY 得到 **body 到 world** 还是 **world 到 body** 要以代码/BFL 骨架为准，**推导 Jacobian 时全程同一约定** |

**自测**：

- [ ] 画一张小图：世界系、车体系、\(v\) 沿哪根轴、\(\omega\) 绕哪根轴。
- [ ] 明确：仿真里 IMU 是四元数，滤波状态是 RPY。**Day 2** 再实现 `h_imu`，今天只需知道"别在过程模型里混用四元数状态"。

### A3. 过程模型 \(s_{t+1}=f(s_t,u_t)\) 的推导策略（今天核心）

1. **连续时间直觉**：车体速度在 **车体系** 为 \([v,0,0]^T\)，角速度为 \([0,0,\omega]^T\)（与作业一致）。  
2. **转到世界系**：线速度在世界系为 \(R \begin{bmatrix}v\\0\\0\end{bmatrix}\)（\(R\) 与 A2 约定一致）。  
3. **姿态更新**：由 **body 角速度** 与 **欧拉角速率** 的关系，写出 \(\dot r,\dot p,\dot \psi\) 与 \(\omega\) 的式子（依赖 \(R_X R_Y R_Z\) 顺序）。  
4. **离散化**：作业框架通常用 **固定 \(\Delta t\)**（在代码里查实际变量名），用 **欧拉积分**：  
   \(s_{t+1} \approx s_t + \Delta t \cdot \dot s\)。

**工具**：PDF 允许用 Matlab / Mathematica 符号求导。**建议今天下午就用符号算 \(A=\partial f/\partial s\)**，再抄进 `sys_evaluate_A`，减少手写抄错。

**今天不要求完美**：\(f\) 与 \(A\) 先 **自洽**（同一套 \(R\) 与角速度映射），Day 3 再与噪声一起细调。

### A4. 代码地图（打开作业包后 15 分钟）

- [ ] 打开 `ekf_models.hpp` / `ekf_models.cpp`，**高亮**今天要实现的函数：  
  `sys_evaluate_f`、`sys_evaluate_A`、`sys_evaluate_WMWt`（WMWt 今天可先 **占位对角阵**，明天再认真调）。
- [ ] 确认：状态向量 **索引顺序** 与 Jacobian 维度 **6×6** 与骨架一致（**不要假设**是 \([x,y,z,\phi,\theta,\psi]\) 别的顺序）。

---

## 上午块 B：环境安装与仿真跑通（约 1.5-2.5 h）

> 若你在 **Wyman 工作站** 且已预装依赖，可跳过 apt 部分，只验证 launch。

### B1. 系统依赖（按需执行）

```bash
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install ros-humble-clearpath*
```

### B2. 拉取作业脚本并安装工作空间

按 PDF 提供的步骤（示例）：

```bash
wget https://www.cs.jhu.edu/~sleonard/sbr-a4.sh
chmod 755 sbr-a4.sh
./sbr-a4.sh
```

- [ ] 脚本无报错；`colcon build`（或脚本内含的构建）**通过**。

### B3. 启动仿真

```bash
source ~/sbr_workspace/install/setup.bash   # 路径按你机器修改
ros2 launch assignment4 assignment4.launch.py
```

- [ ] Gazebo 打开后，点击 **Play**；车体应落到地面（可能略有弹跳，属正常范围）。
- [ ] 打开遥控窗口：**键盘或 UI** 发速度指令。

### B4. 关键话题与真值（今天就要会用）

在新终端（已 `source`）：

```bash
# EKF 后验（你实现正确后才有意义）
ros2 topic echo /posterior --once

# 传感器（Day 2 主用，今天可先看类型与频率）
ros2 topic info /sensors/gps_0/fix
ros2 topic info /sensors/imu_0/data

# 仿真真值位姿（调 GPS / Expert 对照用）
ign topic -e -t /model/robot/pose
```

**作业重要提示**：

- [ ] **必须发送至少一次遥控指令**，EKF 才会开始更新（PDF 明确说明）。今天跑通后顺手做掉。

### B5. 常见故障速查

| 现象 | 排查 |
|------|------|
| 找不到 `assignment4` 包 | `echo $ROS_DISTRO`、`source install/setup.bash`、是否在同一 workspace |
| Gazebo 极卡 | 关其它占 GPU/CPU 的程序；避免单核 VM |
| `ign topic` 无输出 | 仿真是否已 Play；topic 名是否一致 |
| 编译缺 BFL / 头文件 | 回看 `sbr-a4.sh` 是否装全依赖；把完整报错贴到笔记 |

**上午结束检查清单**

- [ ] 仿真可 Play，可遥控。  
- [ ] 确认 `ekf_models.*` 中待填函数列表。  
- [ ] 笔记里已有 **\(f\)** 的连续时间形式 + 离散化方式。

---

## 下午块 C：实现 `sys_evaluate_f`（约 2-3 h）

### C1. 读骨架：输入输出与 \(\Delta t\)

- [ ] 查清：`sys_evaluate_f` 里 **状态向量引用方式**（指针 / `ColumnVector` 下标）。
- [ ] 查清：**控制量** \(v,\omega\) 从哪里读入（函数参数名）。
- [ ] 查清：**离散时间步长** 是常量还是从 ROS 时间戳计算。

### C2. 实现策略（建议顺序）

1. 从 \(s\) 取出 \(x,y,z,r,p,\psi\)（位置里的 \(y\) 与航向 \(\psi\) 不要搞混）。  
2. 按作业约定构造 \(R(r,p,\psi)=R_X(r)R_Y(p)R_Z(\psi)\)。  
3. 计算 \(\dot x,\dot y,\dot z\)：世界系线速度 = \(R [v,0,0]^T\)（此处 \(\dot y\) 是**位置** \(y\) 对时间的导数）。  
4. 计算 \(\dot r,\dot p,\dot \psi\)：由 \([0,0,\omega]^T\)（车体角速度）映射到欧拉角导数。**此处最易错**，应用符号工具核对。  
5. \(s_{t+1} = s_t + \Delta t \cdot [\dot x,\dot y,\dot z,\dot r,\dot p,\dot \psi]^T\)。  
6. **角度包裹**（若骨架或仿真需要）：例如将航向 \(\psi\) 归一化到 \((-\pi,\pi]\)。**以现有代码其它部分是否已处理为准**，避免重复或冲突。

### C3. 最小自检（不写观测也能做）

- [ ] **静止输入** \(v=0,\omega=0\)：一步预测后状态应近似不变（允许数值微小漂移）。  
- [ ] **只前进**：\(v>0,\omega=0\) 在平地时，主要变化应在 **沿车头方向的世界位移**；\(r,p,\psi\) 变化应符合模型（通常小）。  
- [ ] **原地转**：\(v=0,\omega\neq0\) 时，位置变化应接近 0，**yaw（\(\psi\)）变化**与 \(\omega\Delta t\) 量级一致（具体关系取决于欧拉率公式）。

---

## 下午块 D：实现 `sys_evaluate_A`（约 2-3 h）

### D1. 定义

\[
A_{ij} = \frac{\partial f_i}{\partial s_j}
\]

维度 **6×6**，与状态分量顺序一致。

### D2. 推荐工作流（减少抄错）

1. 在 Matlab Symbolic / Mathematica 中定义 \(f(s,u,\Delta t)\)。  
2. `jacobian(f, s)` 得到符号 \(A\)。  
3. 导出为 C++ 友好表达式，或 **逐项手抄**到 `sys_evaluate_A`。  
4. **抽查**：对某个分量用 **数值差分** \(\frac{f(s+\epsilon e_j)-f(s)}{\epsilon}\) 对比 \(A_{ij}\)（选 2-3 个元素即可）。

### D3. 提交前逻辑检查

- [ ] 若 \(f\) 对某状态分量 **无关**，对应 **Jacobian 列为 0**（除非离散化链隐含依赖）。  
- [ ] \(A\) 不应出现 **未初始化** 元素（骨架若预零填充，仍要逐项确认）。

---

## 傍晚块 E：`sys_evaluate_WMWt` 占位 + 构建（约 0.5-1 h）

- [ ] 今天目标：**正定、对角或简单块对角**，数值取保守（偏大则更信观测；偏小则更信模型；明天有观测后再精调）。  
- [ ] 作业要求过程噪声与 **指令** 相关：可先写 **\(\propto |v|\)、\(\propto|\omega|\)** 的对角放大形式，**Day 3** 再为 Expert 指标微调。  
- [ ] 全量编译：

```bash
cd ~/sbr_workspace
colcon build --packages-select assignment4
source install/setup.bash
```

- [ ] 运行 launch，确认 **无段错误**、节点存活。

---

## Day 1 完成定义（当天必须全打勾）

- [ ] 仿真环境可重复启动；已 **Play** 且 **发过遥控**。  
- [ ] `sys_evaluate_f`、`sys_evaluate_A` 已实现且 **自洽**（与符号/差分抽查一致）。  
- [ ] `sys_evaluate_WMWt` 有 **合理占位**，包 **干净编译**。  
- [ ] 笔记中保存：**RPY 顺序、\(R\) 方向约定、时间步长 \(\Delta t\) 的来源**。Day 2 写 \(h,H\) 时继续用同一约定。

---

## 与 Day 2 的衔接（明天第一件事）

1. 读 `meas_evaluate_gps` / `meas_evaluate_imu` 的接口与 **观测向量拼接顺序**。  
2. 采集一小段 **GPS fix + 真值 pose**，做线性标定或比例关系（PDF 允许用真值辅助 \(h_\mathrm{gps}\)）。  
3. 在 IMU 四元数与 **\(R_X R_Y R_Z\)** RPY 之间建立 **稳定转换**（注意 ROS 四元数顺序与右手系）。

---

## 附：今日时间表示例（可按课表平移）

| 时间段 | 内容 |
|--------|------|
| 09:00-11:00 | 块 A 理论 + 代码地图 |
| 11:00-12:30 | 块 B 安装与仿真 |
| 午休 | |
| 14:00-17:00 | 块 C `sys_evaluate_f` |
| 17:00-19:00 | 块 D `sys_evaluate_A` |
| 19:00-20:00 | 块 E WMWt 占位 + 编译运行 |

（若上午已有环境，可把块 B 压缩到 30 min，把时间拨给 \(A\) 的推导与差分验证。）
