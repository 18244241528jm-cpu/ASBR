# GitHub 上传指南

## ✅ 已完成步骤

1. ✅ 初始化 Git 仓库
2. ✅ 创建 .gitignore 文件
3. ✅ 添加文件到暂存区
4. ✅ 创建初始提交

## 📋 接下来需要做的步骤

### 步骤1：在 GitHub 上创建新仓库

1. **登录 GitHub**
   - 访问 https://github.com
   - 登录你的账户

2. **创建新仓库**
   - 点击右上角的 "+" 号
   - 选择 "New repository"

3. **填写仓库信息**
   - **Repository name**: `ASBR`（或你喜欢的名字）
   - **Description**: "ASBR课程资料和作业解析"
   - **Visibility**: 
     - Public（公开，所有人都能看到）
     - Private（私有，只有你能看到）
   - ⚠️ **不要**勾选 "Initialize this repository with a README"
   - ⚠️ **不要**添加 .gitignore 或 license（我们已经有了）

4. **点击 "Create repository"**

### 步骤2：连接本地仓库到 GitHub

**复制仓库地址**（创建仓库后 GitHub 会显示）

然后执行以下命令：

```bash
cd /Users/tommyboy/Desktop/ASBR

# 添加远程仓库（替换 YOUR_USERNAME 为你的GitHub用户名）
git remote add origin https://github.com/YOUR_USERNAME/ASBR.git

# 或者使用SSH（如果你配置了SSH密钥）
# git remote add origin git@github.com:YOUR_USERNAME/ASBR.git
```

**查看远程仓库**：
```bash
git remote -v
```

### 步骤3：推送代码到 GitHub

```bash
# 推送代码到GitHub（第一次推送）
git push -u origin main
```

**如果遇到错误**：
- 如果提示需要认证，输入你的 GitHub 用户名和密码（或 Personal Access Token）
- 如果分支名是 `master` 而不是 `main`，使用：`git push -u origin master`

### 步骤4：验证上传

1. 刷新 GitHub 网页
2. 应该能看到所有文件
3. 检查 README.md 是否正确显示

---

## 🔄 后续更新代码

如果以后修改了文件，使用以下命令更新：

```bash
# 1. 查看修改状态
git status

# 2. 添加修改的文件
git add .

# 3. 提交修改
git commit -m "描述你的修改"

# 4. 推送到GitHub
git push
```

---

## 🔐 认证方式

### 方式1：HTTPS + Personal Access Token（推荐）

1. **生成 Token**：
   - GitHub → Settings → Developer settings → Personal access tokens → Tokens (classic)
   - 点击 "Generate new token"
   - 选择权限：至少勾选 `repo`
   - 生成并复制 token

2. **使用 Token**：
   - 推送时，用户名输入你的 GitHub 用户名
   - 密码输入刚才生成的 token

### 方式2：SSH 密钥（更安全）

1. **生成 SSH 密钥**：
   ```bash
   ssh-keygen -t ed25519 -C "your_email@example.com"
   ```

2. **添加 SSH 密钥到 GitHub**：
   - 复制公钥：`cat ~/.ssh/id_ed25519.pub`
   - GitHub → Settings → SSH and GPG keys → New SSH key
   - 粘贴公钥并保存

3. **使用 SSH URL**：
   ```bash
   git remote set-url origin git@github.com:YOUR_USERNAME/ASBR.git
   ```

---

## 📁 关于大文件

**注意**：当前目录有一些大型PDF文件（>10MB）

### 选项1：不上传PDF文件（推荐）

`.gitignore` 已经配置好了，PDF文件不会被上传。

如果你想上传某些PDF，可以：

```bash
# 强制添加特定文件
git add -f assignment1-sp26.pdf
git commit -m "Add assignment PDF"
git push
```

### 选项2：使用 Git LFS（大文件存储）

如果必须上传大文件：

```bash
# 安装 Git LFS
brew install git-lfs  # macOS

# 初始化
git lfs install

# 跟踪PDF文件
git lfs track "*.pdf"

# 添加并提交
git add .gitattributes
git add *.pdf
git commit -m "Add PDF files with LFS"
git push
```

---

## 🛠️ 常用 Git 命令

```bash
# 查看状态
git status

# 查看提交历史
git log

# 查看远程仓库
git remote -v

# 拉取最新代码
git pull

# 查看文件差异
git diff

# 撤销暂存的文件
git reset HEAD <文件名>

# 查看分支
git branch
```

---

## ❓ 常见问题

### Q1: 推送时提示 "Permission denied"
**A**: 
- 检查用户名和密码/token是否正确
- 如果使用SSH，检查SSH密钥是否已添加到GitHub

### Q2: 推送时提示 "remote origin already exists"
**A**: 
```bash
# 删除现有远程仓库
git remote remove origin

# 重新添加
git remote add origin https://github.com/YOUR_USERNAME/ASBR.git
```

### Q3: 推送时提示 "failed to push some refs"
**A**: 
```bash
# 先拉取远程代码
git pull origin main --allow-unrelated-histories

# 解决冲突后再次推送
git push
```

### Q4: 想删除已上传的文件
**A**: 
```bash
# 从Git中删除（但保留本地文件）
git rm --cached <文件名>

# 提交并推送
git commit -m "Remove file"
git push
```

---

## 📝 完整命令序列（快速参考）

```bash
# 1. 进入目录
cd /Users/tommyboy/Desktop/ASBR

# 2. 添加远程仓库（替换YOUR_USERNAME）
git remote add origin https://github.com/YOUR_USERNAME/ASBR.git

# 3. 推送代码
git push -u origin main
```

---

**完成！** 现在你的代码应该已经在GitHub上了！🎉

