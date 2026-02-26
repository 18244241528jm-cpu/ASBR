# Day 3 详细操作指导：solveTx + axxb + 自测

**预计总时长**：约 4–5 小时  
**产出**：`solveTx.m` + `axxb.m` + 完整合成数据自测通过

---

## 上午：实现 solveTx.m（约 1–1.5h）

### 1. 函数签名（严格匹配作业）

```matlab
function tx = solveTx(RA, tA, RB, tB, RX)
% RA: 3×3×N，所有 R_{A_i}
% tA: 3×N，所有 t_{A_i}
% RB: 3×3×N，所有 R_{B_i}
% tB: 3×N，所有 t_{B_i}
% RX: 3×3，已求得的 R_X
% 返回: 3×1 平移向量 t_X
```

### 2. 公式

对每组 $i$：
$$(R_{A_i} - I) \, t_X = R_X \, t_{B_i} - t_{A_i}$$

堆叠成 $C \, t_X = d$：
$$\begin{pmatrix} R_{A_1} - I \\ R_{A_2} - I \\ \vdots \\ R_{A_N} - I \end{pmatrix} t_X = \begin{pmatrix} R_X t_{B_1} - t_{A_1} \\ R_X t_{B_2} - t_{A_2} \\ \vdots \\ R_X t_{B_N} - t_{A_N} \end{pmatrix}$$

最小二乘解：$t_X = C \backslash d$ 或 $t_X = (C^T C)^{-1} C^T d$

### 3. 实现

```matlab
function tx = solveTx(RA, tA, RB, tB, RX)
    N = size(RA, 3);
    C = [];
    d = [];
    for i = 1:N
        C = [C; RA(:,:,i) - eye(3)];
        d = [d; RX * tB(:,i) - tA(:,i)];
    end
    tx = C \ d;
end
```

或用 `mldivide`：`tx = C \ d` 自动处理超定/适定。

### 4. 维度检查

- `RA(:,:,i)` 为 3×3
- `tA(:,i)`、`tB(:,i)` 为 3×1
- `C` 为 3N×3，`d` 为 3N×1
- `tx` 为 3×1

---

## 下午：实现 axxb.m（约 1.5–2h）

### 1. 函数签名

```matlab
function X = axxb(e_bh, e_sc)
% e_bh: N×7，N 次正向运动学测量
% e_sc: N×7，N 次 AR 标定板测量
% 返回: 4×4 手眼标定变换 X
```

### 2. 实现流程

```
e_bh, e_sc (N×7)
    ↓ 解析 + 构造 A_i, B_i
RA, tA, RB, tB (3×3×N, 3×N)
    ↓ log_so3
alphas, betas (3×N)
    ↓ solveRx
R_X (3×3)
    ↓ solveTx
t_X (3×1)
    ↓ 组装
X = [R_X, t_X; 0 0 0 1]
```

### 3. 构造 A_i、B_i 的配对方式

**方式 A：相邻帧**（推荐）
- 配对 (1,2), (2,3), ..., (N-1,N)
- 共 N-1 对

**方式 B：相对第一帧**
- 配对 (1,2), (1,3), ..., (1,N)
- 共 N-1 对

作业未强制指定，两种均可。下面以**相邻帧**为例。

### 4. 完整实现骨架

```matlab
function X = axxb(e_bh, e_sc)
    N = size(e_bh, 1);
    
    % 1. 构造 A_i, B_i（相邻帧）
    RA = zeros(3, 3, N-1);
    tA = zeros(3, N-1);
    RB = zeros(3, 3, N-1);
    tB = zeros(3, N-1);
    alphas = [];
    betas = [];
    
    for i = 1:N-1
        E_bh_i = vec2transform(e_bh(i,:));
        E_bh_j = vec2transform(e_bh(i+1,:));
        E_sc_i = vec2transform(e_sc(i,:));
        E_sc_j = vec2transform(e_sc(i+1,:));
        
        A = inv(E_bh_i) * E_bh_j;
        B = E_sc_i * inv(E_sc_j);
        
        RA(:,:,i) = A(1:3, 1:3);
        tA(:,i)   = A(1:3, 4);
        RB(:,:,i) = B(1:3, 1:3);
        tB(:,i)   = B(1:3, 4);
        
        alphas = [alphas, log_so3(RA(:,:,i))];
        betas  = [betas,  log_so3(RB(:,:,i))];
    end
    
    % 2. 求 R_X
    R_X = solveRx(alphas, betas);
    
    % 3. 求 t_X
    t_X = solveTx(RA, tA, RB, tB, R_X);
    
    % 4. 组装 X
    X = [R_X, t_X; 0 0 0 1];
end
```

### 5. 依赖函数

需确保以下函数可用（同目录或 path 中）：
- `vec2transform`
- `log_so3`
- `solveRx`
- `solveTx`

---

## 晚上：完整自测（约 1h）

### 1. 主验证脚本

```matlab
% test_axxb.m
[e_bh, e_sc, X_true] = generatedata(20);

X_est = axxb(e_bh, e_sc);

% 旋转误差
err_R = norm(X_est(1:3,1:3) - X_true(1:3,1:3), 'fro');
% 平移误差
err_t = norm(X_est(1:3,4) - X_true(1:3,4));

fprintf('旋转误差 (Frobenius): %e\n', err_R);
fprintf('平移误差 (Euclidean): %e\n', err_t);

assert(err_R < 1e-10, '旋转误差过大');
assert(err_t < 1e-10, '平移误差过大');
disp('✓ 所有验证通过');
```

### 2. 交叉验证 AX=XB

```matlab
% 抽查几对 (A_i, B_i)
for i = 1:min(5, size(e_bh,1)-1)
    E_bh_i = vec2transform(e_bh(i,:));
    E_bh_j = vec2transform(e_bh(i+1,:));
    E_sc_i = vec2transform(e_sc(i,:));
    E_sc_j = vec2transform(e_sc(i+1,:));
    A = inv(E_bh_i) * E_bh_j;
    B = E_sc_i * inv(E_sc_j);
    err = norm(A*X_est - X_est*B, 'fro');
    assert(err < 1e-10, 'AX=XB 验证失败');
end
disp('✓ AX=XB 验证通过');
```

### 3. 与闭式解对比

```matlab
e_bh_3 = e_bh(1:3,:);
e_sc_3 = e_sc(1:3,:);
X_closed = axxb_closedform(e_bh_3, e_sc_3);
X_full   = axxb(e_bh, e_sc);

% 两者应接近（闭式解用 3 点，axxb 用全部 N 点）
fprintf('闭式解 vs 全量解 旋转差: %e\n', norm(X_closed(1:3,1:3)-X_full(1:3,1:3), 'fro'));
```

### 4. 无噪声数据预期

- 旋转误差 < 1e-10
- 平移误差 < 1e-10
- 若达不到，按 Day 2 排查顺序检查 log_so3、solveRx、A/B 构造

---

## Day 3 完成检查清单

- [ ] `solveTx(RA, tA, RB, tB, RX)` 参数维度正确：RA 3×3×N，tA 3×N 等
- [ ] `axxb(e_bh, e_sc)` 能处理任意 N≥2 的数据
- [ ] 用 generatedata(20) 验证：X_est 与 X_true 误差 < 1e-10
- [ ] AX=XB 抽查验证通过
- [ ] 所有 .m 文件在 `assignment2/matlab/` 下

---

## 常见坑

1. **quat2rotm 批量**：`quat2rotm([q1; q2; ...])` 返回 3×3×N，注意维度
2. **tA、tB 列向量**：`tA(:,i)` 为 3×1，与 `RX * tB(:,i)` 匹配
3. **N=2 边界**：N=2 时只有 1 对 (A,B)，solveRx 仍可解，但需至少 2 列
