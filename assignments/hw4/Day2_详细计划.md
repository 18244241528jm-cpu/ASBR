# HW4（Assignment 4）Day 2 详细计划：观测模型 + Jacobian + 初版调参

**对应作业**：JHU ASBR Assignment 4 - Jackal 6D EKF（BFL）。  
**Day 2 目标**：完成 **GPS / IMU 观测模型** 与各自 Jacobian，给出一版可运行的 **`meas_evaluate_R`**；让 `/posterior` 在仿真中能稳定跟随真值，不再只靠预测步硬撑。  
**预计总时长**：约 **7-9 小时**。

---

## 0. 开始前：Day 1 结果确认

- [ ] `sys_evaluate_f` 与 `sys_evaluate_A` 已能编译通过。
- [ ] 仿真可启动：`ros2 launch assignment4 assignment4.launch.py`。
- [ ] 已确认状态顺序、`R = R_X(r) R_Y(p) R_Z(\psi)` 约定（$\psi$ 为 yaw，作业 PDF 中常记为 $y$）、时间步长 $\Delta t$ 的来源。
- [ ] 已知今天要改的函数：`meas_evaluate_gps`、`meas_evaluate_Hgps`、`meas_evaluate_imu`、`meas_evaluate_Himu`、`meas_evaluate_R`。

---

## 上午块 A：先把观测接口读透（约 1-1.5 h）

### A1. 代码地图

- [ ] 打开 `ekf_models.hpp` / `ekf_models.cpp`，找到所有 `meas_*` 函数。
- [ ] 记下每个函数的 **输入**、**输出向量维度**、**矩阵维度**。
- [ ] 明确 GPS 返回的是不是 **3 维量测**，IMU 返回的是不是 **3 维姿态量测**。

### A2. 先搞清"滤波器在比较什么"

今天最重要的认知不是立刻写代码，而是先回答这两个问题：

1. `meas_evaluate_gps` 应该输出什么？  
   答：它不是"读取传感器"，而是 **从状态 $s$ 预测传感器应该看到什么**。
2. `meas_evaluate_Hgps` / `H_imu` 是什么？  
   答：分别是 **观测函数对状态的一阶导数**，供 EKF 校正步线性化使用。

### A3. 观测拼接顺序

根据 PDF，代码会把两个观测拼成：

$$
h(s) =
\begin{bmatrix}
h_{gps}(s) \\
h_{imu}(s)
\end{bmatrix}
$$

并把 Jacobian 拼成：

$$
H =
\begin{bmatrix}
H_{gps} & 0 \\
0 & H_{imu}
\end{bmatrix}
$$

今天开始前要确认：

- [ ] 代码里 GPS 部分对应状态中的哪几个量。
- [ ] 代码里 IMU 部分对应状态中的哪几个量。
- [ ] 行列顺序与你写的 Jacobian 完全一致。

---

## 上午块 B：GPS 观测模型（约 2.5-3 h）

> PDF 允许你"合理假设地球在机器人附近是平的"，也允许你用真值辅助建立 GPS 与仿真 XYZ 的映射。今天的目标不是地理学最严谨，而是做出 **稳定、一致、可解释** 的 `h_gps`。

### B1. 先采一小段数据

启动仿真后，在几个不同位置记录：

```bash
ros2 topic echo /sensors/gps_0/fix
ign topic -e -t /model/robot/pose
```

建议至少记 **5-8 组点**，每组包括：

- [ ] GPS 的 `latitude`
- [ ] GPS 的 `longitude`
- [ ] GPS 的 `altitude`
- [ ] 真值中的 `x`
- [ ] 真值中的 `y`
- [ ] 真值中的 `z`

### B2. 先判断映射关系大致长什么样

看数据时先回答：

- [ ] `latitude` 更像对应世界系里的哪一轴？
- [ ] `longitude` 更像对应世界系里的哪一轴？
- [ ] `altitude` 是否基本和 `z` 线性对应？
- [ ] 是否存在明显的 **常数偏移 + 比例缩放** 关系？

通常可先尝试这种线性模型（下面 **三行公式**，每行一条；与作业 PDF 中 $\lambda,\phi,\psi$ 表示 GPS 量一致）：

$$
\begin{aligned}
\lambda &= a_x x + b_x \\
\phi &= a_y y + b_y \\
\psi &= a_z z + b_z
\end{aligned}
$$

**纯文本写法**（不依赖公式渲染，复制即可）：

```text
lambda = a_x * x + b_x
phi    = a_y * y + b_y
psi    = a_z * z + b_z
（第二行里的 y 是位置的 y，不是航向 yaw）
```

或者如果轴有互换：

$$
\begin{aligned}
\lambda &= a_y y + b_y \\
\phi &= a_x x + b_x \\
\psi &= a_z z + b_z
\end{aligned}
$$

**纯文本写法**：

```text
lambda = a_y * y + b_y
phi    = a_x * x + b_x
psi    = a_z * z + b_z
```

### B3. 实现 `meas_evaluate_gps`

推荐最小思路：

1. 从状态里取出 `x, y, z`。  
2. 代入你根据采样数据得到的线性关系。  
3. 返回预测的 GPS 三维量测。  

今天的要求不是"一步到位最精确"，而是：

- [ ] 方向对。
- [ ] 单位尺度基本对。
- [ ] 常数偏移已处理。
- [ ] 机器人向某轴正方向走时，预测 GPS 也对应变化。

### B4. 实现 `meas_evaluate_Hgps`

如果你的模型是线性的，例如：

$$
h_{gps}(s)=
\begin{bmatrix}
a_x x + b_x \\
a_y y + b_y \\
a_z z + b_z
\end{bmatrix}
$$

那么它对完整状态 $s=[x,y,z,r,p,\psi]^T$ 的 Jacobian 很简单，就是（$\psi$ 为 yaw，与位置分量 $y$ 区分）：

$$
H_{gps}=
\begin{bmatrix}
a_x & 0   & 0   & 0 & 0 & 0 \\
0   & a_y & 0   & 0 & 0 & 0 \\
0   & 0   & a_z & 0 & 0 & 0
\end{bmatrix}
$$

如果你用了轴交换，就把对应列换过去。  
**关键点**：

- [ ] 偏移项 $b_x,b_y,b_z$ **不会出现在 Jacobian 里**。
- [ ] GPS 若只依赖位置，就 **不应对 roll/pitch/yaw 有导数**。

### B5. GPS 最小验证

- [ ] 推着机器人往前/侧向移动，预测 GPS 与真实 GPS 趋势一致。
- [ ] 如果 `z` 上下起伏时，你的 `altitude` 预测也跟着变。
- [ ] 校正加入后，位置估计比只靠预测时更不漂。

---

## 下午块 C：IMU 观测模型（约 1.5-2 h）

> 这部分通常比 GPS 简单，但最怕 **姿态顺序**、**四元数转欧拉角约定**、**角度包裹** 出错。

### C1. 先看 IMU 原始数据格式

```bash
ros2 topic echo /sensors/imu_0/data
```

重点看：

- [ ] 姿态是否以 quaternion 给出。
- [ ] 是否已经能从别处直接拿到 roll / pitch / yaw。

### C2. 明确 `h_imu` 的含义

滤波状态本身就包含 `roll, pitch, yaw`（下文用 $\psi$ 表示 yaw，避免与位置 $y$ 混淆），因此最常见的观测函数形式是：

$$
h_{imu}(s)=
\begin{bmatrix}
r \\
p \\
\psi
\end{bmatrix}
$$

也就是"如果状态是对的，IMU 理应测到这些姿态角"。

### C3. 实现 `meas_evaluate_imu`

- [ ] 从状态向量取出 `roll, pitch, yaw`。
- [ ] 直接按观测向量顺序返回。
- [ ] 检查是否需要做角度归一化到 `(-\pi,\pi]`。

### C4. 实现 `meas_evaluate_Himu`

若 `h_imu(s) = [r, p, yaw]^T`（与数学记号 $\psi$ 同一分量），则对完整状态的 Jacobian 是：

$$
H_{imu}=
\begin{bmatrix}
0 & 0 & 0 & 1 & 0 & 0 \\
0 & 0 & 0 & 0 & 1 & 0 \\
0 & 0 & 0 & 0 & 0 & 1
\end{bmatrix}
$$

检查点：

- [ ] 不要把 yaw 的列和 `y` 位置状态的列混掉。
- [ ] 命名时分清楚：`y` 既可能表示 **position y**，也可能是 **yaw**，代码里最好用 `yaw` 变量名避免踩坑。

### C5. IMU 最小验证

- [ ] 机器人静止时，姿态估计稳定，不突然跳变。
- [ ] 上坡/下坡时，`roll` / `pitch` 至少趋势合理。
- [ ] 原地转向时，`yaw` 能跟上。

---

## 下午块 D：观测噪声 `meas_evaluate_R` 初版（约 1.5-2 h）

> 今天的目标是"先稳住"，不是一步调到最优。原则：**不确定就先保守**。

### D1. R 的物理意义

$$
R =
\begin{bmatrix}
R_{gps} & 0 \\
0 & R_{imu}
\end{bmatrix}
$$

它表示"你对传感器观测噪声有多大信心"。

- `R` **大**：少信传感器，更信预测模型。
- `R` **小**：多信传感器，校正更强。

### D2. 第一版建议

先写成简单对角阵：

$$
R_{gps} = \mathrm{diag}(\sigma_{\lambda}^2, \sigma_{\phi}^2, \sigma_{\psi}^2)
$$

$$
R_{imu} = \mathrm{diag}(\sigma_r^2, \sigma_p^2, \sigma_y^2)
$$

建议策略：

- [ ] GPS 噪声先设 **中等偏大**，避免线性映射还没调准时把滤波拉飞。
- [ ] IMU 中 `roll/pitch` 可稍微更信一些。
- [ ] `yaw` 不要盲目设太小，否则可能转向时出现抖动或过度纠正。

### D3. 调参顺序

按下面顺序，不要一上来同时乱调所有数：

1. 固定 `R_imu`，先调 `R_gps`。  
2. 让位置轨迹大致跟真值重合。  
3. 再固定 `R_gps`，微调 `R_imu`。  
4. 观察坡地和转向时姿态是否稳定。  

### D4. 今天不要做的事

- [ ] 不要一边改 `R` 一边改 `WMWt`，否则不知道是谁起作用。
- [ ] 不要只凭一次短轨迹就宣布"调好了"。
- [ ] 不要为了暂时平滑，把噪声设得极端小或极端大。

---

## 傍晚块 E：联调与记录（约 1.5-2 h）

### E1. 跑 3 类测试轨迹

每类至少观察 20-40 秒：

1. **直线前进**  
2. **原地转向 / 小半径转弯**  
3. **坡地上行或下行**  

### E2. 你今天要记录的不是"感觉"，而是这些现象

- [ ] `/posterior` 是否明显比纯预测更贴近真值。
- [ ] GPS 一来时位置是否突然剧烈跳变。
- [ ] IMU 一来时姿态是否出现不连续抖动。
- [ ] 哪一维误差最大：`x/y/z` 还是 `roll/pitch/yaw`。

### E3. 若出现问题，优先这样排查

| 现象 | 优先检查 |
|------|----------|
| 位置一融合就乱跳 | GPS 轴映射、比例系数、偏移项、`R_gps` 太小 |
| 姿态估计方向反了 | 四元数转 RPY 顺序、坐标系方向、yaw 符号 |
| yaw 和位置 `y` 混乱 | 变量命名与状态索引 |
| 加入 IMU 后发散 | `H_imu` 列顺序错误、角度 wrap 错误 |
| 全部都不太对 | Day 1 的 `f` / `A` 与今天的 `h` / `H` 约定不一致 |

---

## Day 2 完成定义（当天必须全打勾）

- [ ] `meas_evaluate_gps` 已实现，GPS 与真值位置关系基本合理。
- [ ] `meas_evaluate_Hgps` 已实现，维度与状态顺序正确。
- [ ] `meas_evaluate_imu` 与 `meas_evaluate_Himu` 已实现。
- [ ] `meas_evaluate_R` 有一版可运行参数，滤波器不会一上电就发散。
- [ ] 已做至少 3 类短轨迹测试，并记录主要现象与下一步调参方向。

---

## 与 Day 3 的衔接（明天第一件事）

1. 固定今天的 `h` 与 `H`，优先调 `sys_evaluate_WMWt`。  
2. 用更长轨迹统计 `/posterior` 与真值的 `XYZ` 误差。  
3. 目标转为 Expert Challenge：位置误差向量的 **L2 范数** $\|e\|_2$ 的时间平均 **小于 0.5 m**。  
4. 最后整理打包：`assignment4.tgz`，并做一次干净构建检查。

---

## 附：今日时间表示例

| 时间段 | 内容 |
|--------|------|
| 09:00-10:00 | 块 A 读接口与观测拼接 |
| 10:00-13:00 | 块 B GPS 模型 + `H_gps` |
| 午休 | |
| 14:00-16:00 | 块 C IMU 模型 + `H_imu` |
| 16:00-18:00 | 块 D `R` 初版与调参 |
| 18:00-20:00 | 块 E 联调、记录问题、准备 Day 3 |

（如果 GPS 标定花得比预期久，就优先保证 `meas_evaluate_gps` / `meas_evaluate_Hgps` 正确，再把 `R` 留到傍晚收尾。）
