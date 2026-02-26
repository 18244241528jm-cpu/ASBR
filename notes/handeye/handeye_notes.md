# 手眼标定与旋转矩阵指数映射笔记

**来源**: `handeye.pdf` (手写笔记转写)  
**转写工具**: Gemini OCR  
**整理日期**: 2024年

---

## 目录

1. [旋转运动学基础](#第一页旋转运动学基础)
2. [相似变换与伴随性质](#第二页相似变换与伴随性质)
3. [旋转与叉积的性质](#第三页旋转与叉积的性质)
4. [手眼标定的最小二乘法推导](#第四页手眼标定的最小二乘法推导)
5. [极分解与最优旋转求解](#第五页极分解与最优旋转求解)

---

## 第一页：旋转运动学基础 (Rotation Kinematics)

### 微分方程与解

这一部分描述了点 $P$ 绕轴 $\omega$ 旋转的运动学方程。

这是一个微分方程 (differential equation)：

$$\dot{P} = \omega \times P$$

其解为：

$$P(t) = e^{[\omega]t} P(0)$$

### 旋转矩阵的指数映射

以单位角速度绕 $\omega$ 轴旋转 $\theta$ 角度（此处笔记中 $\omega$ 和 $\theta$ 概念互通）：

$$R = e^{[\omega]\theta}$$

根据泰勒级数展开 (Remember)：

$$e^{[\omega]\theta} = I + [\omega]\theta + \frac{([\omega]\theta)^2}{2!} + \frac{([\omega]\theta)^3}{3!} + \cdots$$

### 旋转群性质

$R \in SO(3)$ （特殊正交群），满足以下性质：

- $R^T R = I$ （正交性）
- $\det(R) = 1$ （行列式为1）

对应到指数映射形式：

$$R = e^{[\omega]\theta} \in SO(3)$$

---

## 第二页：相似变换与伴随性质 (Similarity Transformation & Adjoint Property)

### 手眼标定方程 (AX=XB)

这一页主要推导旋转矩阵的相似变换关系。

将旋转矩阵写成指数形式 $A = e^{[a]}$ 和 $B = e^{[b]}$：

$$AX = XB$$

由于对于旋转矩阵 $X \in SO(3)$，且对两边取对数 (log)：

$$[a] = X [b] X^T$$

### 反对称矩阵 (Skew-symmetric Matrix)

定义向量 $a$ 和 $b$ 的反对称矩阵形式（笔记中下标有些潦草，此处按标准形式补全）：

$$[a] = \begin{bmatrix}
0 & -a_z & a_y \\
a_z & 0 & -a_x \\
-a_y & a_x & 0
\end{bmatrix}$$

$$[b] = \begin{bmatrix}
0 & -b_z & b_y \\
b_z & 0 & -b_x \\
-b_y & b_x & 0
\end{bmatrix}$$

### 核心结论

通过上述推导得出向量旋转与矩阵共轭的关系：

$$[a] = X [b] X^T$$

这等价于向量形式：

$$a = X b$$

### 数学证明：矩阵指数的相似变换

页面底部证明了 $e^{X [b] X^T} = X e^{[b]} X^T$。

设 $M = X [b] X^T$，根据指数定义：

$$e^M = I + M + \frac{M^2}{2!} + \frac{M^3}{3!} + \cdots$$

展开项：

$$M^2 = (X [b] X^T)(X [b] X^T) = X [b]^2 X^T$$

$$M^3 = X [b]^3 X^T$$

提取公共项 $X$ 和 $X^T$：

$$e^M = X \left(I + [b] + \frac{[b]^2}{2!} + \frac{[b]^3}{3!} + \cdots\right) X^T = X e^{[b]} X^T$$

证毕。

---

## 第三页：旋转与叉积的性质 (Rotation & Cross Product Properties)

### 从矩阵共轭到向量旋转的推导

这一部分通过代数推导证明了"反对称矩阵的相似变换"等价于"向量的旋转"。

由上一页结论出发：

$$[a] = X [b] X^T$$

右乘 $X$：

$$[a] X = X [b]$$

将两边作用于任意向量 $v$：

$$[a] X v = X [b] v$$

利用叉积算子定义 $[a]v = a \times v$：

$$a \times (X v) = X (b \times v)$$

利用旋转矩阵对叉积的分配律（Distributive Property of Rotation over Cross Product）：

$$(X a) \times (X v) = X (a \times v)$$

结合上述两式：

$$a \times (X v) = (X a) \times (X v)$$

移项整理：

$$(a - X a) \times (X v) = 0$$

由于这对任意 $v$ 成立，且 $X$ 是满秩旋转矩阵，可得核心结论：

$$a = X b$$

### 叉积矩阵定义回顾

页面下半部分复习了叉积矩阵的构造以及旋转矩阵的性质。

向量定义：

$$v = \begin{bmatrix} v_x \\ v_y \\ v_z \end{bmatrix}$$

叉积矩阵 $[v]$ (或笔记中的 $[v]_\times$)：

$$[v] = \begin{bmatrix}
0 & -v_z & v_y \\
v_z & 0 & -v_x \\
-v_y & v_x & 0
\end{bmatrix}$$

旋转矩阵性质补充：

$$\det(X) = 1$$

(注意：仅当 $X \in SO(3)$ 时成立，若含反射则需变号)

---

## 第四页：手眼标定的最小二乘法推导 (Least-Squares Derivation for Hand-Eye)

### 问题定义

目标是求解未知旋转矩阵 $X$ （笔记中简写为 $X$），使得对于测量数据集 $\{(A_i, B_i)\}_{i=1}^n$：

$$A_i X = X B_i, \quad \forall i$$

### 优化目标函数

由于存在噪声，无法精确满足，因此构建最小二乘问题：

$$\min_X \sum_{i=1}^n \|A_i X - X B_i\|_F^2$$

### 展开目标函数

范数的平方可以写成向量与其转置的点积：

$$\|A_i X - X B_i\|_F^2 = \text{tr}[(A_i X - X B_i)^T (A_i X - X B_i)]$$

展开各项：

$$\text{tr}[X^T A_i^T A_i X - X^T A_i^T X B_i - B_i^T X^T A_i X + B_i^T X^T X B_i]$$

利用旋转矩阵的正交性 $X^T X = I$：

第一项 $\text{tr}[X^T A_i^T A_i X]$ 和最后一项 $\text{tr}[B_i^T B_i]$ 是常数（向量长度不随旋转改变），不影响优化结果，因此可以忽略。

### 简化优化问题

优化问题简化为最小化交叉项：

$$\text{Minimize} \quad \sum_{i=1}^n \text{tr}[-X^T A_i^T X B_i - B_i^T X^T A_i X]$$

因为结果是标量，标量的转置等于其自身：

$$\text{tr}[B_i^T X^T A_i X] = \text{tr}[(B_i^T X^T A_i X)^T] = \text{tr}[X^T A_i^T X B_i]$$

所以原式变为：

$$\text{Minimize} \quad -2 \sum_{i=1}^n \text{tr}[X^T A_i^T X B_i]$$

这等价于最大化正值：

$$\text{Maximize} \quad \sum_{i=1}^n \text{tr}[X^T A_i^T X B_i]$$

### 引入迹 (Trace) 的性质

利用迹的性质 $\text{tr}[AB] = \text{tr}[BA]$：

$$\text{tr}[X^T A_i^T X B_i] = \text{tr}[A_i^T X B_i X^T] = \text{tr}[A_i^T (X B_i X^T)]$$

代入求和公式：

$$\text{Maximize} \quad \sum_{i=1}^n \text{tr}[A_i^T X B_i X^T]$$

定义相关矩阵 $M$：

$$M = \sum_{i=1}^n A_i B_i^T$$

### 最终形式

问题转化为寻找旋转矩阵 $X$，使得 $\text{tr}[X^T M]$ 最大化。

$$\text{Maximize} \quad \text{tr}[X^T M]$$

(注：这通常通过对 $M$ 进行奇异值分解 SVD 来求解，即 Arun's Method 的基础步骤，这部分内容可能会在后续页面展开。)

---

## 第五页：极分解与最优旋转求解 (Polar Decomposition & Optimal Rotation)

### 极分解定义

将矩阵 $M$ 分解为正交矩阵和半正定矩阵的乘积 $M = UP$：

$$M = UP$$

其中：

- $P$ 是正定/半正定矩阵 (Positive semi-definite, eigenvalues $\geq 0$)
- $U$ 是酉矩阵/正交矩阵 (Unitary/Orthogonal matrix)

### 分解公式

$P$ 和 $U$ 的计算公式如下：

$$P = (M^T M)^{1/2}$$

$$U = M P^{-1}$$

### 迹与特征值的性质

回顾矩阵迹 (Trace) 的定义：

$$\text{tr}[M] = \sum_i \lambda_i$$ （特征值之和）

对于旋转矩阵 $X$，其 3 个特征值都在单位圆上：

$$\lambda_1 = 1, \quad \lambda_{2,3} = e^{\pm i\theta}$$ （共轭复数对）

只有当 $X = I$ (单位矩阵) 时，$\text{tr}[X] = 3$。

此时 $\text{tr}[X^T M]$ 取得最大值。

### 最大化目标函数

回到上一页的优化目标：

$$\text{Maximize} \quad \text{tr}[X^T M]$$

将 $M = UP$ 代入：

$$\text{Maximize} \quad \text{tr}[X^T U P]$$

由于 $P$ 是半正定矩阵 (PSD)，要使迹最大，需要旋转部分 $X^T U$ 尽可能接近单位矩阵。

### 最优解

最大值在 $X^T U = I$ 时取得。

由此解得 $X$：

$$X = U$$

代入 $U$ 的定义，得到最终的闭式解：

$$X = M (M^T M)^{-1/2}$$

*(注：这就是著名的 Kabsch 算法或 Arun's method 的核心步骤，通过 SVD 或极分解将相关矩阵 $M$ 投影到 $SO(3)$ 空间以求得最佳旋转。)*

---

## 总结

本笔记完整推导了手眼标定问题中旋转矩阵的求解方法：

1. **理论基础**：旋转矩阵的指数映射和相似变换
2. **核心关系**：$AX = XB$ 等价于 $a = Xb$（向量形式）
3. **优化方法**：最小二乘法将问题转化为最大化 $\text{tr}[X^T M]$
4. **求解算法**：通过极分解或SVD得到最优旋转矩阵 $X = U$，其中 $M = UP$ 是 $M$ 的极分解

---

**备注**：
- 为了保证数学公式的连贯性，部分手写模糊的地方（如矩阵下标）已根据标准数学定义进行了修正
- 本笔记主要关注旋转部分，平移部分的求解通常需要额外的约束条件
- 实际应用中，通常使用SVD方法：$M = U \Sigma V^T$，则 $X = U V^T$（需确保 $\det(X) = 1$）
