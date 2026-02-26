# Day 1 详细操作指导：环境 + 数据生成 + 理论复习

**预计总时长**：约 4–5 小时  
**产出**：理清 A/B/X 对应关系 + 完成 `generatedata.m` + 验证公式

---

## 上午：理论复习（约 1.5–2h）

### 1. 打开并阅读的讲义

| 顺序 | 文件 | 重点页 |
|------|------|--------|
| 1 | `notes/study_guides/第6讲_手眼标定.md` | 全文，重点第 4 节 AX=XB 推导 |
| 2 | `notes/study_guides/第7讲_AX=XB求解.md` | 第 1–2 节（拆分方程、指数/对数映射） |
| 3 | `lectures/week03/lecture03.pdf` | 第 6–10 页 |

### 2. 必须搞清的对应关系（作业 vs 讲义）

作业中的符号与讲义不同，**务必建立一一对应**：

| 作业符号 | 讲义符号 | 含义 |
|----------|----------|------|
| $E_{bc}$ | 常量（世界/标定板） | Base → Checkerboard，**假设已知** |
| $E_{bh}$ | $E$（末端位姿） | Base → Hand，正向运动学给出 |
| $E_{sc}$ | $S$（传感器测量） | Sensor → Checkerboard，摄像头测得 |
| $X$ | $X$ | Hand → Sensor，**待求的手眼变换** |

**核心方程**（两种写法等价）：
$$E_{bc} = E_{bh} \cdot X \cdot E_{sc} \quad \Leftrightarrow \quad E \cdot X \cdot S = \text{常量}$$

### 3. A、B 的构造公式（必背）

对**两帧** $i$ 和 $j$，由 $E_{bh,i} X E_{sc,i} = E_{bh,j} X E_{sc,j}$ 可得：

$$A = E_{bh,i}^{-1} \cdot E_{bh,j}$$
$$B = E_{sc,i} \cdot E_{sc,j}^{-1}$$

满足 **AX = XB**。

**记忆口诀**：A 是「手」的相对运动（两帧末端之差），B 是「眼」的相对运动（两帧传感器之差）。

### 4. Park & Martin 策略

1. 先求旋转：$R_A R_X = R_X R_B$ → 用对数映射线性化 → 求 $R_X$
2. 再求平移：$(R_A - I) t_X = R_X t_B - t_A$ → 堆叠后最小二乘

### 5. 自测题（复习完做）

- [ ] 写出 $E_{bc} = E_{bh} X E_{sc}$ 中每个矩阵的物理含义
- [ ] 推导：为什么 $A = E_{bh,i}^{-1} E_{bh,j}$，$B = E_{sc,i} E_{sc,j}^{-1}$ 满足 AX=XB？
- [ ] 轴角向量 $\alpha = \omega\theta$ 的方向和长度分别代表什么？

---

## 下午：实现 generatedata.m（约 1.5–2h）

### 1. 创建目录和文件

```bash
mkdir -p assignment2/matlab
cd assignment2/matlab
touch generatedata.m
```

### 2. randSE3() 实现

**随机旋转矩阵 R**（任选其一）：

```matlab
% 方法 A：正交化随机矩阵（简单）
R = orth(randn(3, 3));
% 确保是 proper rotation (det=+1)
if det(R) < 0
    R(:, 1) = -R(:, 1);
end

% 方法 B：Rodrigues 公式（与第 7 讲一致）
% theta = rand * 2 * pi;
% omega = randn(3, 1); omega = omega / norm(omega);
% W = [0 -omega(3) omega(2); omega(3) 0 -omega(1); -omega(2) omega(1) 0];
% R = eye(3) + sin(theta)*W + (1-cos(theta))*(W*W);
```

**随机平移 t**：

```matlab
t = rand(3, 1) * 100;  % 例如 0–100 范围，可自定
```

**组装 4×4 齐次变换**：

```matlab
Rt = [ R, t; 0 0 0 1 ];
```

### 3. generatedata(N) 主循环

**E_bc**（作业给定）：

```matlab
E_bc = [ eye(3), [1; 0; 0]; 0 0 0 1 ];
```

**每次循环**：

1. 生成随机 `E_bh_i = randSE3()`
2. 由 $E_{bc} = E_{bh,i} \cdot X \cdot E_{sc,i}$ 得：
   $$E_{sc,i} = X^{-1} \cdot E_{bh,i}^{-1} \cdot E_{bc}$$

3. 将 `E_bh_i`、`E_sc_i` 转为 ROS 格式并 append 到 `e_bh`、`e_sc`

### 4. 四元数格式转换（易错）

| 来源 | 格式 |
|------|------|
| Matlab `rotm2quat(R)` | `[qw, qx, qy, qz]` |
| ROS / 作业要求 | `[qx, qy, qz, qw]` |

**转换代码**：

```matlab
q_matlab = rotm2quat(R);  % [qw qx qy qz]
q_ros = [q_matlab(2), q_matlab(3), q_matlab(4), q_matlab(1)];  % [qx qy qz qw]
```

**每行格式**：`[tx ty tz qx qy qz qw]`

```matlab
row = [t(1), t(2), t(3), q_ros];
e_bh = [e_bh; row];
```

### 5. 完整 generatedata 骨架

```matlab
function [e_bh, e_sc, X] = generatedata(N)
    E_bc = [ eye(3), [1; 0; 0]; 0 0 0 1 ];
    X = randSE3();
    e_bh = [];
    e_sc = [];
    for i = 1:N
        E_bh = randSE3();
        E_sc = inv(X) * inv(E_bh) * E_bc;  % E_sc = X^{-1} * E_bh^{-1} * E_bc
        % 将 E_bh 转为 [tx ty tz qx qy qz qw]，append 到 e_bh
        % 将 E_sc 转为 [tx ty tz qx qy qz qw]，append 到 e_sc
    end
end
```

**注意**：`inv(E_bh)` 和 `inv(X)` 可直接用，或手写 4×4 齐次变换求逆。

---

## 晚上：验证 generatedata（约 0.5–1h）

### 1. 验证公式

生成数据后，抽查几组检查：

$$E_{bc} \stackrel{?}{=} E_{bh,i} \cdot X \cdot E_{sc,i}$$

```matlab
[e_bh, e_sc, X] = generatedata(5);

% 将 e_bh 第 i 行转回 4x4 矩阵 E_bh_i
% 将 e_sc 第 i 行转回 4x4 矩阵 E_sc_i
% 检查 norm(E_bh_i * X * E_sc_i - E_bc, 'fro') < 1e-10
```

### 2. 验证 A、B 满足 AX=XB

取 $i=1, j=2$：

```matlab
% A = E_bh_1^{-1} * E_bh_2
% B = E_sc_1 * E_sc_2^{-1}
% 检查 norm(A*X - X*B, 'fro') < 1e-10
```

### 3. 辅助函数：7 元组 → 4×4 矩阵

```matlab
function T = vec2transform(row)
    % row = [tx ty tz qx qy qz qw]
    t = row(1:3)';
    q_ros = row(4:7);
    q_matlab = [q_ros(4), q_ros(1), q_ros(2), q_ros(3)];  % [qw qx qy qz]
    R = quat2rotm(q_matlab);
    T = [R, t; 0 0 0 1];
end
```

### 4. 完整验证脚本示例

```matlab
[e_bh, e_sc, X] = generatedata(10);

for i = 1:size(e_bh, 1)
    E_bh_i = vec2transform(e_bh(i,:));
    E_sc_i = vec2transform(e_sc(i,:));
    E_bc = [eye(3), [1;0;0]; 0 0 0 1];
    err = norm(E_bh_i * X * E_sc_i - E_bc, 'fro');
    assert(err < 1e-10, '验证失败: E_bh*X*E_sc != E_bc');
end
disp('所有 E_bc = E_bh*X*E_sc 验证通过');

% 验证 AX=XB
E_bh_1 = vec2transform(e_bh(1,:));
E_bh_2 = vec2transform(e_bh(2,:));
E_sc_1 = vec2transform(e_sc(1,:));
E_sc_2 = vec2transform(e_sc(2,:));
A = inv(E_bh_1) * E_bh_2;
B = E_sc_1 * inv(E_sc_2);
err2 = norm(A*X - X*B, 'fro');
assert(err2 < 1e-10, '验证失败: AX != XB');
disp('AX=XB 验证通过');
```

---

## Day 1 完成检查清单

- [ ] 能默写 A、B 与 E_bh、E_sc 的对应关系
- [ ] `randSE3()` 能生成随机 4×4 SE(3) 变换
- [ ] `generatedata(N)` 输出 e_bh、e_sc 为 N×7，格式 `[tx ty tz qx qy qz qw]`
- [ ] 四元数已正确转换为 ROS 顺序
- [ ] 验证 `E_bc = E_bh_i * X * E_sc_i` 误差 < 1e-10
- [ ] 验证 `A*X ≈ X*B` 误差 < 1e-10

---

## 常见坑提醒

1. **四元数顺序**：`rotm2quat` 输出 `[qw qx qy qz]`，作业要 `[qx qy qz qw]`
2. **齐次变换求逆**：$\begin{pmatrix} R & t \\ 0 & 1 \end{pmatrix}^{-1} = \begin{pmatrix} R^T & -R^T t \\ 0 & 1 \end{pmatrix}$
3. **无 Robotics Toolbox**：若没有 `rotm2quat`/`quat2rotm`，需自写 Rodrigues 或四元数转换

完成以上后，可进入 **Day 2：闭式解 + solveRx**。
