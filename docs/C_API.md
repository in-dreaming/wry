# Wry C API 参考文档

## 概述

Wry C API 提供了对 wry WebView 库的完整 C 语言绑定，支持 Windows、Linux 和 macOS。

## 基本概念

### 错误处理

所有 C API 函数返回 `WryResult`（实际上是 `WryErrorCode`）。成功时返回 `WRY_OK`，失败时返回相应的错误代码。可以使用 `wry_last_error_message()` 获取错误消息。

```c
WryResult result = wry_webview_load_url(webview, url);
if (result != WRY_OK) {
    const char* error = wry_last_error_message();
    fprintf(stderr, "Error: %s\n", error);
}
```

### 内存管理

- 所有字符串返回值需要使用 `wry_string_free()` 释放
- 所有对象（WebView、Builder、Context）需要使用对应的 `wry_*_destroy()` 函数释放
- 回调的 userdata 由调用方管理生命周期

### 线程安全

C API 函数本身是线程安全的，但 WebView 操作应该在主线程上进行。

## API 参考

### 初始化和版本

#### wry_version()

获取 wry 库的版本号。

```c
const char* wry_version();
```

**返回值：** 版本字符串指针（不需要释放）

**示例：**
```c
printf("Wry version: %s\n", wry_version());
```

#### wry_init()

初始化 wry 库。在 Linux 上这是必需的（初始化 GTK）。

```c
WryResult wry_init();
```

**返回值：**
- `WRY_OK` - 成功
- `WRY_ERROR_INIT_FAILED` - 初始化失败

#### wry_clear_last_error()

清除最后一个错误消息。

```c
void wry_clear_last_error();
```

### WebContext API

#### wry_context_new()

创建新的 WebContext。

```c
WryWebContext* wry_context_new(const char* data_dir);
```

**参数：**
- `data_dir`: 数据目录路径，可以为 NULL

**返回值：** WebContext 指针，失败返回 NULL

**示例：**
```c
WryWebContext* ctx = wry_context_new("/path/to/data");
if (!ctx) {
    // Handle error
}
```

#### wry_context_destroy()

销毁 WebContext。

```c
void wry_context_destroy(WryWebContext* context);
```

#### wry_context_set_allows_automation()

设置是否允许自动化。

```c
WryResult wry_context_set_allows_automation(WryWebContext* context, int flag);
```

### WebViewBuilder API

#### wry_builder_new()

创建新的 WebViewBuilder。

```c
WryWebViewBuilder* wry_builder_new();
```

**返回值：** Builder 指针，失败返回 NULL

#### wry_builder_with_url()

设置要加载的 URL。

```c
WryResult wry_builder_with_url(WryWebViewBuilder* builder, const char* url);
```

**示例：**
```c
wry_builder_with_url(builder, "https://example.com");
```

#### wry_builder_with_html()

设置要加载的 HTML 内容。

```c
WryResult wry_builder_with_html(WryWebViewBuilder* builder, const char* html);
```

#### wry_builder_with_devtools()

设置是否启用开发者工具。

```c
WryResult wry_builder_with_devtools(WryWebViewBuilder* builder, int enable);
```

#### wry_builder_with_ipc_handler()

设置 IPC 消息处理器。

```c
WryResult wry_builder_with_ipc_handler(
    WryWebViewBuilder* builder,
    void (*callback)(const char* message, size_t len, void* userdata),
    void* userdata
);
```

**回调函数签名：**
```c
void ipc_callback(const char* message, size_t len, void* userdata) {
    // Process message
}
```

#### wry_builder_build()

构建 WebView（需要窗口句柄）。

```c
WryResult wry_builder_build(
    WryWebViewBuilder* builder,
    void* window_handle,
    WryWebView** out_webview
);
```

### WebView API

#### wry_webview_load_url()

加载指定的 URL。

```c
WryResult wry_webview_load_url(WryWebView* webview, const char* url);
```

#### wry_webview_eval()

执行 JavaScript 代码。

```c
WryResult wry_webview_eval(WryWebView* webview, const char* javascript);
```

**示例：**
```c
wry_webview_eval(webview, "console.log('Hello from C!')");
```

#### wry_webview_reload()

重新加载当前页面。

```c
WryResult wry_webview_reload(WryWebView* webview);
```

#### wry_webview_open_devtools()

打开开发者工具。

```c
void wry_webview_open_devtools(WryWebView* webview);
```

#### wry_webview_close_devtools()

关闭开发者工具。

```c
void wry_webview_close_devtools(WryWebView* webview);
```

## 完整示例

```c
#include "wry.h"
#include <stdio.h>

void on_message(const char* msg, size_t len, void* data) {
    printf("Message: %.*s\n", (int)len, msg);
}

int main() {
    // Initialize
    wry_init();
    
    // Create builder
    WryWebViewBuilder* builder = wry_builder_new();
    wry_builder_with_url(builder, "https://www.rust-lang.org");
    wry_builder_with_devtools(builder, 1);
    wry_builder_with_ipc_handler(builder, on_message, NULL);
    
    // Create webview (requires window handle)
    WryWebView* webview = NULL;
    // wry_builder_build(builder, window_handle, &webview);
    
    // Clean up
    wry_builder_destroy(builder);
    
    return 0;
}
```

## 平台特定说明

### Windows

使用 Windows.h 获取窗口句柄：

```c
#include <windows.h>

HWND hwnd = CreateWindow(...);
```

### Linux

需要先初始化 GTK：

```c
#include <gtk/gtk.h>

gtk_init(NULL, NULL);
wry_init();

GtkWindow* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
```

### macOS

使用 AppKit：

```c
#import <Cocoa/Cocoa.h>

NSWindow* window = [[NSWindow alloc] init];
```

## 错误代码

- `WRY_OK` (0) - 成功
- `WRY_ERROR_GENERIC` (-1) - 通用错误
- `WRY_ERROR_INVALID_ARGUMENT` (-2) - 无效参数
- `WRY_ERROR_INVALID_STATE` (-3) - 无效状态
- `WRY_ERROR_INIT_FAILED` (-4) - 初始化失败
- `WRY_ERROR_SCRIPT` (-7) - 脚本错误
- `WRY_ERROR_IO` (-9) - IO 错误

## 线程安全

C API 函数本身是线程安全的，但建议在主线程上操作 WebView。


