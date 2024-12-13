# 数学解题大模型

## 项目简介

本项目旨在开发一个基于人工智能的数学解题大模型，能够自动解析和解决各种数学问题。该模型将利用最新的自然语言处理技术和机器学习算法，帮助用户快速获取数学问题的解答和详细步骤。

## 功能特性

- **问题解析**：能够理解用户输入的数学问题，包括代数、几何、微积分等领域。
- **解题步骤**：提供详细的解题步骤，帮助用户理解解题过程。
- **多种输入格式**：支持文本输入和公式输入，方便用户使用。
- **用户友好的界面**：提供简洁直观的用户界面，提升用户体验。

## 安装与运行

### 克隆项目

首先，您需要克隆项目到本地：

```bash
git clone https://github.com/yourusername/math-solver.git
cd math-solver
```

### 创建虚拟环境

为确保依赖项的隔离，建议使用虚拟环境：

```bash
# Windows
python -m venv venv
venv\Scripts\activate

# macOS/Linux
python3 -m venv venv
source venv/bin/activate
```

### 安装依赖

使用 `pip` 安装所需的依赖项：

```bash
pip install -r requirements.txt
```

### 数据库设置（可选）

如果您选择使用数据库，请根据需要创建数据库并配置连接。以下是使用 SQLite 的示例：

```bash
# 创建数据库文件
touch database.db
```

### 运行后端服务器

在项目根目录下运行 Flask 服务器：

```bash
export FLASK_APP=app.py
flask run
```

### 运行前端应用

在前端目录中运行 Vue.js 应用：

```bash
cd frontend
npm install
npm run serve
```

### 访问应用

打开浏览器，访问 `http://localhost:8080` 以查看应用。
