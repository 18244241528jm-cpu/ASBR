# Day 4 详细操作指导：ROS 数据采集 + 打包提交

**预计总时长**：约 3–5 小时（463 可缩短，663 需完成 Expert Challenge）  
**产出**：data.txt（663）+ 最终验证 + assignment2.tgz

---

## 版本区分

| 版本 | Day 4 重点 | Expert Challenge |
|------|------------|------------------|
| **463** | 最终测试 + 打包 | 无，可不做 ROS |
| **663** | ROS 数据采集 + 验证 + 打包 | 必做，平移 <5mm、旋转 <0.1 rad |

---

## 上午：ROS 数据采集（663 必做，463 可跳过）

### 1. 环境准备

```bash
# 若未安装，先运行作业提供的脚本
wget www.cs.jhu.edu/~sleonard/sbr-a2.sh
chmod 755 sbr-a2.sh
./sbr-a2.sh
```

### 2. 启动仿真

```bash
ros2 launch assignment2 assignment2.launch.py
```

- 打开 Rviz，移动机械臂使摄像头能看到 AR tag
- 确保 AR tag 被正确检测（画面中有 tag 名称和坐标系）

### 3. 采集数据

需采集 **5+ 个不同位姿**，每个位姿记录两组 tf：

| 帧 | 命令 | 含义 |
|----|------|------|
| gripper | `ros2 run tf2_ros tf2_echo world gripper_pick` | 正向运动学 E_bh |
| marker | `ros2 run tf2_ros tf2_echo camera_link marker` | 传感器测量 E_sc |

**操作流程**（每个位姿重复）：

1. 移动机械臂到新位姿，确保 AR tag 可见
2. 终端 1：`tf2_echo world gripper_pick`，复制输出的 translation 和 rotation (qx qy qz qw)
3. 终端 2：`tf2_echo camera_link marker`，复制 translation 和 rotation
4. 将两行按格式写入 data.txt

### 4. data.txt 格式（必须严格遵循）

```
# gripper
tx1 ty1 tz1 qx1 qy1 qz1 qw1
tx2 ty2 tz2 qx2 qy2 qz2 qw2
...
txN tyN tzN qxN qyN qzN qwN
# marker
tx1 ty1 tz1 qx1 qy1 qz1 qw1
tx2 ty2 tz2 qx2 qy2 qz2 qw2
...
txN tyN tzN qxN qyN qzN qwN
```

- `# gripper` 和 `# marker` 行必须保留
- 每行 7 个数：`tx ty tz qx qy qz qw`
- gripper 与 marker 行数相同，一一对应

### 5. 解析 data.txt 供 axxb 使用

```matlab
function [e_bh, e_sc] = load_data(filename)
    fid = fopen(filename, 'r');
    section = 0;  % 0=未开始, 1=gripper, 2=marker
    e_bh = [];
    e_sc = [];
    while ~feof(fid)
        line = fgetl(fid);
        if ~ischar(line), continue; end
        line = strtrim(line);
        if isempty(line), continue; end
        if strcmp(line, '# gripper')
            section = 1;
            continue;
        end
        if strcmp(line, '# marker')
            section = 2;
            continue;
        end
        nums = str2num(line);
        if length(nums) == 7
            if section == 1
                e_bh = [e_bh; nums];
            elseif section == 2
                e_sc = [e_sc; nums];
            end
        end
    end
    fclose(fid);
end
```

---

## 下午：验证与可视化（663）

### 1. 用采集数据跑 axxb

```matlab
[e_bh, e_sc] = load_data('data.txt');
X = axxb(e_bh, e_sc);
```

### 2. 在 Rviz 中对比

用 `static_transform_publisher` 发布你的结果：

```bash
ros2 run tf2_ros static_transform_publisher --x TX --y TY --z TZ --qx QX --qy QY --qz QZ --qw QW --frame-id gripper_pick --child-frame-id result
```

将 TX TY TZ QX QY QZ QW 替换为 X 的平移和四元数（注意从 4×4 或 rotm2quat 转换，并转为 [qx qy qz qw]）。

- `result` 应接近真实的 `camera_link`
- 平移误差 < 5 mm，旋转误差 < 0.1 rad 即达标

### 3. 误差计算（用于自检）

```matlab
% 若已知真实 X（仿真中可用 tf 查询 camera 相对 gripper 的变换）
X_true = ...;  % 从仿真获取
err_t = norm(X(1:3,4) - X_true(1:3,4));
R_err = X(1:3,1:3) * X_true(1:3,1:3)';
angle_err = acos((trace(R_err) - 1) / 2);  % 弧度
fprintf('平移误差: %.4f mm\n', err_t*1000);
fprintf('旋转误差: %.4f rad\n', angle_err);
```

---

## 晚上：打包提交（463/663 均需）

### 1. 目录结构检查

```
assignment2/
└── matlab/
    ├── generatedata.m
    ├── axxb_closedform.m
    ├── solveRx.m
    ├── solveTx.m
    ├── axxb.m
    └── data.txt          # 仅 663 需要
```

### 2. 提交检查清单

- [ ] 所有 .m 文件在 `assignment2/matlab/` 下
- [ ] 函数签名与作业完全一致（参数名、维度）
- [ ] 663：data.txt 存在且格式正确（含 `# gripper`、`# marker`）
- [ ] 463：可不含 data.txt

### 3. 打包命令（必须按此执行）

```bash
cd ~/sbr_workspace/src/sbr   # 或你的 sbr 工作空间路径
tar cvfz ~/assignment2.tgz -C ~/sbr_workspace/src/sbr assignment2
```

**注意**：`-C` 后的路径需保证 `assignment2` 在该目录下。若你的结构不同，调整为：

```bash
tar cvfz ~/assignment2.tgz -C <assignment2 所在目录的父目录> assignment2
```

### 4. 验证压缩包

```bash
tar tvfz ~/assignment2.tgz
```

应看到 `assignment2/matlab/*.m` 及（663）`assignment2/matlab/data.txt`。

---

## 463 版本 Day 4 简化流程

若选 463，Day 4 可简化为：

1. **上午**：用 generatedata 做最后一轮完整测试，确认无回归
2. **下午**：检查代码风格、注释、函数签名
3. **晚上**：按上述步骤打包提交

---

## Day 4 完成检查清单

- [ ] 663：data.txt 格式正确，5+ 组数据
- [ ] 663：平移误差 < 5 mm，旋转误差 < 0.1 rad
- [ ] 所有 .m 文件齐全且可运行
- [ ] 使用规定命令生成 assignment2.tgz
- [ ] `tar tvfz` 检查压缩包内容正确

---

## 常见坑

1. **tf2_echo 输出**：直接是 [qx qy qz qw]，与 data.txt 一致，无需再转换
2. **static_transform_publisher**：参数为 `--x --y --z --qx --qy --qz --qw`，注意单位（米、四元数）
3. **打包路径**：`-C` 指定的是解压时的根目录，要确保 `assignment2` 在解压后路径正确
