# 第 6 讲（扩展版）：`search_accessibility` 与 `search_departability` —— 把起终点接上路网（小白向）

> **课程**：EN.601.663 Assignment 3 · **分值**：各 **1 分**（共 2 分）  
> **前置**：[第 4 讲](./lecture04_make_vertices顶点采样.md) **V**、[第 5 讲](./lecture05_make_edges连边.md) **E** 与边上 **interpolate + 碰撞** 检验已通。  
> **目标**：从 **规划起点构型** 连到某个 **vs ∈ V**，从 **规划终点构型** 连到某个 **vf ∈ V**；边的合法性与 **make_edges** 相同。  
> **诚信**：思路与伪代码仅供学习；**实现代码须独立完成**。

---

## 0. 在实现链中的位置

| 维度 | 说明 |
|------|------|
| **依赖** | [第 5 讲](./lecture05_make_edges连边.md) 已有 **V** 与 **adj**；[第 3 讲 §0.1](./lecture03_读懂骨架与PRM对应.md) 步骤 **③④**。 |
| **本讲交付物** | 索引 **vs**、**vf**；并保证 **start–vs**、**vf–goal** 经 **同一 `edge_valid`** 检验。 |
| **下游** | [第 7 讲](./lecture07_search_path图上最短路.md) 需要 **vs、vf** 在 **同一连通分量** 内。 |
| **验收** | 在 **make_edges 已通** 的前提下，简单场景能 **找到 vs/vf**；日志可区分 **接不上** vs **图上不通**。 |

**逻辑提示**：若 **② 邻接表** 错了，本讲再巧也 **接不上**；调试时先确认 **make_edges** 与 **edge_valid** 无误。

---

## 1. 为什么需要这两步？

经典 PRM 里，**路网上的点** 是随机采样的 **V**，**起始姿态** 和 **目标姿态** 一般是 **用户当前给的**，**不会** 恰好等于某个已采样的顶点。

因此需要：

- **Accessibility（可达路网）**：在 **start** 与 **某个 vs** 之间建立一条 **无碰撞** 的「局部连接」。  
- **Departability（可离路网）**：在 **goal** 与 **某个 vf** 之间同样建立 **无碰撞** 连接。

**与 Week05 PPT 的对应**：**RoadMap Definition** 幻灯把这三件事写进同一张定义里 —— **Accessibility / Departability / Connectivity**；本讲前两步即前两者，**Connectivity** 在 **[第 7 讲](./lecture07_search_path图上最短路.md)**。PPT 也说明查询时通常把 **start、goal** 当作 **与普通顶点一样去连**（同一套局部连接逻辑）。

之后 **`search_path`** 只在 **图内部** 找 **vs → vf**；完整轨迹再拼成 **start → … → goal**（具体拼接是否在 `solve` 里完成，以你的骨架为准）。

---

## 2. 课堂 digest：`week06.pdf`（接续 Week05，重点在「查询」与难例）

你本地的 **`week06.pdf`** 与 **`week05.pdf`** 同属 *Sampling-Based Motion Planning* 系列；**前约 31 页** 与 week05 **内容重叠**（路图、PRM 主流程、距离、局部规划器、概率完备性梗概）。**从「Sampling Strategies」起**（约第 32 页后）是 **week06 多出来的部分**，和 **本讲（把 start/goal 接上路网）**、以及 **专家场景为什么难** 关系最直接。下面按幻灯顺序 **压缩咀嚼**（材料署名同前：**Hager / LaValle / Plaku**）。

### 查询阶段：PPT 怎么谈 start / goal？（与 accessibility / departability 对齐）

**Some Minor Issues** 一节（week05/06 均有）写明：要做一次 **查询**，必须：

1. 把 **起点、终点构型** **接到路网上** —— 「**通常就把它们当成节点，用** **和建图时相同的办法** **去做连接**」（原文大意：*treat like nodes and perform same algorithm*）。  
2. 在 **得到的图** 上做 **图搜索**。  
3. 如需，可对路径 **略作平滑**。

这正是你在代码里拆成的：**`search_accessibility` / `search_departability`**（接线）+ **`search_path`**（搜索）。**本讲对应第 1 步**。

### PPT 补充：查询还能「反哺」路图（了解即可）

同一节还提到：**查询过程也可以用来往图里 **再加节点****（例如失败后把沿路有用的点并入路图）。作业骨架 **一般不要求** 做这种 **在线扩展**；若你实现 **外层重试**，本质上已在用「多次查询、多次重采样」逼近成功，与这种精神相近。

### 窄通道（narrow corridor）与「均匀采样不够用」（week06 扩展核心）

**Sampling Strategies** 指出：

- **窄通道问题**：在 **均匀随机** 下，点落在 **通道内部** 的概率 **极低**（与 **联合可见域面积** 等有关）；因此 **路网上** 在通道里 **常常很稀**。  
- 幻灯还列举 **其它采样思路**（了解名词即可）：**Bridge**（两端碰、中间空）、**障碍附近采样**、**OBPRM / Gaussian OBPRM**、**膨胀障碍** 等 —— 都是为了 **提高难区的样本密度**。

**与作业的关系**：你实现的是 **标准 PRM + 拒绝采样**；**Bugatti 车门—车内** 正是典型 **窄通道**（详见 **[第 9 讲](./lecture09_专家挑战与提交.md)**）。**调大 n、k、m、重试**（**[第 8 讲](./lecture08_联调与重试.md)**）是在 **不改动采样策略** 的前提下最常见的应对；**不是** 你漏写了 Bridge 就一定错。

### Single-query：RRT / EST 与 PRM 的对比（本作业为何是 PRM）

week06 后半 **What about Single Queries?** / **Single Query Planners**：

- **EST、RRT** 等 **单次查询** 建树，常 **从 start（和/或 goal）生长**，适合 **单次** 运动；**非完整** 地先离线建大图。  
- **PRM** 偏 **multi-query**：先建 **路图**，再多次 **查询**。

课程作业要求你在 MoveIt 里实现 **PRM 规划器**，与 **路图 + 查询** 结构一致；**不必** 在本作业里实现 RRT。

### 分析部分：失败与哪些几何因素有关？（与联调心照不宣）

**Analysis of PRM** 重申：若 **a、b** 在 \(Q_{\mathrm{free}}\) 内 **存在** 可行路径，PRM **失败概率** 与 **路径长度**、路径到障碍的 **最小间隙（clearance）**、**路图样本数** 等有关。**Understanding Complexity / Expansiveness**（扩张性 \((\varepsilon,\alpha,\beta)\)）从 **空间结构** 解释「为什么有的环境 **特别难**」—— **窄、几乎不可扩张** 的区域与 **窄通道** 同类现象。**浏览即可**，证明不要求掌握。

---

## 3. 与 PDF 条目的对应

| PDF / 课程用语 | 常见函数名 | 含义 |
|----------------|------------|------|
| accessibility | `search_accessibility` | start 接到 **vs ∈ V** |
| departability | `search_departability` | goal 接到 **vf ∈ V** |

**注意**：有的骨架把 **start / goal** 存成 **`RobotState` 指针** 或 **关节向量**；读头文件注释，**类型与拷贝方式** 与 **V 中元素** 一致即可。

---

## 4. 核心算法：近邻 + 与 make_edges 相同的边检验

对 **accessibility**（departability **对称**，把 start 换成 goal 即可）：

```text
输入：start 构型，顶点集 V
输出：成功则得到某个索引 vs 且「start–vs 边合法」；失败则报告失败

1. 对每个顶点 i，计算 d(start, qi)   // 与 make_edges 相同的关节距离
2. 按距离升序排序，得到索引列表 order
3. 对 order 中前 m 个候选 i（m 可配置，如 10～50）:
       if edge_valid(start, V[i]):   // 与 make_edges 相同：interpolate + state_collides
           vs ← i
           记录这条「临时边」供路径拼接使用
           返回成功
4. 若 m 个都失败：返回失败（或由 solve 重试整图）
```

**`edge_valid`**：应 **复用** 你在 **`make_edges`** 里写的逻辑（抽成 **私有成员函数** 最省事），保证 **Δ / 步数** 与路网边 **一致**，避免「网上能走、接线穿模」。

---

## 5. 选 m（尝试多少个最近顶点？）

| m 太小 | start 附近恰好没有「看得见」的顶点 → 易失败 |
|--------|-----------------------------------------------|
| m 太大 | 碰撞检测次数增加；通常仍比不过 **增大 n 或 k** 有效 |

**建议**：与 **k** 同量级或略大；失败时在 **`solve` 外层重试** 时可 **增大 m** 或 **重采 V**。

---

## 6. 需要存下来什么？（与 `search_path`、solve 衔接）

除 **索引 vs、vf** 外，通常还要能 **重建路径**：

- **方案 A**：只存 **vs、vf**，假设 **search_path** 返回 **中间顶点序列**，最后在 **solve** 里拼接：  
  **start → vs → … → vf → goal**（其中 start→vs、vf→goal 用 **插值细分** 或骨架提供的轨迹填充方式）。  
- **方案 B**：在 accessibility / departability 里把 **整条局部边** 离散成 **构型序列** 存进成员变量，最后 **拼接**。

**以骨架为准**：看 **`solve` 末尾** 如何把 **`RobotTrajectory`** 或路径消息填好；若已有 **从若干 waypoints 生成轨迹** 的辅助函数，你的输出应 **喂得进** 那个接口。

---

## 7. 对称性：departability 实现要点

- **距离**：**d(goal, qi)**。  
- **检验**：**edge_valid(goal, V[j])**，注意 **goal** 若来自 IK，可能 **多解**；骨架通常已固定 **一个 goal 状态**。  
- **失败**：不一定是 goal「在障碍里」，也可能是 **离最近的路网点都穿不过窄缝** —— 与 **图不连通** 症状类似，可用 **日志** 区分（见第 8 讲联调）。

---

## 8. start / goal 与 V 中顶点重复？

若 **start** 恰好非常接近 **某顶点 qi**，**d** 很小，**edge_valid(start, qi)** 应 **极易通过**（几乎退化线段）。  

一般 **不必** 把 start **强行插入 V**，除非骨架要求；多数实现 **保持 V 仅为采样集** 更清晰。

---

## 9. 伪代码：两段合成（语言无关）

```text
function try_connect(q_from, V, m):
    candidates ← 按 d(q_from, qi) 排序的顶点下标
    for i in candidates[0 .. m-1]:
        if edge_valid(q_from, V[i]):
            return (成功, i)
    return (失败, -1)

accessibility:
    (ok, vs) ← try_connect(start, V, m_start)
    if not ok: 失败

departability:
    (ok, vf) ← try_connect(goal, V, m_goal)
    if not ok: 失败
```

**无向图注意**：**search_path** 用的邻接表 **不含** start/goal 节点时，**vs、vf** 只是 **索引**；拼接路径时 **不要忘记** 两段 **悬边**。

---

## 10. 本讲动手清单

- [ ] 抽出 **`edge_valid(a, b)`**（或同名），与 **make_edges** 共用 **同一套插值步长**。  
- [ ] **accessibility**：按距 **start** 排序，尝试前 **m** 个顶点。  
- [ ] **departability**：对 **goal** 同理。  
- [ ] 失败时 **明确返回** 或设标志，让 **`solve`** 能 **整体失败** 而非未定义行为。  
- [ ] 简单场景：**Plan** 在 **实现 search_path 之前** 仍可能失败；至少 **断点/日志** 可看到 **vs 或 vf 被找到**。

---

## 11. 自检问答题

1. **accessibility** 失败时，更优先 **增大 m** 还是 **增大 n（顶点数）**？各解决什么问题？  
2. 若 **make_edges** 里 **Δ 很大** 而 **accessibility** 里 **Δ 很小**，可能出现什么 **不一致**？  
3. **goal 在自由空间** 但 **departability** 仍失败，说明 **路网** 可能有什么缺陷？

---

## 12. 常见误区

- **误区 1**：对 **start–vs** 只测 **两个端点** —— 与 **make_edges** 同样会 **穿模**。  
- **误区 2**：**vs、vf** 存了，但 **`search_path` 只搜图**，路径里 **漏掉 start→vs** —— 动画从错误姿态开始。  
- **误区 3**：**departability** 里误用 **start** 的距离排序 —— 复制粘贴类 bug。  
- **误区 4**：**m = |V|** 且 **|V| 上万** —— 每次 Plan **极慢**；应限制 m 或优化数据结构。

---

## 13. 下一讲预告

**[第 7 讲：search_path 图上最短路](./lecture07_search_path图上最短路.md)** —— 在邻接表上做 **BFS / Dijkstra**，得到 **vs → vf** 的顶点序列，并与 **start、goal** 在 `solve` 中拼接为完整路。

---

## 参考

- 课程 PDF §4.1：`search accessibility`、`search departability`。  
- 课堂幻灯 **`week06.pdf`**（在 **`week05.pdf`** 基础上扩展；**§2** 消化其后半），与 **查询阶段、窄通道、RRT/PRM 对比** 对照。  
- PRM：query phase、connection to roadmap。

---

*讲义版本：扩展第 6 讲 · 与课程 PDF §4.1 及 `week06.pdf` 对齐 · 仅供学习梳理使用。*
