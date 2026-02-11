# Assignment 1 第四、五部分解析：前向运动学与轨迹生成

**章节**: 
- 4. Forward Kinematics of the UR5 and its Inverse
- 5. Inverse Jacobian Trajectory Generator

**重要性**: ⭐⭐⭐⭐⭐ 这是作业的核心数学和算法部分

---

## 📋 第四部分概述：UR5 前向运动学

### 4.1 任务目标

**主要任务**：推导 UR5 机器人的前向运动学

**具体要求**：
- 计算 `gripper_pick` 坐标系相对于 `world` 坐标系的：
  - **3D 旋转**（旋转矩阵）
  - **3D 位置**（平移向量）
- 作为关节角度 $q_1, q_2, \ldots, q_6$ 的函数

### 4.2 UR5 机器人结构

**关节配置**：
- **6个旋转关节**（全部是旋转关节，revolute joints）
- **肩部**：2个关节
- **肘部**：1个关节
- **腕部**：3个关节

**关节角度向量**：
$$q = [q_1, q_2, q_3, q_4, q_5, q_6]^T$$

### 4.3 关节与连杆对应关系

| 关节名称 (Joint Name) | 连杆名称 (Link Name) |
|---------------------|-------------------|
| `shoulder_pan_joint` | `shoulder_link` |
| `shoulder_lift_joint` | `upper_arm_link` |
| `elbow_joint` | `forearm_link` |
| `wrist_1_joint` | `wrist_1_link` |
| `wrist_2_joint` | `wrist_2_link` |
| `wrist_3_joint` | `wrist_3_link` |

**注意**：所有关节都绕 **Z 轴**旋转

---

## 🔧 4.4 前向运动学推导步骤

### 步骤1：找到相邻坐标系之间的变换

**目标**：找到每个相邻坐标系之间的变换 ${}^i E_j$

**方法**：
1. 使用 `tf2_echo` 或 RViz 确定每个 ${}^i E_j$ 的自由度（DoF）
2. 分析每个变换的旋转和平移部分

**示例**：
- ${}^{world} E_{base}$：world 到 base_link 的变换
- ${}^{base} E_{shoulder}$：base_link 到 shoulder_link 的变换
- ${}^{shoulder} E_{upper\_arm}$：shoulder_link 到 upper_arm_link 的变换
- ... 依此类推

### 步骤2：验证每个变换

**验证方法**：
1. 改变关节值（使用 GUI 滑块）
2. 使用 `tf2_echo` 查看实际输出
3. 与你的计算结果比较

**Matlab 技巧**：
- 使用 `subs` 命令代入具体数值
- 例如：`subs(E_i_j, [q1, q2, ...], [0.5, -1.57, ...])`

### 步骤3：组合所有变换

**前向运动学公式**：

$${}^{world} E_{gripper} = {}^{world} E_{base} \cdot {}^{base} E_{shoulder} \cdot {}^{shoulder} E_{upper\_arm} \cdot \ldots \cdot {}^{wrist\_3} E_{gripper}$$

**矩阵形式**：
$$T_{gripper}^{world} = T_1(q_1) \cdot T_2(q_2) \cdot T_3(q_3) \cdot T_4(q_4) \cdot T_5(q_5) \cdot T_6(q_6)$$

其中每个 $T_i(q_i)$ 是 4×4 齐次变换矩阵。

### 步骤4：简化表达式

**使用 Matlab**：
- 对每个 ${}^i E_j$ 使用 `simplify` 命令
- 对最终的前向运动学结果使用 `simplify` 命令
- 简化可以：
  - 减少计算量
  - 发现潜在的简化
  - 提高数值稳定性

### 步骤5：验证最终结果

**验证工具**：`tf2_echo`

**验证步骤**：
1. 设置一组关节角度
2. 计算前向运动学
3. 使用 `tf2_echo world gripper_pick` 查看实际值
4. 比较结果

**关键点**：
- `tf2_echo` 是**找到错误的工具**
- 如果计算结果与 `tf2_echo` 不一致，说明计算有误
- **技术上没有借口出错**（因为有验证工具）

---

## 🔄 4.5 前向运动学的逆

### 任务

**计算逆变换**：${}^{gripper} E_{world}$

**关系**：
$${}^{gripper} E_{world} = ({}^{world} E_{gripper})^{-1}$$

### 齐次变换矩阵的逆

对于齐次变换矩阵：
$$T = \begin{bmatrix} R & t \\ 0 & 1 \end{bmatrix}$$

其逆为：
$$T^{-1} = \begin{bmatrix} R^T & -R^T t \\ 0 & 1 \end{bmatrix}$$

**解释**：
- 旋转部分的逆 = 转置（因为旋转矩阵是正交的）
- 平移部分 = $-R^T t$（考虑旋转后的反向平移）

### 用途

**在轨迹生成中的作用**：
- 将目标点从 world 坐标系转换到 gripper 坐标系
- 计算误差向量 $e = {}^{gripper} P_{goal}$

---

## 🎯 第五部分概述：逆雅可比轨迹生成器

### 5.1 问题描述

**目标**：
- 将机器人夹爪移动到指定的 3D 坐标点
- 例如：${}^W P = [0.5, 0.5, 0.2]^T$（在 world 坐标系中）
- **不使用** `rqt_joint_trajectory_controller` 节点

**要求**：
- 关节逐渐移动到目标位置
- 沿直线移动（straight 3D line）
- 恒定线速度（constant linear velocity）
- **不改变夹爪的方向**（orientation 保持不变）

### 5.2 Resolved Motion Rate Control（解析运动速率控制）

**概念**：使用操作臂雅可比矩阵来控制末端执行器的运动

**示意图**（图3：2D 机器人示例）：
- 基座 $B$ 在原点
- 两个连杆，末端执行器 $T$
- 目标点 $P$ 在空间中
- 误差向量 $e$ 从 $T$ 指向 $P$

**控制思想**：
- 计算误差向量 $e$
- 将 $e$ 作为期望的 body 线速度
- 通过雅可比矩阵反解关节速度
- 执行小步移动，重复直到到达目标

---

## 📐 5.3 误差向量计算

### 误差向量的定义

**在每次迭代中**：
1. 计算目标点 $P$ 在 gripper 坐标系中的坐标
2. 这个坐标就是误差向量 $e$

**数学表示**：
$${}^G P = {}^G E_W \cdot {}^W P$$

其中：
- ${}^W P$：目标点在 world 坐标系中的坐标
- ${}^G E_W$：world 到 gripper 的变换（前向运动学的逆）
- ${}^G P$：目标点在 gripper 坐标系中的坐标 = **误差向量 $e$**

### 误差向量的物理意义

**Body 线速度**：
- 误差向量 $e$ 定义了**期望的 body 线速度** ${}^W v_G^b$
- 这是从 gripper 坐标系 $G$ 中看到的运动方向
- 表示"我想朝这个方向移动"

---

## 🔢 5.4 速度变换方程

### 空间速度（Spatial Velocity）

**操作臂雅可比矩阵**：
$$\begin{bmatrix} {}^W v_G^s \\ {}^W \omega_G^s \end{bmatrix} = J(q) \dot{q}$$

其中：
- ${}^W v_G^s$：gripper 在 world 坐标系中的**空间线速度**
- ${}^W \omega_G^s$：gripper 在 world 坐标系中的**空间角速度**
- $J(q)$：6×6 操作臂空间雅可比矩阵
- $\dot{q}$：关节角速度向量

### 伴随变换（Adjoint Transformation）

**Body 速度到空间速度的转换**：
$$\begin{bmatrix} {}^W v_G^s \\ {}^W \omega_G^s \end{bmatrix} = Ad \begin{bmatrix} {}^W v_G^b \\ {}^W \omega_G^b \end{bmatrix} = \begin{bmatrix} R & \hat{t} R \\ 0 & R \end{bmatrix} \begin{bmatrix} {}^W v_G^b \\ {}^W \omega_G^b \end{bmatrix}$$

其中：
- $R$：旋转矩阵（从 world 到 gripper）
- $\hat{t}$：平移向量 $t$ 的反对称矩阵（skew-symmetric matrix）
- ${}^W v_G^b$：**Body 线速度**（从 gripper 坐标系看）
- ${}^W \omega_G^b$：**Body 角速度**（从 gripper 坐标系看）

**反对称矩阵**：
对于向量 $t = [t_x, t_y, t_z]^T$：
$$\hat{t} = \begin{bmatrix}
0 & -t_z & t_y \\
t_z & 0 & -t_x \\
-t_y & t_x & 0
\end{bmatrix}$$

### 组合方程

**将误差向量作为 body 线速度**：

由于 $e$ 表示期望的 body 线速度 ${}^W v_G^b$，我们可以写出：

$$\begin{bmatrix} {}^W v_G^b \\ {}^W \omega_G^b \end{bmatrix} = Ad^{-1} J(q) \dot{q}$$

**具体形式**：
$$\begin{bmatrix} e \\ {}^W \omega_G^b \end{bmatrix} = Ad^{-1} J(q) \dot{q}$$

其中：
- $e$：误差向量（期望的 body 线速度）
- ${}^W \omega_G^b = 0$：**不改变方向**（角速度为0）

---

## ⚙️ 5.5 实现步骤

### 轨迹生成器的算法流程

**在每次迭代中**（使用当前关节角度 $q$）：

#### 步骤1：计算空间雅可比矩阵

$$J(q) = \text{计算 6×6 空间雅可比矩阵}$$

**雅可比矩阵的组成**：
$$J(q) = \begin{bmatrix} J_v(q) \\ J_\omega(q) \end{bmatrix}$$

其中：
- $J_v(q)$：3×6 线速度雅可比（前3行）
- $J_\omega(q)$：3×6 角速度雅可比（后3行）

#### 步骤2：计算逆伴随变换

$$Ad^{-1} = \text{计算 6×6 逆伴随变换矩阵}$$

**逆伴随变换公式**：
$$Ad^{-1} = \begin{bmatrix} R^T & -R^T \hat{t} \\ 0 & R^T \end{bmatrix}$$

**推导**：
- 从 $Ad = \begin{bmatrix} R & \hat{t} R \\ 0 & R \end{bmatrix}$
- 利用旋转矩阵的逆 = 转置
- 计算得到 $Ad^{-1}$

#### 步骤3：计算组合矩阵并求逆

$$M = Ad^{-1} J(q)$$

**求解关节速度**：
$$\dot{q} = M^{-1} \begin{bmatrix} e \\ 0 \end{bmatrix}$$

其中：
- $e$：误差向量（3×1）
- $0$：零向量（3×1，表示角速度为0）

#### 步骤4：执行运动

1. 计算新的关节角度：
   $$q_{new} = q_{old} + \dot{q} \cdot \Delta t$$

2. 其中 $\Delta t = 0.01$ 秒（小时间步长）

3. 将新的关节角度发送给机器人

4. **重复**步骤1-4，直到到达目标点

---

## 📝 5.6 你需要实现的部分

### 你的任务（6分）

1. **ForwardKinematics（3分）**
   - 实现前向运动学
   - 输入：关节角度 $q_1, \ldots, q_6$
   - 输出：4×4 齐次变换矩阵 ${}^{world} E_{gripper}$

2. **ForwardKinematicsInverse（1分）**
   - 实现前向运动学的逆
   - 输入：4×4 齐次变换矩阵
   - 输出：4×4 逆变换矩阵 ${}^{gripper} E_{world}$
   - 用于计算误差向量 $e$

3. **AdjointTransformationInverse（2分）**
   - 实现逆伴随变换 $Ad^{-1}$
   - 输入：旋转矩阵 $R$ 和平移向量 $t$
   - 输出：6×6 逆伴随变换矩阵

4. **Jacobian（3分）**
   - 实现 6×6 操作臂空间雅可比矩阵 $J(q)$
   - 输入：关节角度 $q$
   - 输出：6×6 雅可比矩阵

### 已实现的部分

**不需要你实现**：
- 求解 $\dot{q}$（矩阵求逆）
- 轨迹插值
- ROS 消息发布
- 控制循环

---

## 💡 5.7 实现建议

### 使用 Matlab 推导

**强烈推荐**：
1. **在 Matlab 中推导公式**
   - 使用符号计算（Symbolic Math Toolbox）
   - 推导每个变换矩阵
   - 推导雅可比矩阵

2. **使用 `ccode` 命令导出**
   ```matlab
   ccode(simplified_expression)
   ```
   - 将符号表达式转换为 C++ 代码
   - 减少手写代码的错误

3. **验证中间结果**
   - 使用 `subs` 代入数值验证
   - 使用 `simplify` 简化表达式

### 代码实现注意事项

**⚠️ 重要警告**：

1. **填充所有矩阵元素**
   - 不要假设矩阵元素初始化为0
   - **必须显式设置每个元素**
   - 未初始化的值可能导致错误

2. **矩阵维度检查**
   - 确保所有矩阵维度正确
   - 4×4 齐次变换矩阵
   - 6×6 雅可比矩阵和伴随变换

3. **坐标系一致性**
   - 确保所有坐标系定义一致
   - 检查旋转方向（右手/左手坐标系）

---

## 🔍 5.8 调试技巧

### 验证前向运动学

1. **使用 tf2_echo**
   ```bash
   ros2 run tf2_ros tf2_echo world gripper_pick
   ```

2. **比较结果**
   - 设置一组关节角度
   - 计算前向运动学
   - 与 tf2_echo 输出比较

3. **检查精度**
   - 位置误差应该 < 1mm
   - 旋转误差应该 < 0.1°

### 验证雅可比矩阵

1. **数值微分验证**
   - 使用有限差分法计算数值雅可比
   - 与解析雅可比比较

2. **速度一致性检查**
   - 给定关节速度 $\dot{q}$
   - 计算末端速度
   - 验证是否合理

### 验证伴随变换

1. **单位测试**
   - 测试 $Ad \cdot Ad^{-1} = I$
   - 验证逆变换的正确性

2. **速度转换验证**
   - 给定 body 速度
   - 转换为 spatial 速度
   - 再转换回 body 速度
   - 应该得到原始值

---

## 📊 5.9 数学公式总结

### 前向运动学

$$T_{gripper}^{world} = T_1(q_1) T_2(q_2) T_3(q_3) T_4(q_4) T_5(q_5) T_6(q_6)$$

### 逆前向运动学

$$T_{world}^{gripper} = (T_{gripper}^{world})^{-1} = \begin{bmatrix} R^T & -R^T t \\ 0 & 1 \end{bmatrix}$$

### 误差向量

$$e = {}^G P_{goal} = {}^G E_W \cdot {}^W P_{goal}$$

### 空间速度

$$\begin{bmatrix} {}^W v_G^s \\ {}^W \omega_G^s \end{bmatrix} = J(q) \dot{q}$$

### 伴随变换

$$Ad = \begin{bmatrix} R & \hat{t} R \\ 0 & R \end{bmatrix}, \quad Ad^{-1} = \begin{bmatrix} R^T & -R^T \hat{t} \\ 0 & R^T \end{bmatrix}$$

### 关节速度求解

$$\begin{bmatrix} e \\ 0 \end{bmatrix} = Ad^{-1} J(q) \dot{q}$$

$$\dot{q} = (Ad^{-1} J(q))^{-1} \begin{bmatrix} e \\ 0 \end{bmatrix}$$

---

## ✅ 实现检查清单

### 前向运动学
- [ ] 正确计算每个相邻坐标系之间的变换
- [ ] 正确组合所有变换
- [ ] 使用 tf2_echo 验证结果
- [ ] 简化表达式（使用 Matlab simplify）

### 逆前向运动学
- [ ] 正确计算齐次变换矩阵的逆
- [ ] 验证 $T \cdot T^{-1} = I$
- [ ] 正确计算误差向量 $e$

### 逆伴随变换
- [ ] 正确实现 $Ad^{-1}$ 公式
- [ ] 验证 $Ad \cdot Ad^{-1} = I$
- [ ] 正确转换 body 速度到 spatial 速度

### 雅可比矩阵
- [ ] 正确计算 6×6 空间雅可比矩阵
- [ ] 线速度部分 $J_v(q)$ 正确
- [ ] 角速度部分 $J_\omega(q)$ 正确
- [ ] 使用数值方法验证

### 代码质量
- [ ] 所有矩阵元素都显式设置
- [ ] 没有未初始化的值
- [ ] 矩阵维度正确
- [ ] 代码可以编译

---

## 🚀 下一步

完成数学推导和代码实现后：

1. **编译代码**
   ```bash
   cd ~/sbr_workspace
   colcon build
   source install/setup.bash
   ```

2. **测试轨迹生成**
   - 启动仿真
   - 发布目标点
   - 观察机器人运动

3. **验证结果**
   - 检查是否到达目标点
   - 检查方向是否保持不变
   - 检查运动是否平滑

---

## 📖 相关概念回顾

### 齐次变换矩阵

$$T = \begin{bmatrix} R & t \\ 0 & 1 \end{bmatrix}$$

- $R$：3×3 旋转矩阵
- $t$：3×1 平移向量

### 反对称矩阵

对于向量 $v = [v_x, v_y, v_z]^T$：
$$\hat{v} = \begin{bmatrix}
0 & -v_z & v_y \\
v_z & 0 & -v_x \\
-v_y & v_x & 0
\end{bmatrix}$$

性质：$\hat{v} u = v \times u$（叉积）

### 雅可比矩阵

**空间雅可比**：描述关节速度到空间速度的映射

**Body 雅可比**：描述关节速度到 body 速度的映射

**关系**：$J_{body} = Ad^{-1} J_{spatial}$

---

**解析完成！** 现在你应该理解前向运动学和轨迹生成器的数学原理了。

---

*解析完成时间：2024年*  
*如有疑问，请参考原始PDF文档或联系课程助教*

