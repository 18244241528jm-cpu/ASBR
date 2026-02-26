# 第 7 讲：AX=XB 求解 — Park & Martin 方法

**对应讲义**：lecture03.pdf 第 9–15 页 + Robot_sensor_calibration_AXXB.pdf
**本讲目标**：掌握旋转矩阵的指数/对数映射，理解 AX=XB 的"先解旋转、再解平移"策略

---

## 先想清楚：这讲到底在解什么问题？

上一讲我们建立了方程 $AX = XB$，其中 $A, B$ 已知，$X$ 未知。现在要**具体把 $X$ 算出来**。

$X$ 是一个 $4 \times 4$ 齐次变换矩阵，包含一个旋转 $R_X$ 和一个平移 $\mathbf{t}_X$。直接对着整个方程解很复杂，所以 Park & Martin 的策略是：

> **先用旋转部分单独求 $R_X$，再把 $R_X$ 代入平移部分求 $\mathbf{t}_X$。**

---

## 和前面课程的关系

| 上一讲 | 本讲 | 下一讲 |
|--------|------|--------|
| 第 6 讲：建立了 AX=XB 方程 | **具体求解 $X = (R_X, \mathbf{t}_X)$** | 第 8 讲：构型空间（进入运动规划部分） |

> 本讲数学含量较高。核心新工具是旋转矩阵的**指数映射**和**对数映射**——它们在 Lie 群理论中是基本概念，但我们只需要理解"怎么用"，不需要深入拓扑学。

---

## 1. 拆分方程：旋转部分 + 平移部分

> 📖 **对照 PPT**：打开 lecture03.pdf **第 9 页** "Park and Martin Method"，
> 展示了把 $4 \times 4$ 方程拆成两个子方程的过程。

把 $AX = XB$ 写成分块形式：

$$\begin{pmatrix} R_A & \mathbf{t}_A \\ 0 & 1 \end{pmatrix} \begin{pmatrix} R_X & \mathbf{t}_X \\ 0 & 1 \end{pmatrix} = \begin{pmatrix} R_X & \mathbf{t}_X \\ 0 & 1 \end{pmatrix} \begin{pmatrix} R_B & \mathbf{t}_B \\ 0 & 1 \end{pmatrix}$$

展开后得到两个子方程：

**旋转方程**：
$$R_A R_X = R_X R_B \tag{1}$$

**平移方程**：
$$R_A \mathbf{t}_X + \mathbf{t}_A = R_X \mathbf{t}_B + \mathbf{t}_X \tag{2}$$

策略：先用方程 (1) 求 $R_X$，再代入方程 (2) 求 $\mathbf{t}_X$。

---

## 2. 新工具：旋转的指数映射与对数映射

> 📖 **对照 PPT**：打开 lecture03.pdf **第 10 页** "Solving the Rotation"，
> 介绍了旋转矩阵可以用**矩阵指数** $e^{[\omega]\theta}$ 表示。

### 2.1 指数映射：轴-角 → 旋转矩阵

任何旋转都可以用一个**旋转轴** $\omega$（单位向量）和一个**旋转角** $\theta$ 来表示：

$$R(\omega, \theta) = e^{[\omega]\theta}$$

其中 $[\omega]$ 是 $\omega$ 对应的反对称矩阵（第 5 讲学过）。

**一句话直觉**：绕轴 $\omega$ 旋转 $\theta$ 弧度。就像地球绕南北极轴旋转。

> 具体计算用 **Rodrigues 公式**：
> $$e^{[\omega]\theta} = I + \sin\theta \cdot [\omega] + (1 - \cos\theta) \cdot [\omega]^2$$

### 2.2 对数映射：旋转矩阵 → 轴-角（怎么对矩阵取对数？）

**数学含义**：对旋转矩阵取对数，指的是**李群 SO(3) 上的对数映射**，将旋转矩阵 $R \in \text{SO}(3)$ 映射到李代数 $\mathfrak{so}(3)$ 中的元素，得到轴角向量 $\boldsymbol{\alpha} = \boldsymbol{\omega}\theta$（3 个数）：
- **方向** = 旋转轴 $\boldsymbol{\omega}$
- **长度** = 旋转角 $\theta$（弧度）

**三种情况的公式**：

| 情况 | 条件 | 结果 |
|------|------|------|
| 1 | $R = I$（单位矩阵） | $\log(R) = \mathbf{0}$ |
| 2 | $\text{trace}(R) \neq -1$（一般情况） | 见下方公式 |
| 3 | $\text{trace}(R) = -1$（$\theta = \pi$） | 需单独处理 180° 旋转 |

**一般情况（$\theta \neq \pi$）的步骤**：

1. 求旋转角：
   $$\theta = \arccos\left(\frac{\text{trace}(R) - 1}{2}\right)$$

2. 求反对称矩阵：
   $$[\boldsymbol{\omega}]_\times = \frac{\theta}{2\sin\theta}(R - R^\top)$$

3. 从反对称矩阵提取轴角向量 $\boldsymbol{\alpha}$：
   $$[\boldsymbol{\omega}]_\times = \begin{bmatrix} 0 & -\omega_z & \omega_y \\ \omega_z & 0 & -\omega_x \\ -\omega_y & \omega_x & 0 \end{bmatrix} \quad \Rightarrow \quad \boldsymbol{\alpha} = \theta \cdot \boldsymbol{\omega}$$

**$\theta = \pi$ 的特殊情况**：此时 $\sin\theta = 0$，上式分母为零。需从 $R + I$ 的非零列提取旋转轴 $\boldsymbol{\omega}$，再令 $\boldsymbol{\alpha} = \pi \cdot \boldsymbol{\omega}$。

**与指数映射的关系**：$\log$ 与 $\exp$ 互为逆映射，即 $\log(\exp(\boldsymbol{\alpha})) = \boldsymbol{\alpha}$，$\exp(\log(R)) = R$。

**一句话直觉**：对数映射把一个旋转矩阵（$3 \times 3$, 9 个数）"压缩"成一个 3D 向量 $\alpha = \omega \theta$（3 个数），其方向是旋转轴，长度是旋转角。

### 2.3 数值算例：指数/对数映射

绕 z 轴旋转 90°：$\omega = (0, 0, 1)^T$，$\theta = \pi/2$

```python
import numpy as np

def skew(w):
    """3D 向量 → 反对称矩阵"""
    return np.array([[0, -w[2], w[1]],
                     [w[2], 0, -w[0]],
                     [-w[1], w[0], 0]])

def exp_so3(w, theta):
    """指数映射：轴角 → 旋转矩阵 (Rodrigues)"""
    W = skew(w)
    return np.eye(3) + np.sin(theta)*W + (1 - np.cos(theta))*(W @ W)

def log_so3(R):
    """对数映射：旋转矩阵 → 轴角向量 α = ωθ"""
    theta = np.arccos(np.clip((np.trace(R) - 1) / 2, -1, 1))
    if abs(theta) < 1e-10:  # R ≈ I
        return np.zeros(3)
    if abs(theta - np.pi) < 1e-10:  # θ = π 的特殊情况
        w = np.array([R[2,1]-R[1,2], R[0,2]-R[2,0], R[1,0]-R[0,1]])
        if np.linalg.norm(w) < 1e-10:
            return np.array([np.pi, 0, 0])
        w = w / np.linalg.norm(w)
        return np.pi * w
    # 一般情况: [ω] = (R-R^T)/(2sinθ), 则 α = θ·ω
    W = (R - R.T) / (2 * np.sin(theta))
    w = np.array([W[2,1], W[0,2], W[1,0]])
    return theta * w

# 绕 z 轴旋转 90°
omega = np.array([0, 0, 1])
theta = np.pi / 2
R = exp_so3(omega, theta)
print("R (绕z轴90°):")
print(np.round(R, 4))

# 对数映射还原
alpha = log_so3(R)
print(f"\nlog(R) = {np.round(alpha, 4)}")
print(f"旋转轴: {alpha / np.linalg.norm(alpha)}")
print(f"旋转角: {np.degrees(np.linalg.norm(alpha)):.1f}°")
```

---

**🧭 有了指数/对数映射这个工具，就可以求解旋转方程了。**

---

## 3. 求解旋转 $R_X$（两组数据的精确解）

> 📖 **对照 PPT**：打开 lecture03.pdf **第 11 页** "Solving the Rotation"（第二页），
> 给出了利用对数映射把旋转方程线性化的思路。

### 3.1 核心思路

对每组数据 $(A_i, B_i)$，取对数：

$$\alpha_i = \log(R_{A_i}), \quad \beta_i = \log(R_{B_i})$$

旋转方程 $R_A R_X = R_X R_B$ 在对数空间中变成：

$$\alpha_i = R_X \beta_i$$

**直觉**：$R_X$ 把 $B$ 的旋转轴"旋转"到 $A$ 的旋转轴方向。

### 3.2 两组数据的解

有两组数据 $(\alpha_1, \beta_1)$ 和 $(\alpha_2, \beta_2)$，构造矩阵：

$$\mathcal{A} = [\alpha_1 \quad \alpha_2 \quad \alpha_1 \times \alpha_2]$$
$$\mathcal{B} = [\beta_1 \quad \beta_2 \quad \beta_1 \times \beta_2]$$

则：

$$R_X = \mathcal{A} \mathcal{B}^{-1}$$

> **前提条件**：$\alpha_1 \times \alpha_2 \neq 0$ 且 $\beta_1 \times \beta_2 \neq 0$，即两组运动的旋转轴**不平行**。这就是为什么要做两次**不同方向**的运动。

---

## 4. 多组数据的最小二乘解

> 📖 **对照 PPT**：打开 lecture03.pdf **第 13–14 页** "More Than Two Data Points"，
> 给出了 N 组数据的最小二乘拟合公式。

实际中有噪声，我们采集 $N \gg 2$ 组数据 $(A_i, B_i)$，然后找最优 $R_X$。

构造矩阵 $M$：

$$M = \sum_{i=1}^{N} \beta_i \alpha_i^T$$

然后用**极分解**（Polar Decomposition）：

$$R_X = (M^T M)^{-1/2} M^T$$

> $(M^T M)^{-1/2}$ 可以通过特征值分解计算：$M^T M = Q \Lambda Q^{-1}$，
> 则 $(M^T M)^{-1/2} = Q \Lambda^{-1/2} Q^{-1}$，其中 $\Lambda^{-1/2}$ 对每个特征值取倒数再开方。

---

## 5. 求解平移 $\mathbf{t}_X$

> 📖 **对照 PPT**：打开 lecture03.pdf **第 12 页** "Solving the Translation" 和 **第 15 页**。

已经求得 $R_X$，回到平移方程 (2)：

$$R_A \mathbf{t}_X + \mathbf{t}_A = R_X \mathbf{t}_B + \mathbf{t}_X$$

整理：

$$(R_A - I) \mathbf{t}_X = R_X \mathbf{t}_B - \mathbf{t}_A$$

用 $N$ 组数据堆叠：

$$\begin{pmatrix} R_{A_1} - I \\ R_{A_2} - I \\ \vdots \\ R_{A_N} - I \end{pmatrix} \mathbf{t}_X = \begin{pmatrix} R_X \mathbf{t}_{B_1} - \mathbf{t}_{A_1} \\ R_X \mathbf{t}_{B_2} - \mathbf{t}_{A_2} \\ \vdots \\ R_X \mathbf{t}_{B_N} - \mathbf{t}_{A_N} \end{pmatrix}$$

这是一个**超定线性方程组** $C \mathbf{t}_X = \mathbf{d}$，用最小二乘求解：

$$\mathbf{t}_X = (C^T C)^{-1} C^T \mathbf{d}$$

---

## 6. 完整数值算例

使用 Park & Martin 论文中的数据（lecture03.pdf 对应 Robot_sensor_calibration_AXXB.pdf 第 4 页的例子）：

```python
import numpy as np

def skew(w):
    return np.array([[0, -w[2], w[1]], [w[2], 0, -w[0]], [-w[1], w[0], 0]])

def log_so3(R):
    theta = np.arccos(np.clip((np.trace(R) - 1) / 2, -1, 1))
    if abs(theta) < 1e-10:
        return np.zeros(3)
    W = (R - R.T) / (2 * np.sin(theta))
    w = np.array([W[2,1], W[0,2], W[1,0]])
    return w * theta

def make_transform(R, t):
    E = np.eye(4)
    E[:3, :3] = R
    E[:3, 3] = t
    return E

# === 已知真实 X（最终要验证） ===
R_X_true = np.array([[1, 0, 0], [0, 0.980067, -0.198669], [0, 0.198669, 0.980067]])
t_X_true = np.array([0, 50, 100])

# === 两组已知的 A 和 B ===
A1 = np.array([[-0.989992, -0.141120, 0, 0],
               [ 0.141120, -0.989992, 0, 0],
               [ 0,         0,        1, 0],
               [ 0,         0,        0, 1]])

B1 = np.array([[-0.989992, -0.138307,  0.028036, -26.9559],
               [ 0.138307, -0.911439,  0.387470, -96.1332],
               [-0.028036,  0.387470,  0.921456,  19.4872],
               [ 0,         0,         0,         1]])

A2 = np.array([[ 0.070737, 0, 0.997495, -400],
               [ 0,        1, 0,          0],
               [-0.997495, 0, 0.070737,  400],
               [ 0,        0, 0,          1]])

B2 = np.array([[ 0.070737,  0.198172,  0.977612, -309.543],
               [-0.198172,  0.963323, -0.180936,   59.0244],
               [-0.977612, -0.180936,  0.107415,  291.177],
               [ 0,         0,         0,          1]])

# === Step 1: 求 R_X ===
R_A1, R_A2 = A1[:3,:3], A2[:3,:3]
R_B1, R_B2 = B1[:3,:3], B2[:3,:3]

alpha1 = log_so3(R_A1)
alpha2 = log_so3(R_A2)
beta1  = log_so3(R_B1)
beta2  = log_so3(R_B2)

A_mat = np.column_stack([alpha1, alpha2, np.cross(alpha1, alpha2)])
B_mat = np.column_stack([beta1,  beta2,  np.cross(beta1,  beta2)])

R_X = A_mat @ np.linalg.inv(B_mat)
print("求解得到 R_X:")
print(np.round(R_X, 4))
print(f"\n真实 R_X:")
print(np.round(R_X_true, 4))

# === Step 2: 求 t_X ===
t_A1, t_A2 = A1[:3,3], A2[:3,3]
t_B1, t_B2 = B1[:3,3], B2[:3,3]

C = np.vstack([R_A1 - np.eye(3), R_A2 - np.eye(3)])
d = np.concatenate([R_X @ t_B1 - t_A1, R_X @ t_B2 - t_A2])

t_X = np.linalg.lstsq(C, d, rcond=None)[0]
print(f"\n求解得到 t_X: {np.round(t_X, 2)}")
print(f"真实 t_X:     {t_X_true}")

# === 验证: AX = XB ===
X = make_transform(R_X, t_X)
print(f"\nA1 @ X ≈ X @ B1? {np.allclose(A1 @ X, X @ B1, atol=0.1)}")
print(f"A2 @ X ≈ X @ B2? {np.allclose(A2 @ X, X @ B2, atol=0.1)}")
```

---

## 7. 本讲小结

| 概念 | 要点 |
|------|------|
| **求解策略** | 先解旋转 $R_X$（方程 1），再解平移 $\mathbf{t}_X$（方程 2） |
| **指数映射** $e^{[\omega]\theta}$ | 轴-角表示 → 旋转矩阵（Rodrigues 公式） |
| **对数映射** $\log(R)$ | 旋转矩阵 → 轴-角向量 $\alpha = \omega\theta$ |
| **两组精确解** | $R_X = \mathcal{A}\mathcal{B}^{-1}$，要求旋转轴不平行 |
| **N 组最小二乘** | $R_X = (M^TM)^{-1/2}M^T$（极分解），$\mathbf{t}_X$ 用线性最小二乘 |

**学完本讲你应该能**：
1. 把 $AX = XB$ 拆分成旋转和平移两个子方程
2. 用对数映射把旋转矩阵转成轴-角向量
3. 用 Rodrigues 公式把轴-角转回旋转矩阵
4. 解释为什么两组运动的旋转轴必须不平行
5. 理解多组数据的最小二乘流程

---

## 8. 自测与练习建议

1. **手算**：给定 $R = R_z(60°)$，计算 $\log(R)$（$\theta$ 和 $\omega$）
2. **手算**：给定 $\omega = (1, 0, 0)^T$，$\theta = 90°$，用 Rodrigues 公式计算 $R$
3. **编程**：实现完整的手眼标定流程（`log_so3` → 构造 $\mathcal{A}, \mathcal{B}$ → 求 $R_X$ → 求 $\mathbf{t}_X$）
4. **编程**：生成带噪声的 $(A_i, B_i)$ 数据，测试最小二乘解随数据量增加的精度变化
5. **思考题**：如果所有运动的旋转轴都平行（比如机器人只绕 z 轴转），为什么 $R_X$ 无法唯一确定？

完成以上后，可以进入 **第 8 讲：构型空间 — 从工作空间到 C-space**。
