# Assignment 1 第二部分解析：Ubuntu 和 ROS 安装配置

**章节**: 2. Installation and Configuration of Ubuntu and ROS  
**重要性**: ⭐⭐⭐⭐⭐ 环境配置是完成作业的基础

---

## 📋 概述

如果你没有安装 Ubuntu 和 ROS 的电脑，有以下**5种选择**：

1. 在电脑上安装 Ubuntu（独立安装或双系统）
2. 使用虚拟机（Virtual Machine）
3. 使用 Docker
4. 使用 Wyman 170 实验室电脑
5. 熟悉 ROS（无论选择哪种方式都需要）

---

## 🖥️ 方案1：在电脑上安装 Ubuntu（2.1节）

### 适用场景
- ✅ 有备用电脑
- ✅ 想学习 Linux 系统管理
- ✅ 想要最佳性能

### 安装选项

#### 选项A：独立安装（Standalone Installation）
- **特点**：完全占用整台电脑
- **适用**：专用开发机器
- **优点**：性能最佳，无干扰

#### 选项B：双系统安装（Dual Boot）
- **特点**：保留现有操作系统（Windows/Mac）
- **适用**：需要同时使用两个系统
- **⚠️ 警告**：安装时**务必小心**，不要误删现有系统！

**启动方式**：每次开机时选择要启动的操作系统

### 安装步骤

1. **安装 Ubuntu 22.04**
   - 下载地址：https://ubuntu.com/download/desktop
   - 版本：Ubuntu 22.04 LTS（长期支持版）

2. **安装 ROS2 Humble**
   - 安装指南：https://docs.ros.org/en/humble/Installation.html
   - 版本：ROS2 Humble（与 Ubuntu 22.04 匹配）

### 推荐人群
- **MSE 机器人学学生**：强烈推荐学习 Linux 系统管理
- 这是**值得投入时间**的经验（worthwhile experience）

---

## 💻 方案2：虚拟机（2.2节）

### 适用场景
- ✅ 使用 Apple 或 Windows 电脑
- ✅ 想保持现有系统不变
- ✅ 不想修改硬盘分区

### 虚拟机软件
- **VirtualBox**：免费开源
- **VMware**：商业软件（有免费版本）

### 工作原理
- **Host OS（主系统）**：你当前的操作系统
- **Guest OS（客户系统）**：在虚拟机中运行的 Ubuntu
- 两个系统**同时运行**

### 硬件要求

| 资源 | 最低要求 | 推荐 |
|------|---------|------|
| **内存** | 8GB+ | 16GB+ |
| **硬盘空间** | 20GB+ | 50GB+ |
| **CPU** | 多核处理器 | 4核+ |

**为什么需要这么多资源？**
- 同时运行两个操作系统
- ROS 和机器人仿真需要较多资源

### 安装步骤
1. 下载并安装 VirtualBox 或 VMware
2. 创建新的虚拟机
3. 下载 Ubuntu 22.04 ISO 镜像
4. 在虚拟机中安装 Ubuntu（与普通安装相同）
5. 安装 ROS2 Humble

### ⚠️ 注意事项

**虚拟机不是完美的**，可能遇到的问题：

1. **图形应用问题**
   - 某些图形界面可能显示异常
   - 3D 渲染可能较慢

2. **相机仿真问题**
   - 后续作业中的相机仿真可能有问题
   - 需要额外配置

3. **性能较慢**
   - 比物理安装慢
   - 编译包需要更长时间

**解决建议**：
- 大部分问题可以解决
- 需要一些**耐心和调试工作**

---

## 🐳 方案3：Docker（2.3节）

### 适用场景
- ✅ 熟悉 Docker 容器技术
- ✅ 想要快速部署环境
- ✅ 需要隔离的开发环境

### 特点
- **容器化**：轻量级虚拟化
- **快速部署**：使用预构建镜像
- **易于管理**：可以快速重置环境

### 获取方式
- 有可用的 Docker 镜像
- 需要查找相关说明文档（文档中提到"here"链接）

### ⚠️ 注意事项
- 作者不熟悉 Docker
- 有学生使用过此方法
- 需要自己查找文档和解决问题

### 推荐人群
- 已有 Docker 使用经验的学生
- 想要尝试新技术的学生

---

## 🏢 方案4：Wyman 170 实验室电脑（2.4节）

### 适用场景
- ✅ 不想配置自己的电脑
- ✅ 需要高性能计算资源
- ✅ 可以到实验室工作

### 访问要求

| 项目 | 要求 |
|------|------|
| **物理访问** | JCard（学生卡） |
| **时间限制** | 正常时间 + 非正常时间（需要JCard） |
| **登录** | JHED ID 和密码 |
| **电脑数量** | 12 台工作站 |

### 硬件配置

**强大的硬件**：
- **CPU**：10 核 @ 3.7GHz
- **内存**：70GB RAM
- **存储**：学生账户 20GB 配额

### 软件环境

**已预装软件**：
- ✅ ROS2 Humble
- ✅ Matlab
- ✅ 所有必需的包
- ✅ **无需运行任何 `sudo` 命令**

**优势**：
- 开箱即用
- 无需配置
- 性能强大

### ⚠️ 限制和注意事项

1. **账户配额**
   - 20GB 存储空间
   - 学期结束后账户会被删除

2. **网络性能**
   - 学生账户是**网络账户**
   - 编译包时**显著变慢**
   - 因为文件系统通过网络访问

3. **账户配置**
   - 首次使用需要按照说明配置账户
   - 有专门的配置指南（文档中提到的"instructions"链接）

### 使用建议
- 适合不想折腾环境的学生
- 但编译速度可能较慢
- 需要到实验室工作

---

## 📚 方案5：熟悉 ROS（2.5节）

### ⭐ 重要性

**无论选择哪种安装方式，都必须熟悉 ROS！**

这是作业的**重要组成部分**，也是整个学期的**基础技能**。

### 推荐教程

**官方 ROS2 Humble 教程**：
- 网址：https://docs.ros.org/en/humble/Tutorials.html

### 重点学习内容

**必须掌握的 ROS 概念**：

1. **Nodes（节点）**
   - ROS 中的基本执行单元
   - 每个节点执行特定功能

2. **Publisher/Subscriber（发布者/订阅者）**
   - 节点间通信的模式
   - Publisher 发布消息
   - Subscriber 订阅消息

3. **Topics（话题）**
   - 节点间通信的通道
   - 消息通过话题传递

4. **TF（Transformation Manager，变换管理器）**
   - 管理坐标系之间的变换
   - 在机器人学中非常重要
   - 用于跟踪机器人各部分的位姿

### 学习建议

**时间投资**：
- 这些教程是**非常值得投入时间**的
- 对机器人学课程很有帮助
- 建议在开始作业前完成基础教程

**学习顺序**：
1. 先学习 Nodes 和 Topics
2. 然后学习 Publisher/Subscriber
3. 最后学习 TF（变换）

---

## 📦 额外包安装（2.5节补充）

### 适用场景

**仅适用于**：在自己的电脑上工作的学生

**不适用于**：使用 Wyman 170 电脑的学生（已预装）

### 需要安装的包

如果使用自己的电脑，需要安装以下 ROS2 包：

```bash
# 更新包列表
$ sudo apt-get update

# 安装 ROS2 包
$ sudo apt-get install \
    ros-humble-control* \
    ros-humble-ros2-control* \
    ros-humble-moveit* \
    ros-humble-robotiq* \
    ros-humble-ign-ros2-control \
    ros-humble-ros-ign* \
    ros-humble-joint-state-publisher-gui \
    ros-humble-kinematics-interface-kdl \
    ros-humble-rqt-joint-trajectory-controller \
    python3-colcon*
```

### 包的功能说明

| 包名 | 功能 |
|------|------|
| `ros-humble-control*` | 机器人控制框架 |
| `ros-humble-ros2-control*` | ROS2 控制接口 |
| `ros-humble-moveit*` | 运动规划库 |
| `ros-humble-robotiq*` | Robotiq 夹爪驱动 |
| `ros-humble-ign-ros2-control` | Gazebo 仿真控制 |
| `ros-humble-ros-ign*` | ROS-Gazebo 桥接 |
| `ros-humble-joint-state-publisher-gui` | 关节状态发布GUI |
| `ros-humble-kinematics-interface-kdl` | KDL 运动学接口 |
| `ros-humble-rqt-joint-trajectory-controller` | 轨迹控制器GUI |
| `python3-colcon*` | 构建工具（Python3） |

### 安装注意事项

1. **使用通配符 `*`**
   - 会安装所有相关包
   - 确保安装完整功能

2. **需要管理员权限**
   - 使用 `sudo` 命令
   - 需要输入密码

3. **安装时间**
   - 可能需要较长时间
   - 取决于网络速度

---

## 🎯 方案选择建议

### 决策树

```
你有自己的电脑吗？
├─ 是 → 你想学习Linux系统管理吗？
│   ├─ 是 → 选择方案1（物理安装）
│   └─ 否 → 选择方案2（虚拟机）
│
└─ 否 → 你熟悉Docker吗？
    ├─ 是 → 选择方案3（Docker）
    └─ 否 → 选择方案4（Wyman 170）
```

### 各方案对比

| 方案 | 优点 | 缺点 | 推荐度 |
|------|------|------|--------|
| **物理安装** | 性能最佳，学习价值高 | 需要专用电脑或双系统 | ⭐⭐⭐⭐⭐ |
| **虚拟机** | 不改变现有系统 | 性能较慢，可能有问题 | ⭐⭐⭐⭐ |
| **Docker** | 快速部署，易于管理 | 需要Docker知识 | ⭐⭐⭐ |
| **Wyman 170** | 无需配置，开箱即用 | 需要到实验室，编译慢 | ⭐⭐⭐⭐ |

### 个人推荐

**对于初学者**：
1. **首选**：Wyman 170（如果可以去实验室）
2. **次选**：虚拟机（如果想在家工作）
3. **进阶**：物理安装（如果想深入学习）

**对于有经验的学生**：
- Docker 或物理安装都可以

---

## ✅ 配置检查清单

完成安装后，检查以下项目：

### Ubuntu 检查
- [ ] Ubuntu 22.04 已安装
- [ ] 系统可以正常启动
- [ ] 网络连接正常
- [ ] 可以安装软件包

### ROS2 检查
- [ ] ROS2 Humble 已安装
- [ ] 环境变量已配置（`source /opt/ros/humble/setup.bash`）
- [ ] 可以运行 `ros2 --help`

### 额外包检查（仅自己的电脑）
- [ ] 所有必需的 ROS2 包已安装
- [ ] 可以导入相关模块

### ROS 知识检查
- [ ] 理解 Nodes 概念
- [ ] 理解 Topics 概念
- [ ] 理解 Publisher/Subscriber 模式
- [ ] 理解 TF 变换

---

## 🚀 下一步

完成环境配置后：

1. **继续阅读**：第3节 - SBR 包安装
2. **开始学习**：ROS2 官方教程
3. **准备作业**：理解前向运动学和雅可比矩阵

---

## 📖 相关资源链接

### Ubuntu
- 下载：https://ubuntu.com/download/desktop
- 文档：https://ubuntu.com/tutorials

### ROS2 Humble
- 安装指南：https://docs.ros.org/en/humble/Installation.html
- 教程：https://docs.ros.org/en/humble/Tutorials.html

### VirtualBox
- 下载：https://www.virtualbox.org/

### VMware
- 下载：https://www.vmware.com/

---

## ⚠️ 常见问题

### Q1: 我应该选择哪种方案？
**A**: 如果你不确定，建议从虚拟机开始。如果性能不够，再考虑物理安装。

### Q2: 虚拟机太慢了怎么办？
**A**: 
- 增加分配给虚拟机的内存
- 使用物理安装
- 或使用 Wyman 170 电脑

### Q3: 安装 ROS2 时出错怎么办？
**A**: 
- 检查 Ubuntu 版本（必须是 22.04）
- 检查网络连接
- 查看 ROS2 官方故障排除指南

### Q4: Wyman 170 电脑编译很慢怎么办？
**A**: 
- 这是网络文件系统的限制
- 可以在本地临时目录编译
- 或使用自己的电脑

---

**解析完成！** 现在你应该知道如何选择最适合你的环境配置方案了。

---

*解析完成时间：2024年*  
*如有疑问，请参考原始PDF文档或联系课程助教*

