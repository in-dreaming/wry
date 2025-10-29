# Wry C API 示例

本目录包含使用 Wry C API 的示例程序。

## 示例列表

- **simple.c** - 基础 WebView 创建示例
- **ipc_example.c** - IPC 通信示例
- **custom_protocol.c** - 自定义协议示例
- **full_features.c** - 完整功能演示

## 编译

### 前置要求

1. 编译 wry 库（启用 cabi feature）：
```bash
cargo build --features cabi --release
```

2. 准备 wry.h 头文件：
```bash
# 头文件位于项目根目录的 include/ 目录
cp ../../include/wry.h .
```

### Windows 编译

使用 Visual Studio 或 MSVC：

```cmd
cl simple.c /I..\..\include /link /LIBPATH:..\..\..\target\release libwry.lib
```

或使用 GCC/MinGW：

```bash
gcc -o simple.exe simple.c -I../../include -L../../target/release -lwry
```

### Linux 编译

```bash
gcc -o simple simple.c -I../../include -L../../target/release -lwry \
    $(pkg-config --libs gtk+-3.0 webkit2gtk-4.1)
```

### macOS 编译

```bash
gcc -o simple simple.c -I../../include -L../../target/release -lwry \
    -framework AppKit -framework WebKit
```

## 运行

确保库文件在库路径中：

**Linux/macOS:**
```bash
export LD_LIBRARY_PATH=../../target/release:$LD_LIBRARY_PATH
./simple
```

**Windows:**
```cmd
set PATH=..\..\..\target\release;%PATH%
simple.exe
```

## 示例说明

### simple.c

最简单的示例，展示如何：
- 初始化 Wry
- 创建 WebViewBuilder
- 配置 WebView
- 清理资源

### ipc_example.c

演示如何：
- 设置 IPC 消息处理器
- 从 JavaScript 接收消息
- 处理导航事件

### custom_protocol.c

展示如何使用自定义协议：
- 注册自定义协议处理器
- 处理自定义协议请求
- 发送响应

### full_features.c

综合示例，展示所有功能：
- WebContext 管理
- 完整的 WebViewBuilder 配置
- IPC 和导航处理
- 资源清理

## 注意事项

1. **窗口句柄**：实际应用需要提供有效的窗口句柄
2. **事件循环**：需要在主线程上运行事件循环
3. **平台差异**：不同平台可能需要额外的依赖和配置

## 故障排除

### 编译错误

- 确保已安装所有平台依赖
- 检查库文件路径是否正确
- 确认头文件版本与库版本匹配

### 运行时错误

- 确保库在正确的路径
- 检查初始化是否正确调用
- 查看错误消息使用 `wry_last_error_message()`

### 链接错误

- Linux: 安装 webkit2gtk-4.1
- macOS: 确保链接 WebKit 框架
- Windows: 确保链接 wry DLL


