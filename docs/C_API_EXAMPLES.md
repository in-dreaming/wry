# Wry C API 示例说明

## 概述

本文档详细说明如何使用 Wry C API 示例程序。

## 示例程序

### 1. simple.c - 基础示例

最简单的示例，展示基本的 WebView 创建流程。

**功能：**
- 初始化 Wry
- 创建 WebViewBuilder
- 配置 URL 和开发者工具
- 设置回调函数
- 清理资源

**运行：**
```bash
make simple
LD_LIBRARY_PATH=../../target/release ./simple
```

### 2. ipc_example.c - IPC 通信

演示如何从 JavaScript 接收消息。

**功能：**
- 设置 IPC 消息处理器
- 实现导航回调
- 处理来自 JavaScript 的消息

**运行：**
```bash
make ipc_example
LD_LIBRARY_PATH=../../target/release ./ipc_example
```

### 3. custom_protocol.c - 自定义协议

展示如何注册和使用自定义 URL 协议。

**功能：**
- 注册自定义协议处理器
- 处理自定义协议请求
- 发送自定义响应

**运行：**
```bash
make custom_protocol
LD_LIBRARY_PATH=../../target/release ./custom_protocol
```

**注意：** 这个示例需要完整的自定义协议 C API 支持。

### 4. full_features.c - 完整功能演示

展示 Wry C API 的所有主要功能。

**功能：**
- WebContext 创建和管理
- 完整的 WebViewBuilder 配置
- IPC 和导航处理
- 所有主要 API 的使用

**运行：**
```bash
make full_features
LD_LIBRARY_PATH=../../target/release ./full_features
```

## 编译步骤

### 1. 编译 wry 库

```bash
cd ../..
cargo build --features cabi --release
```

### 2. 编译示例

```bash
cd examples/c_api
make
```

### 3. 运行示例

确保库文件在路径中：

```bash
export LD_LIBRARY_PATH=../../target/release:$LD_LIBRARY_PATH
./simple
```

## 平台特定说明

### Windows

**编译：**
```cmd
cl simple.c /I..\..\include /link /LIBPATH:..\..\..\target\release libwry.lib
```

**运行：**
```cmd
set PATH=..\..\..\target\release;%PATH%
simple.exe
```

### Linux

**依赖：**
```bash
sudo apt-get install webkit2gtk-4.1-dev  # Ubuntu/Debian
sudo pacman -S webkit2gtk-4.1            # Arch Linux
```

**编译和运行：**
```bash
gcc -o simple simple.c -I../../include \
    -L../../target/release -lwry \
    $(pkg-config --libs gtk+-3.0 webkit2gtk-4.1)
export LD_LIBRARY_PATH=../../target/release:$LD_LIBRARY_PATH
./simple
```

### macOS

**编译和运行：**
```bash
gcc -o simple simple.c -I../../include \
    -L../../target/release -lwry \
    -framework AppKit -framework WebKit
export DYLD_LIBRARY_PATH=../../target/release:$DYLD_LIBRARY_PATH
./simple
```

## 代码讲解

### 初始化和清理

```c
// 初始化 Wry（Linux 必需）
if (wry_init() != WRY_OK) {
    fprintf(stderr, "Failed to initialize\n");
    return 1;
}

// 清理
wry_clear_last_error();
```

### 创建 WebView

```c
// 创建 builder
WryWebViewBuilder* builder = wry_builder_new();

// 配置
wry_builder_with_url(builder, "https://example.com");
wry_builder_with_devtools(builder, 1);

// 构建（需要窗口句柄）
WryWebView* webview = NULL;
// wry_builder_build(builder, window_handle, &webview);

// 清理
wry_builder_destroy(builder);
wry_webview_destroy(webview);
```

### IPC 通信

```c
// 定义回调
void on_message(const char* msg, size_t len, void* data) {
    printf("Received: %.*s\n", (int)len, msg);
}

// 设置处理器
wry_builder_with_ipc_handler(builder, on_message, NULL);
```

### 错误处理

```c
WryResult result = wry_webview_load_url(webview, url);
if (result != WRY_OK) {
    const char* error = wry_last_error_message();
    fprintf(stderr, "Error: %s\n", error);
}
```

## 常见问题

### Q: 编译时找不到 wry.h

A: 确保头文件在 include 目录中，并正确指定包含路径：
```bash
gcc -I../../include ...
```

### Q: 运行时找不到库

A: 确保库在正确的路径：
```bash
export LD_LIBRARY_PATH=../../target/release:$LD_LIBRARY_PATH
```

### Q: 如何获取窗口句柄？

A: 窗口句柄是平台特定的。参考平台的窗口创建 API：
- Windows: `HWND` from Win32 API
- Linux: `GtkWindow*` from GTK
- macOS: `NSWindow*` from AppKit

### Q: 回调函数需要什么签名？

A: 参考 `wry.h` 中的类型定义：
```c
typedef void (*WryIpcCallback)(const char* message, size_t len, void* userdata);
```

## 下一步

1. 查看 [C API 参考文档](C_API.md)
2. 参考实际的示例代码
3. 阅读 wry 的 Rust 文档了解底层功能
4. 实现自己的应用

## 贡献

如果发现示例有误或需要改进，请提交 issue 或 pull request。


