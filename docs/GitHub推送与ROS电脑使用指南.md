# ASBR 推送到 GitHub 及在 ROS 电脑上使用

---

## 一、把 ASBR 推送到 GitHub

### 1. 当前状态

- 远程仓库已配置：`https://github.com/18244241528jm-cpu/ASBR.git`
- 有未提交内容：assignments/、notes/、lectures/、docs/ 等

### 2. 推送步骤

```bash
cd /Users/tommyboy/Desktop/ASBR

# 添加所有文件
git add .

# 提交
git commit -m "Add HW2 plans, study guides, ROS launch guide"

# 推送
git push -u origin main
```

### 3. 若推送失败

**认证**：使用 Personal Access Token 作为密码（不是 GitHub 登录密码）

- 生成：https://github.com/settings/tokens
- 勾选 `repo` 权限
- 推送时密码处粘贴 token

**分支名**：若本地是 `master`，用：

```bash
git push -u origin master
```

### 4. 大文件说明

`.gitignore` 已排除 `*.tgz`、`*.tar.gz`。若 PDF 过大导致推送失败，可临时加入：

```
# 在 .gitignore 中添加（若需要）
*.pdf
```

---

## 二、在有 ROS 的电脑上使用

### 1. 克隆仓库

```bash
cd ~
git clone https://github.com/18244241528jm-cpu/ASBR.git
```

### 2. 安装课程 ROS 环境

```bash
cd ~
wget www.cs.jhu.edu/~sleonard/sbr-a2.sh
chmod 755 sbr-a2.sh
./sbr-a2.sh
```

会创建 `~/sbr_workspace`（或脚本指定的路径）。

### 3. 复制 Matlab 代码到 assignment2

作业要求 `.m` 文件放在 `assignment2/matlab/`。若你的代码在 `ASBR/assignment2/matlab/`：

```bash
cp ~/ASBR/assignment2/matlab/*.m ~/sbr_workspace/src/sbr/assignment2/matlab/
```

若在 `ASBR/assignments/hw2/matlab/`：

```bash
mkdir -p ~/sbr_workspace/src/sbr/assignment2/matlab
cp ~/ASBR/assignments/hw2/matlab/*.m ~/sbr_workspace/src/sbr/assignment2/matlab/
```

**注意**：`sbr-a2.sh` 会创建 assignment2 包；若没有 `matlab` 目录，先创建：

```bash
mkdir -p ~/sbr_workspace/src/sbr/assignment2/matlab
```

### 4. 启动 assignment2 仿真

```bash
cd ~/sbr_workspace
source install/setup.bash
ros2 launch assignment2 assignment2.launch.py
```

### 5. 排障

详见 `assignments/hw2/assignment2_ROS启动与排障指南.md`。

---

## 三、目录结构对应关系

| ASBR 仓库 | ROS 工作空间 |
|-----------|--------------|
| `ASBR/assignment2/matlab/*.m` | `sbr_workspace/src/sbr/assignment2/matlab/*.m` |
| 或 `ASBR/assignments/hw2/matlab/*.m` | 同上，需手动复制 |

作业打包命令要求从 `sbr_workspace/src/sbr` 下执行，因此 Matlab 代码需放在该路径的 `assignment2/matlab/` 中。

---

## 四、快速命令汇总

**本机推送**：
```bash
cd /Users/tommyboy/Desktop/ASBR
git add .
git commit -m "Update ASBR"
git push origin main
```

**ROS 电脑拉取并运行**：
```bash
git clone https://github.com/18244241528jm-cpu/ASBR.git
# 安装课程环境（若未安装）
wget www.cs.jhu.edu/~sleonard/sbr-a2.sh && chmod 755 sbr-a2.sh && ./sbr-a2.sh
# 复制 Matlab 代码
mkdir -p ~/sbr_workspace/src/sbr/assignment2/matlab
cp ~/ASBR/assignment2/matlab/*.m ~/sbr_workspace/src/sbr/assignment2/matlab/
# 启动仿真
cd ~/sbr_workspace && source install/setup.bash
ros2 launch assignment2 assignment2.launch.py
```
