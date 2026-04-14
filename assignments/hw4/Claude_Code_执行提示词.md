# HW4 发给 Claude Code 的分段提示词（执行用）

**用途**：把下面各段 **按顺序** 复制到 Claude Code，让它在你本机的 **assignment4 工作区** 里改代码、跑构建与仿真相关命令。  
**说明**：作业要求 **100% 本人提交代码**；提示词用于 **在你已理解 PDF 要求的前提下** 加速实现与排错，请自行核对推导与学术诚信政策。

**路径约定**（按你机器改）：

- 作业源码常在：`~/sbr_workspace/src/sbr/assignment4/`（或 `sbr-a4.sh` 实际解压位置）。
- 本仓库 `assignments/hw4/` 只有学习计划，**不含** 课程下发的 `assignment4` 包时，先在 Claude Code 里 **打开含该包的文件夹** 为工作区根目录。

---

## 使用前（一次性发给 Claude Code）

```text
You are working in my ROS 2 Humble workspace on Assignment 4 (Jackal 6D EKF with BFL).

Rules:
- ONLY modify files: ekf_models.hpp and ekf_models.cpp (per course PDF). Do not refactor unrelated packages.
- After code changes: run `colcon build --packages-select assignment4` from the workspace root and fix any errors.
- Match existing code style (types, naming, BFL ColumnVector/Matrix APIs).
- Use symbolic math (e.g. MATLAB/Python sympy) off-repo if needed; paste derived formulas correctly into C++.
- State is 6D: position (x,y,z) and RPY (roll, pitch, yaw). Control u = [v, omega]^T: v along body +X, omega about body +Z. Rotation convention R = RX(r) RY(p) RZ(psi) — keep the same convention as the skeleton for body/world velocity and Jacobians.
- Do not confuse position y with yaw; use clear variable names (e.g. yaw or psi) in code comments where helpful.

Workspace path (EDIT ME): <PASTE_PATH_TO_sbr_workspace_OR_assignment4_PARENT>
```

---

## 第 1 段：读懂骨架 + 过程模型预测步

```text
Task: Implement the EKF prediction model in ekf_models.cpp / ekf_models.hpp.

1) Read the skeleton: sys_evaluate_f, sys_evaluate_A, sys_evaluate_WMWt. Identify state index order, how u (v, omega) and dt are passed, and how ColumnVector/Matrix are indexed.

2) Implement sys_evaluate_f:
   - Discrete-time propagation consistent with continuous-time kinematics: body linear velocity [v,0,0]^T mapped to world frame using R(r,p,yaw); body angular velocity [0,0,omega]^T mapped to Euler rates for the RX-RY-RZ yaw convention used in the assignment.
   - Integrate with the same dt the filter uses.

3) Implement sys_evaluate_A: Jacobian of f w.r.t. state (6x6). Prefer deriving with symbolic tools, then translate carefully to C++.

4) Implement sys_evaluate_WMWt: positive-definite process noise covariance, explicitly dependent on |v| and |omega| (tunable scalings). Start conservative.

5) Build assignment4 and fix compile errors.

6) Sanity checks (describe results in chat): zero input should approximately hold state; forward v>0 should move in expected world direction; omega-only should mainly change yaw without crazy position drift.

Do not implement measurement functions in this step.
```

---

## 第 2 段：GPS / IMU 观测与 R

```text
Task: Implement measurement models and noise in ekf_models.cpp / ekf_models.hpp.

1) Implement meas_evaluate_gps and meas_evaluate_Hgps (3x3 block in the 3x6 or assembled H as per skeleton):
   - h_gps predicts the GPS fix vector from state. The PDF allows a local flat-earth linear map from (x,y,z) to (lat, lon, alt) or equivalent message fields; you may calibrate linear coefficients using paired samples from ign true pose and /sensors/gps_0/fix while developing.
   - H_gps is the Jacobian of h_gps w.r.t. the full state; if the map is linear in (x,y,z), yaw/roll/pitch columns are zero unless your model says otherwise.

2) Implement meas_evaluate_imu and meas_evaluate_Himu:
   - Map predicted measurement from state RPY consistent with the filter state and IMU message convention in ROS2 (watch quaternion order if converting elsewhere; here prediction is usually direct from state RPY if that matches the assignment expectation).
   - H_imu should be mostly selecting the orientation components of the state with correct column placement.

3) Implement meas_evaluate_R: block-diagonal R for GPS and IMU noise. Start with moderate values; document chosen structure.

4) Build assignment4; fix compile errors.

5) Runtime sanity (I will run simulation): after teleop init, posterior should track without immediate divergence; if jumps occur, first verify h and H dimensions and positivity of R blocks before retuning scalars.
```

---

## 第 3 段：联调、调参、Expert 指标脚本思路

```text
Task: Tune filters and prepare reproducible error evaluation (no change to allowed files beyond ekf_models.* — if a script is needed, put it outside the assignment4 package or only use shell/ros2 CLI).

1) Jointly tune sys_evaluate_WMWt and meas_evaluate_R:
   - Change one family at a time (e.g., fix R, tune Q/WMWt, then vice versa in small steps).
   - Test: straight line, turn-in-place, short hill segment in Gazebo.

2) Expert challenge prep:
   - Define position error e = p_posterior - p_true in a consistent world frame; L2 norm ||e||_2.
   - Propose a minimal way to log time-aligned samples (ros2 bag + offline Python, or a small rclpy node in a scratch folder OUTSIDE assignment4 if course allows) and compute time-averaged mean ||e||_2 over a canyon drive. Target: mean < 0.5 m.

3) If error stuck above 0.5 m: checklist GPS axis/scaling, yaw sign, H column order, A Jacobian spot-check with finite differences on f.

4) Final pass: ensure only ekf_models.hpp/cpp contain assignment logic changes; remove debug spam; rebuild clean.
```

---

## 第 4 段：提交前干净构建与打包

```text
Task: Verify submission packaging per course instructions.

1) From a fresh terminal: source ROS setup, then from workspace root run a clean build of assignment4 only and confirm success.

2) Create assignment4.tgz using the course tar command (parent directory and folder name must match the PDF). Exclude build/install artifacts unless instructed otherwise.

3) List the contents of the tarball to verify package layout (package.xml, CMakeLists.txt, src/ekf_models.* present).

Report exact commands used and any warnings worth fixing.
```

---

## 可选：单条「总控」提示（仅当你希望它自主分步时）

```text
Complete JHU ASBR Assignment 4 EKF implementation in ekf_models only (ekf_models.hpp, ekf_models.cpp): full prediction (f, A, WMWt) and measurements (GPS, IMU, H blocks, R), then tune for stable tracking and mean XYZ L2 error < 0.5 m vs ign ground truth during canyon driving. Obey ROS2 Humble + BFL APIs in the skeleton; build after each milestone; do not edit other packages. Summarize math choices and Jacobian derivation sources you used.
```

（注意：总控提示较长，Claude Code 可能仍会拆步执行；**更稳妥**的做法是使用上面 **第 1-4 段** 分次发送。）

---

## 本地执行前你要做的事（Claude 无法替你完成）

- [ ] 已安装课程环境（含 `ros-humble-clearpath*`、`sbr-a4.sh` 等），仿真可 `ros2 launch assignment4 assignment4.launch.py`。  
- [ ] Gazebo 点 **Play**，并发 **至少一次遥控** 以启动 EKF。  
- [ ] 将 **含 `assignment4` 包的工作区** 作为 Claude Code 打开的根目录（或明确告诉它绝对路径）。
