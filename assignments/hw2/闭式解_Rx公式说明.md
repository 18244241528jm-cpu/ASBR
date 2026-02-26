# 闭式解 $R_X = \mathcal{A} \mathcal{B}^{-1}$ 说明

---

## 一、三行公式在做什么？$\mathcal{A}$ 和 $E_{bh}$ 的关系

### 1. 三行公式

$$\mathcal{A} = [\alpha_1 \quad \alpha_2 \quad \alpha_1 \times \alpha_2]$$
$$\mathcal{B} = [\beta_1 \quad \beta_2 \quad \beta_1 \times \beta_2]$$
$$R_X = \mathcal{A} \mathcal{B}^{-1}$$

这是 **Park & Martin 闭式解**中求旋转 $R_X$ 的步骤。

| 公式 | 含义 |
|------|------|
| $\mathcal{A} = [\alpha_1 \ \alpha_2 \ \alpha_1 \times \alpha_2]$ | 用「手」的轴角向量拼成 3×3 矩阵 |
| $\mathcal{B} = [\beta_1 \ \beta_2 \ \beta_1 \times \beta_2]$ | 用「眼」的轴角向量拼成 3×3 矩阵 |
| $R_X = \mathcal{A} \mathcal{B}^{-1}$ | 得到手眼标定的旋转部分 |

### 2. $\mathcal{A}$ 和 $E_{bh}$ 一样吗？

**不一样**，是不同概念：

| 符号 | 含义 | 维度 | 来源 |
|------|------|------|------|
| $E_{bh}$ | 基座到末端的位姿 | 4×4 | 正向运动学，单次测量 |
| $A_i$（AX=XB 里的 A） | 两次末端的相对运动 | 4×4 | $A_i = E_{bh,i}^{-1} E_{bh,j}$ |
| $\mathcal{A}$（公式里的 A） | 轴角向量拼成的矩阵 | 3×3 | $\mathcal{A} = [\alpha_1 \ \alpha_2 \ \alpha_1 \times \alpha_2]$ |

关系链：

```
E_bh_1, E_bh_2  →  A_1 = E_bh_1^{-1} E_bh_2  →  α_1 = log(R_{A_1})  →  𝒜 的第一列
```

讲义常用花体 $\mathcal{A}$、$\mathcal{B}$ 区分，避免和 AX=XB 里的 4×4 矩阵 $A$、$B$ 混淆。

---

## 二、为什么 $\mathcal{A}$ 和 $\mathcal{B}$ 一乘就得到旋转？

### 1. 符号约定

- $\mathcal{A}$ 的三列：$\alpha_1$、$\alpha_2$、$\alpha_1 \times \alpha_2$，其中 $\alpha_i = \log(R_{A_i})$，$A_i = E_{bh,i}^{-1} E_{bh,j}$ 的旋转部分
- $\mathcal{B}$ 的三列：$\beta_1$、$\beta_2$、$\beta_1 \times \beta_2$，其中 $\beta_i = \log(R_{B_i})$，$B_i = E_{sc,i} E_{sc,j}^{-1}$ 的旋转部分

### 2. 核心关系：$\alpha_i = R_X \beta_i$

旋转方程 $R_A R_X = R_X R_B$ 等价于 $R_A = R_X R_B R_X^T$。

在对数空间里，这对应：

$$\alpha = R_X \, \beta$$

即：**手的轴角向量 = 眼的轴角向量被 $R_X$ 旋转后的结果**。

对每一对 $(A_i, B_i)$：

$$\alpha_1 = R_X \beta_1, \quad \alpha_2 = R_X \beta_2$$

### 3. 叉积那列

旋转保持叉积关系：

$$R_X (\beta_1 \times \beta_2) = (R_X \beta_1) \times (R_X \beta_2) = \alpha_1 \times \alpha_2$$

所以第三列也满足同一关系。

### 4. 写成矩阵形式

按列看：

$$\mathcal{A} = [\alpha_1 \ \alpha_2 \ \alpha_1 \times \alpha_2] = [R_X \beta_1 \ \ R_X \beta_2 \ \ R_X (\beta_1 \times \beta_2)]$$

右边可写成：

$$\mathcal{A} = R_X [\beta_1 \ \beta_2 \ \beta_1 \times \beta_2] = R_X \mathcal{B}$$

因此：

$$R_X = \mathcal{A} \mathcal{B}^{-1}$$

### 5. 直观理解

| 列 | 含义 |
|----|------|
| 第 1 列 | $\alpha_1 = R_X \beta_1$：手的轴角 = 眼的轴角被 $R_X$ 旋转 |
| 第 2 列 | $\alpha_2 = R_X \beta_2$ |
| 第 3 列 | $\alpha_1 \times \alpha_2 = R_X (\beta_1 \times \beta_2)$：叉积也被 $R_X$ 一起旋转 |

$\mathcal{A}$ 的每一列都是 $\mathcal{B}$ 对应列被 $R_X$ 旋转的结果，整体即 $\mathcal{A} = R_X \mathcal{B}$，所以 $R_X = \mathcal{A} \mathcal{B}^{-1}$。

---

## 三、平移方程：$(R_{A_i} - I) \mathbf{t}_X = R_X \mathbf{t}_{B_i} - \mathbf{t}_{A_i}$

### 1. 左边为什么是 $(R_{A_i} - I)$？

这是**移项整理**的结果，不是人为乘上去的。

**原始平移方程**（由 $AX = XB$ 的平移部分）：

$$R_A \mathbf{t}_X + \mathbf{t}_A = R_X \mathbf{t}_B + \mathbf{t}_X$$

**移项求 $\mathbf{t}_X$**：把含 $\mathbf{t}_X$ 的项移到左边

$$R_A \mathbf{t}_X - \mathbf{t}_X = R_X \mathbf{t}_B - \mathbf{t}_A$$

左边提取公因子 $\mathbf{t}_X$：

$$R_A \mathbf{t}_X - I \mathbf{t}_X = (R_A - I) \mathbf{t}_X$$

所以得到：

$$(R_A - I) \mathbf{t}_X = R_X \mathbf{t}_B - \mathbf{t}_A$$

$(R_A - I)$ 是移项、提取公因子后自然出现的，不是额外乘上去的。

### 2. 怎么理解平移方程？

**原始形式**：

$$R_A \mathbf{t}_X + \mathbf{t}_A = R_X \mathbf{t}_B + \mathbf{t}_X$$

两边都是在**基座坐标系**下表示的同一个点。

| 左边 $R_A \mathbf{t}_X + \mathbf{t}_A$ | 右边 $R_X \mathbf{t}_B + \mathbf{t}_X$ |
|--------------------------------------|----------------------------------------|
| 路径 $A \cdot X$ 的平移部分：先做 $X$（手→眼），再做 $A$（手的相对运动） | 路径 $X \cdot B$ 的平移部分：先做 $B$（眼的相对运动），再做 $X$（手→眼） |

$AX = XB$ 要求两种路径在基座下得到同一个点，所以平移必须相等。

**整理后的形式**：

$$(R_A - I) \mathbf{t}_X = R_X \mathbf{t}_B - \mathbf{t}_A$$

| 左边 | 右边 |
|------|------|
| $(R_A - I) \mathbf{t}_X$：$\mathbf{t}_X$ 被 $R_A$ 旋转后，再减去自身 | $R_X \mathbf{t}_B - \mathbf{t}_A$：已知量（$R_X$ 已求出，$\mathbf{t}_A$、$\mathbf{t}_B$ 来自测量） |

含义：在已知 $R_X$ 的前提下，用「旋转差」$(R_A - I)$ 作用在未知平移 $\mathbf{t}_X$ 上，等于一个已知向量。

**几何直觉**：$R_A \approx I$（几乎无旋转）时，$R_A - I \approx 0$，方程接近退化，难以唯一确定 $\mathbf{t}_X$。$R_A$ 旋转越明显，对 $\mathbf{t}_X$ 的约束越强。

### 3. 一句话总结

$(R_A - I)$ 来自把 $R_A \mathbf{t}_X + \mathbf{t}_A = R_X \mathbf{t}_B + \mathbf{t}_X$ 移项、提取 $\mathbf{t}_X$ 后的代数结果；平移方程表示 $AX$ 和 $XB$ 在基座下的平移必须相等，整理后得到关于 $\mathbf{t}_X$ 的线性方程。

---

## 四、solveRx：为什么用极分解而不是指数映射？

### 1. 指数映射在做什么？

$$\alpha \xrightarrow{\exp} R = \exp([\omega]\theta)$$

作用：**单个轴角向量 → 对应的旋转矩阵**。

### 2. solveRx 在解什么？

我们要找的是 **$R_X$**，满足：

$$\alpha_i = R_X \beta_i \quad (i=1,\ldots,N)$$

即：$R_X$ 是**连接两组轴角**的旋转，不是「把某个 α 或 β 变回旋转」。

### 3. 用指数映射会怎样？

- $\exp(\alpha_i)$ → $R_{A_i}$（手的旋转）
- $\exp(\beta_i)$ → $R_{B_i}$（眼的旋转）

这些我们本来就有（来自 $A_i$、$B_i$）。问题不是「把轴角变回旋转」，而是**从多组 $(\alpha_i, \beta_i)$ 估计出连接它们的 $R_X$**。

### 4. 极分解在解什么？

最小二乘目标：

$$\min_{R_X \in \text{SO}(3)} \sum_i \|\alpha_i - R_X \beta_i\|^2$$

可等价为：

$$\max_{R_X \in \text{SO}(3)} \text{trace}(R_X^T M), \quad M = \sum_i \beta_i \alpha_i^T$$

这个最优 $R_X$ 恰好由 **$M$ 的极分解**给出：$M = U \Sigma V^T$，则 $R_X = U V^T$。

### 5. 对比

| 操作 | 作用 |
|------|------|
| **指数映射** | 单个轴角 → 旋转，不直接给出「连接两组轴角」的 $R_X$ |
| **极分解** | 从 $M = \sum \beta_i \alpha_i^T$ 直接得到使 $\alpha_i \approx R_X \beta_i$ 的最优 $R_X$ |

**一句话**：指数映射是把轴角变成旋转；solveRx 要的是「连接 α 和 β 的旋转 $R_X$」，这个由极分解直接给出，所以用极分解而不是指数映射。
