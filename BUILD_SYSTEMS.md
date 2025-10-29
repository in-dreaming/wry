# Wry 构建系统指南

本项目支持多种构建方式，包括 Zig 构建系统（推荐）和传统的 Cargo 构建系统。

## 构建系统对比

| 特性 | Zig 构建系统 | Cargo |
|------|-------------|-------|
| 命令行统一性 | ✅ 优秀 | ⚠️ 需多步骤 |
| C 示例集成 | ✅ 自动 | ❌ 需手动 |
| 交叉编译 | ✅ 简单 | ⚠️ 复杂 |
| 并行构建 | ✅ 内置 | ✅ 内置 |
| 快速 | ✅ 很快 | ✅ 快 |

## 方式 1: Zig 构建系统（推荐）

### 安装 Zig

```bash
# 下载并安装 Zig
# https://ziglang.org/download/
```

### 基础用法

```bash
# 构建所有内容
zig build

# 构建并启用 C ABI
zig build -Dcabi=true

# 构建 C 示例
zig build build-examples

# 运行示例
zig build run-simple
```

详细说明请查看 [`README_ZIG_BUILD.md`](README_ZIG_BUILD.md)。

## 方式 2: Cargo 构建系统

### 基础用法

```bash
# 构建库
cargo build --lib

# 构建库（启用 C ABI）
cargo build --lib --features cabi

# 运行示例
cargo run --example simple

# 测试
cargo test
```

### 使用 C ABI

```bash
# 编译 C ABI
cargo build --lib --features cabi --release

# 查看生成的库
ls target/release/
```

### 编译 C 示例

```bash
cd examples/c_api

# 使用 Makefile
make

# 或手动编译
gcc -o simple simple.c \
    -I../../include \
    -L../../target/release \
    -lwry
```

详细说明请查看 [`examples/c_api/README.md`](examples/c_api/README.md)。

## 平台特定说明

### Windows

**使用 Zig:**
```bash
zig build -Dtarget=x86_64-windows-msvc
```

**使用 Cargo:**
```bash
cargo build --lib --features cabi --release --target x86_64-pc-windows-msvc
```

**依赖:**
- Windows SDK
- MSVC 工具链

### Linux

**使用 Zig:**
```bash
zig build -Dtarget=x86_64-linux-gnu
```

**使用 Cargo:**
```bash
cargo build --lib --features cabi --release --target x86_64-unknown-linux-gnu
```

**依赖:**
```bash
# Ubuntu/Debian
sudo apt-get install libgtk-3-dev libwebkit2gtk-4.1-dev libsoup-3.0-dev

# Arch Linux
sudo pacman -S webkit2gtk-4.1 gtk3

# Fedora
sudo dnf install gtk3-devel webkit2gtk4.1-devel
```

### macOS

**使用 Zig:**
```bash
zig build -Dtarget=aarch64-macos
```

**使用 Cargo:**
```bash
cargo build --lib --features cabi --release --target aarch64-apple-darwin
```

**依赖:**
- Xcode Command Line Tools
- CocoaPods（可选）

## 完整的构建流程

### 使用 Zig（推荐）

```bash
# 1. 克隆仓库
git clone https://github.com/tauri-apps/wry.git
cd wry

# 2. 构建（启用 C ABI）
zig build -Dcabi=true

# 3. 构建示例
zig build build-examples

# 4. 运行测试
zig build test

# 5. 安装
zig build install
```

### 使用 Cargo

```bash
# 1. 克隆仓库
git clone https://github.com/tauri-apps/wry.git
cd wry

# 2. 构建库
cargo build --lib --features cabi --release

# 3. 编译 C 示例
cd examples/c_api
make

# 4. 运行测试
cargo test

# 5. 安装（可选）
cargo install --path .
```

## 比较：Zig vs Cargo

### 简单构建

**Zig:**
```bash
zig build
```

**Cargo:**
```bash
cargo build --lib --features cabi
```

### 构建 C 示例

**Zig:**
```bash
zig build build-examples
```

**Cargo:**
```bash
cd examples/c_api
make  # 或手动编译
```

### 交叉编译

**Zig:**
```bash
zig build -Dtarget=x86_64-linux-gnu
```

**Cargo:**
```bash
# 需要设置环境
export CC_x86_64-unknown-linux-gnu=x86_64-linux-gnu-gcc
cargo build --target x86_64-unknown-linux-gnu
```

## 建议

### 开发环境

- **新手**: 使用 Zig 构建系统，简单统一
- **Rust 开发者**: 使用 Cargo，熟悉的工具
- **跨平台**: 使用 Zig，交叉编译简单

### 生产环境

- **CI/CD**: 使用 Cargo，生态系统成熟
- **分布式构建**: 使用 Zig，速度快
- **容器化**: 两者都支持

## 故障排除

### Zig 构建失败

```bash
# 检查 Zig 版本
zig version  # 需要 0.11.0+

# 清理并重新构建
zig clean
zig build
```

### Cargo 构建失败

```bash
# 更新依赖
cargo update

# 清理缓存
cargo clean

# 重新构建
cargo build --lib --features cabi
```

### 找不到库

```bash
# 使用 Zig
export LD_LIBRARY_PATH=./zig-out/lib:$LD_LIBRARY_PATH

# 使用 Cargo
export LD_LIBRARY_PATH=./target/release:$LD_LIBRARY_PATH
```

## 总结

两种构建系统都可以使用，选择适合你的：

- **Zig**: 统一接口，简单易用，推荐新用户
- **Cargo**: 成熟稳定，Rust 生态，推荐 Rust 开发者

两者可以混用，选择最适合你的方式！


