# 第 9 讲（扩展版）：专家挑战与提交 —— Bugatti 场景、`assignment3.tgz`、扣分项（小白向）

> **课程**：EN.601.663 Assignment 3  
> **前置**：[第 8 讲](./lecture08_联调与重试.md) 已能 **稳定 Plan** 简单障碍；**PRM 六步** 已实现。  
> **目标**：理解 **+1 分专家挑战** 的判定方式；按 PDF 操作 **Bugatti** 场景；**正确打包**并在 **干净目录** 验证编译；**避免** 提交扣分与学术诚信问题。  
> **诚信**：以下仅为流程与检查清单；**代码须为你本人撰写**。

---

## 0. 专家挑战在评什么？（PDF §4.2）

| 项目 | 要求（原文大意） |
|------|------------------|
| **成功率** | 至少 **50%**：**4 次**尝试中 **≥2 次** 成功 |
| **规划时间** | 「可接受」；**合理机器**上约 **&lt; 30 秒**（PDF 举例 **Wyman 170**） |
| **编译** | 评分用 **`colcon build`**，**不** 特意启用 **Release** 等优化开关来加速 |

**+1 分** 是 **额外** 的；主体 **9 分** 仍来自 PRM 六步实现。专家挑战 **拿不到** 也应先保证 **能交、能编、主体能跑**。

---

## 0.5 提交前：对照实现总览做一次闭环（必做）

以下与 **[第 3 讲 §0](./lecture03_读懂骨架与PRM对应.md)** 中 **步骤 ①～⑥** 一一对应；**全部满足** 再考虑 Bugatti 与打包。

| 检查项 | 标准（不满足则先回退到对应讲次） |
|--------|-----------------------------------|
| **① `make_vertices`** | 典型场景 **\|V\|>0**，且采样顶点 **经** `state_collides` **为自由** → [第 4 讲](./lecture04_make_vertices顶点采样.md) |
| **② `make_edges`** | **adj** 已建、**每次规划前清空**；边上 **Δ 足够细**、无穿模 → [第 5 讲](./lecture05_make_edges连边.md) |
| **③④ 接线** | **vs、vf** 存在；**`edge_valid` 与 ② 完全相同** → [第 6 讲](./lecture06_accessibility_departability.md) |
| **⑤ `search_path`** | **vs、vf** 连通则出序列，**不连通** 不崩溃 → [第 7 讲](./lecture07_search_path图上最短路.md) |
| **⑥ `solve` 收尾** | **start→vs→…→vf→goal** 顺序正确，MoveIt **能显示连续轨迹** |
| **联调** | **重试 / 日志 / n、k、Δ、m** 有章法 → [第 8 讲](./lecture08_联调与重试.md) |

**逻辑顺序**：先保证 **简单障碍 Plan 稳定**，再开 **Bugatti**；先 **`colcon build` 通过**，再打 **`assignment3.tgz`**（见 §5）。

---

## 1. 为什么 Bugatti 比「一块板子」难得多？

- **窄通道（narrow passage）**：随机采样点 **落在通道内** 的概率 **极低**，路网在 **车门—车内** 区域 **稀疏**（课堂 **`week06.pdf`** 中 **Sampling Strategies / narrow corridor** 与 **[第 6 讲 §2](./lecture06_accessibility_departability.md)** 的说明一致）。  
- **几何复杂**：车身 mesh **遮挡多**，**边上碰撞检测** 更费时间。  
- **目标刁**：末端要伸到 **方向盘附近**（PDF：指尖约 **10 cm** 在轮后），**可行域** 小。

调参方向与 [第 8 讲](./lecture08_联调与重试.md) 一致：**n、k、m、Δ、R** 折中；必要时 **略放松** 插值步数以换时间（在 **不穿模** 前提下）。

---

## 2. 加载场景：Import `bugatti.scene`

按 PDF：

1. RViz MoveIt 面板 → **Scene Objects**（或等价）。  
2. 点击 **Import**（导入场景文件，而非单 mesh）。  
3. 在文件对话框中选择包内：  
   **`assignment3/meshes/bugatti.scene`**  
   （路径以你工作空间实际位置为准。）

导入后 **调整视角** 看到车身与车门；再设 **Planning** 里的目标到 **方向盘附近**。

---

## 3. 「二次 Publish」：碰撞检测异常时

PDF **经验**：第一次导入后，**碰撞可能未正确更新**；可 **再 Publish 一次** 场景。

**可操作办法**（任选其一，与 PDF 一致）：

- **加一个小 box 再删掉**，使 **Publish** 可点；或  
- **把车（或场景）微移** 再移回，触发更新；然后 **Publish Scene**。

若规划「穿车」或 **明显不该碰却碰** —— 先怀疑 **场景未同步**，再怀疑算法。

---

## 4. 自测专家挑战的推荐流程

1. **固定** 同一 **Start**（或每次从相似起始），**Goal** 按作业描述摆到 **方向盘任务**（可拍照记录位姿，减少变量）。  
2. 连续点 **Plan** **4 次**，记录 **成功/失败** 与 **大致耗时**（或看日志）。  
3. 目标：**≥2 次成功**，且单次 **&lt; ~30s**（按你机器与 PDF 要求理解）。  
4. 若 **4 次全败**：回到 [第 8 讲](./lecture08_联调与重试.md) 调 **n、k、Δ、m、R**；**不要** 只改随机种子赌运气。  
5. 若 **成功但超时**：减少 **单次** 工作量（略降 **n** 或 **k**、**略增 Δ**、**限制 R**），或优化 **实现常数**（避免重复分配、过密日志）。

---

## 5. 提交物：`assignment3.tgz`（PDF §4.3）

PDF 给定命令（路径按你机器调整；核心是 **`-C` 目录** 与 **最后一级包名**）：

```bash
tar cvfz ~/assignment3.tgz -C ~/sbr_workspace/src/sbr assignment3
```

**含义**：

- 压缩包 **顶层** 应出现文件夹 **`assignment3/`**（里面是 `CMakeLists.txt`、`package.xml`、`src/` 等）。  
- **不要** 多一层 `sbr/assignment3` 或 **不要** 只打了 `src` 里错误路径 —— PDF 写明 **打包不当扣 1 分**。

**提交前自检**：

1. 把 `assignment3.tgz` 拷到 **临时空目录**。  
2. 解压后按课程要求放入 **新工作空间** 的 `src/`。  
3. **`colcon build --packages-select assignment3`** **必须成功** —— **构建失败扣 1 分**。

---

## 6. 迟交与其它扣分（PDF Deliverables / §4.3）

- **迟交**：**每天扣 10%（约 1 分）**（以 PDF 与课程政策为准）。  
- **构建失败**：**−1**  
- **未按指定方式打包**：**−1**

---

## 7. 学术诚信（PDF 原文要求，须严肃对待）

提交即表示你已阅读 **JHU 学术诚信政策**，且代码 **100% 本人撰写。

- **不允许** 使用 **代码生成/补全类软件**（包括但不限于用 AI 直接生成作业提交代码）。  
- **不得** 对外分发作业（网络、同学、以后学期的学生等）。

本系列讲义为 **学习梳理**；**实现须独立完成**。若课程另有口头说明，**以教师与 PDF 为准**。

---

## 8. 本讲动手清单

- [ ] 成功 **Import** `bugatti.scene`，必要时 **二次 Publish**。  
- [ ] 按 PDF 描述设置 **方向盘附近** 目标，自测 **4 次 Plan** 统计成功率与时间。  
- [ ] 用 PDF 的 **`tar`** 命令生成 **`assignment3.tgz`**，检查 **顶层目录名**。  
- [ ] 在 **干净目录** 解压并 **`colcon build`** 通过。  
- [ ] 截止前 **Canvas**（或指定入口）提交，确认 **文件名与截止时间**。

---

## 9. 自检问答题

1. 为什么专家挑战强调 **4 次里 2 次成功**，而不是「一次成功即可」？  
2. **Release 编译** 更快，为什么评分以 **`colcon build` 默认** 为准仍合理？  
3. 解压别人的包发现 **没有 `package.xml`**，说明什么？

---

## 10. 系列讲义索引（第 1～9 讲）

| 讲次 | 文件 | 主题 |
|------|------|------|
| 1 | [lecture01_环境与MoveIt入门](./lecture01_环境与MoveIt入门.md) | ROS 2、工作空间、MoveIt 概念 |
| 2 | [lecture02_MoveIt界面与场景规划](./lecture02_MoveIt界面与场景规划.md) | RViz、Publish Scene、Plan |
| 3 | [lecture03_读懂骨架与PRM对应](./lecture03_读懂骨架与PRM对应.md) | `solve`、六函数；**含 `week05.pdf` 路图三要素与 PRM 对照（§1.5）** |
| 4 | [lecture04_make_vertices顶点采样](./lecture04_make_vertices顶点采样.md) | `make_vertices` |
| 5 | [lecture05_make_edges连边](./lecture05_make_edges连边.md) | `make_edges` |
| 6 | [lecture06_accessibility_departability](./lecture06_accessibility_departability.md) | accessibility / departability；**含 `week06.pdf` 查询阶段与窄通道（§2）** |
| 7 | [lecture07_search_path图上最短路](./lecture07_search_path图上最短路.md) | `search_path` |
| 8 | [lecture08_联调与重试](./lecture08_联调与重试.md) | 重试、调参、日志 |
| 9 | 本讲 | 专家挑战、打包、诚信 |
| **附录** | [supplement_week08_Kalman滤波与定位SLAM](./supplement_week08_Kalman滤波与定位SLAM.md) | 消化 **`week08.pdf`**（KF / 定位 / SLAM），与 PRM **并列** |

---

## 参考

- 课程 PDF：`assignment3-663.pdf` §4.2、§4.3、Deliverables、迟交说明。  
- MoveIt：场景导入与 Planning Scene（Humble 文档）。

---

*讲义版本：扩展第 9 讲 · 与课程 PDF §4.2–4.3 对齐 · 系列完结。*
