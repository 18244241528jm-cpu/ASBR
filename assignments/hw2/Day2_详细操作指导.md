# Day 2 详细操作指导：闭式解 + solveRx

**预计总时长**：约 4–5 小时  
**产出**：`axxb_closedform.m` + `solveRx.m` + 合成数据验证通过

---

## 上午：实现 axxb_closedform.m（约 1.5–2h）

### 1. 函数签名（必须严格匹配）

```matlab
function X = axxb_closedform(e_bh, e_sc)
% e_bh: 3×7 矩阵，3 次正向运动学测量，每行 [tx ty tz qx qy qz qw]
% e_sc: 3×7 矩阵，3 次 AR 标定板测量，每行 [tx ty tz qx qy qz qw]
% 返回: 4×4 齐次变换 X
```

**注意**：作业写的是 **3×7**（3 行 7 列），即 3 次测量。

### 2. 实现步骤

#### Step 1：7 元组 → 4×4 变换

若 Day 1 已写 `vec2transform`，可直接用。否则：

```matlab
function T = vec2transform(row)
    t = row(1:3)';
    q_ros = row(4:7);
    q_matlab = [q_ros(4), q_ros(1), q_ros(2), q_ros(3)];  % [qw qx qy qz]
    R = quat2rotm(q_matlab);
    T = [R, t; 0 0 0 1];
end
```

#### Step 2：3 次测量 → 2 对 (A, B)

| 配对 | A | B |
|------|---|---|
| (1,2) | $A_1 = E_{bh,1}^{-1} E_{bh,2}$ | $B_1 = E_{sc,1} E_{sc,2}^{-1}$ |
| (2,3) | $A_2 = E_{bh,2}^{-1} E_{bh,3}$ | $B_2 = E_{sc,2} E_{sc,3}^{-1}$ |

```matlab
E_bh_1 = vec2transform(e_bh(1,:));
E_bh_2 = vec2transform(e_bh(2,:));
E_bh_3 = vec2transform(e_bh(3,:));
E_sc_1 = vec2transform(e_sc(1,:));
E_sc_2 = vec2transform(e_sc(2,:));
E_sc_3 = vec2transform(e_sc(3,:));

A1 = inv(E_bh_1) * E_bh_2;
B1 = E_sc_1 * inv(E_sc_2);
A2 = inv(E_bh_2) * E_bh_3;
B2 = E_sc_2 * inv(E_sc_3);
```

#### Step 3：log_so3（旋转矩阵 → 轴角向量）

**重要**：Matlab 的 `logm` 针对一般矩阵，**不能直接用于 SO(3)**，需自写：

```matlab
function alpha = log_so3(R)
    theta = acos(max(-1, min(1, (trace(R) - 1) / 2)));
    if abs(theta) < 1e-10
        alpha = zeros(3, 1);
        return;
    end
    if abs(theta - pi) < 1e-10  % θ=π 特殊情况
        w = [R(2,3)-R(3,2); R(3,1)-R(1,3); R(1,2)-R(2,1)];
        if norm(w) < 1e-10
            alpha = [pi; 0; 0];
            return;
        end
        w = w / norm(w);
        alpha = pi * w;
        return;
    end
    W = (R - R') / (2 * sin(theta));
    omega = [W(2,3); W(3,1); W(1,2)];
    alpha = theta * omega;
end
```

#### Step 4：闭式解公式

$$\alpha_i = \log(R_{A_i}), \quad \beta_i = \log(R_{B_i})$$
$$\mathcal{A} = [\alpha_1 \quad \alpha_2 \quad \alpha_1 \times \alpha_2]$$
$$\mathcal{B} = [\beta_1 \quad \beta_2 \quad \beta_1 \times \beta_2]$$
$$R_X = \mathcal{A} \mathcal{B}^{-1}$$

```matlab
alpha1 = log_so3(A1(1:3, 1:3));
alpha2 = log_so3(A2(1:3, 1:3));
beta1  = log_so3(B1(1:3, 1:3));
beta2  = log_so3(B2(1:3, 1:3));

A_mat = [alpha1, alpha2, cross(alpha1, alpha2)];
B_mat = [beta1,  beta2,  cross(beta1,  beta2)];

R_X = A_mat * inv(B_mat);
```

#### Step 5：求 t_X 并组装 X

平移方程：$(R_{A_i} - I) t_X = R_X t_{B_i} - t_{A_i}$

用两组数据堆叠（2×3=6 行方程，3 个未知数）：

```matlab
RA1 = A1(1:3, 1:3); tA1 = A1(1:3, 4);
RA2 = A2(1:3, 1:3); tA2 = A2(1:3, 4);
RB1 = B1(1:3, 1:3); tB1 = B1(1:3, 4);
RB2 = B2(1:3, 1:3); tB2 = B2(1:3, 4);

C = [RA1 - eye(3); RA2 - eye(3)];
d = [R_X * tB1 - tA1; R_X * tB2 - tA2];
t_X = C \ d;

X = [R_X, t_X; 0 0 0 1];
```

### 3. log_so3 存放位置

可单独建 `log_so3.m`，或在 `axxb_closedform.m` 末尾以子函数形式定义。`solveRx` 也会用到，建议单独建文件或提取为共享函数。

---

## 下午：实现 solveRx.m（约 1.5–2h）

### 1. 函数签名

```matlab
function Rx = solveRx(alphas, betas)
% alphas: 3×N，每列 α_i = log(R_{A_i})
% betas:  3×N，每列 β_i = log(R_{B_i})
% 返回: 3×3 旋转矩阵 R_X
```

### 2. 公式

$$M = \sum_{i=1}^{N} \beta_i \alpha_i^T$$
$$R_X = (M^T M)^{-1/2} M^T$$

或等价地，用 **SVD 极分解**：$M = U \Sigma V^T$，则 $R_X = U V^T$。

### 3. 实现方式 A：SVD（推荐，数值更稳）

```matlab
function Rx = solveRx(alphas, betas)
    M = betas * alphas';
    [U, ~, V] = svd(M);
    Rx = U * V';
    % 确保 det(Rx) = +1（proper rotation）
    if det(Rx) < 0
        V(:, 3) = -V(:, 3);
        Rx = U * V';
    end
end
```

### 4. 实现方式 B：$(M^T M)^{-1/2} M^T$

```matlab
MtM = M' * M;
[Q, Lambda] = eig(MtM);
Lambda_sqrt_inv = diag(1 ./ sqrt(diag(Lambda)));
MtM_sqrt_inv = Q * diag(Lambda_sqrt_inv) * inv(Q);
Rx = (MtM_sqrt_inv * M')';
```

注意特征值可能接近 0，需加小量或改用 SVD。

### 5. alphas、betas 的构造

在 `axxb.m` 中会用到。对 N 组 (A_i, B_i)，每对得到一列：

```matlab
alphas = [alpha1, alpha2, ..., alphaN];  % 3×N
betas  = [beta1,  beta2,  ..., betaN];   % 3×N
```

---

## 晚上：验证（约 0.5–1h）

### 1. 用 generatedata 验证闭式解

```matlab
[e_bh, e_sc, X_true] = generatedata(5);
e_bh_3 = e_bh(1:3, :);   % 取前 3 次测量
e_sc_3 = e_sc(1:3, :);

X_est = axxb_closedform(e_bh_3, e_sc_3);

err_R = norm(X_est(1:3,1:3) - X_true(1:3,1:3), 'fro');
err_t = norm(X_est(1:3,4) - X_true(1:3,4));
fprintf('闭式解 - 旋转误差: %e, 平移误差: %e\n', err_R, err_t);
assert(err_R < 1e-10 && err_t < 1e-10, '闭式解验证失败');
```

### 2. 验证 solveRx

```matlab
[e_bh, e_sc, X_true] = generatedata(10);

% 构造 A_i, B_i（相邻帧配对）
N = size(e_bh, 1);
alphas = [];
betas = [];
for i = 1:N-1
    E_bh_i = vec2transform(e_bh(i,:));
    E_bh_j = vec2transform(e_bh(i+1,:));
    E_sc_i = vec2transform(e_sc(i,:));
    E_sc_j = vec2transform(e_sc(i+1,:));
    A = inv(E_bh_i) * E_bh_j;
    B = E_sc_i * inv(E_sc_j);
    alphas = [alphas, log_so3(A(1:3,1:3))];
    betas  = [betas,  log_so3(B(1:3,1:3))];
end

R_X_est = solveRx(alphas, betas);
R_X_true = X_true(1:3, 1:3);
err_R = norm(R_X_est - R_X_true, 'fro');
fprintf('solveRx - 旋转误差: %e\n', err_R);
assert(err_R < 1e-10, 'solveRx 验证失败');
```

### 3. 若误差较大，排查顺序

1. **log_so3**：用简单例子验证，如 R = Rz(90°)，log(R) 应为 [0, 0, π/2]
2. **A、B 构造**：确认 $A = E_{bh,i}^{-1} E_{bh,j}$，$B = E_{sc,i} E_{sc,j}^{-1}$
3. **极分解**：检查 det(Rx) 是否为 +1

---

## Day 2 完成检查清单

- [ ] `axxb_closedform(e_bh_3, e_sc_3)` 输入 3×7，输出 4×4
- [ ] 3 次测量正确构造 2 对 (A1,B1)、(A2,B2)
- [ ] `log_so3` 能正确处理 θ≈0 和 θ=π
- [ ] `solveRx(alphas, betas)` 输入 3×N，输出 3×3 旋转矩阵
- [ ] 用 generatedata 验证：闭式解和 solveRx 误差 < 1e-10

---

## 常见坑

1. **logm 不能用**：SO(3) 的对数映射需用轴角公式，不是矩阵对数
2. **极分解 det**：SVD 得到的 U*V' 可能 det=-1，需翻转一列修正
3. **axxb_closedform 输入**：3×7 表示 3 行 7 列，即 3 次测量
