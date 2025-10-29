/* C API for Wry WebView library
 * 
 * This file contains the C API bindings for Wry WebView.
 * For more information, see docs/C_API.md
 */

#ifndef WRY_H
#define WRY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/* =============== 错误代码 =============== */

typedef int32_t WryResult;

#define WRY_OK 0
#define WRY_ERROR_GENERIC -1
#define WRY_ERROR_INVALID_ARGUMENT -2
#define WRY_ERROR_INVALID_STATE -3
#define WRY_ERROR_INIT_FAILED -4
#define WRY_ERROR_GLIB -5
#define WRY_ERROR_X11 -6
#define WRY_ERROR_SCRIPT -7
#define WRY_ERROR_RPC -8
#define WRY_ERROR_IO -9
#define WRY_ERROR_WEBVIEW2 -10
#define WRY_ERROR_HTTP -11
#define WRY_ERROR_WINDOW_HANDLE -12
#define WRY_ERROR_UNSUPPORTED -13
#define WRY_ERROR_MEMORY -14
#define WRY_ERROR_PROTOCOL -15

/* =============== 类型定义 =============== */

typedef struct WryRect {
    double x;
    double y;
    double width;
    double height;
} WryRect;

typedef struct WryRGBA {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} WryRGBA;

typedef struct WryWebView WryWebView;
typedef struct WryWebViewBuilder WryWebViewBuilder;
typedef struct WryWebContext WryWebContext;

/* =============== 初始化和版本 =============== */

/**
 * 获取 Wry 版本号
 * @return 版本字符串指针（不需要释放）
 */
const char* wry_version();

/**
 * 初始化 Wry（在 Linux 上必需）
 * @return 错误代码
 */
WryResult wry_init();

/**
 * 获取最后一个错误消息
 * @return 错误消息字符串
 */
const char* wry_last_error_message();

/**
 * 清除最后一个错误
 */
void wry_clear_last_error();

/* =============== 内存管理 =============== */

/**
 * 释放字符串
 * @param ptr 字符串指针
 */
void wry_string_free(char* ptr);

/* =============== WebContext API =============== */

/**
 * 创建新的 WebContext
 * @param data_dir 数据目录路径（可以为 NULL）
 * @return WebContext 指针，失败返回 NULL
 */
WryWebContext* wry_context_new(const char* data_dir);

/**
 * 销毁 WebContext
 * @param context WebContext 指针
 */
void wry_context_destroy(WryWebContext* context);

/**
 * 设置是否允许自动化
 * @param context WebContext 指针
 * @param flag 是否允许（非 0 为 true）
 * @return 错误代码
 */
WryResult wry_context_set_allows_automation(WryWebContext* context, int flag);

/**
 * 检查协议是否已注册
 * @param context WebContext 指针
 * @param name 协议名称
 * @return 如果已注册返回非 0，否则返回 0
 */
int wry_context_is_protocol_registered(WryWebContext* context, const char* name);

/* =============== WebViewBuilder API =============== */

/**
 * 创建新的 WebViewBuilder
 * @return Builder 指针，失败返回 NULL
 */
WryWebViewBuilder* wry_builder_new();

/**
 * 用 WebContext 创建 WebViewBuilder
 * @param context WebContext 指针
 * @return Builder 指针，失败返回 NULL
 */
WryWebViewBuilder* wry_builder_new_with_context(WryWebContext* context);

/**
 * 设置要加载的 URL
 */
WryResult wry_builder_with_url(WryWebViewBuilder* builder, const char* url);

/**
 * 设置要加载的 HTML 内容
 */
WryResult wry_builder_with_html(WryWebViewBuilder* builder, const char* html);

/**
 * 设置透明度
 */
WryResult wry_builder_with_transparent(WryWebViewBuilder* builder, int transparent);

/**
 * 设置可见性
 */
WryResult wry_builder_with_visible(WryWebViewBuilder* builder, int visible);

/**
 * 设置开发者工具
 */
WryResult wry_builder_with_devtools(WryWebViewBuilder* builder, int devtools);

/**
 * 设置用户代理
 */
WryResult wry_builder_with_user_agent(WryWebViewBuilder* builder, const char* user_agent);

/**
 * 设置 IPC 消息处理器
 */
WryResult wry_builder_with_ipc_handler(
    WryWebViewBuilder* builder,
    void (*callback)(const char* message, size_t len, void* userdata),
    void* userdata
);

/**
 * 设置导航处理器
 */
WryResult wry_builder_with_navigation_handler(
    WryWebViewBuilder* builder,
    int (*callback)(const char* url, void* userdata),
    void* userdata
);

/**
 * 设置 WebView 边界（在构建前）
 */
WryResult wry_builder_with_bounds(
    WryWebViewBuilder* builder,
    double x,
    double y,
    double width,
    double height
);

/**
 * 构建 WebView 作为子窗口
 * @param builder WebViewBuilder 指针
 * @param parent_hwnd 父窗口句柄（HWND on Windows, GtkWidget* on Linux, NSView* on macOS）
 * @return WebView 指针，失败返回 NULL
 */
WryWebView* wry_builder_build_as_child(WryWebViewBuilder* builder, void* parent_hwnd);

/**
 * 销毁 Builder
 */
void wry_builder_destroy(WryWebViewBuilder* builder);

/* =============== WebView API =============== */

/**
 * 销毁 WebView
 */
void wry_webview_destroy(WryWebView* webview);

/**
 * 获取 WebView ID
 */
WryResult wry_webview_id(const WryWebView* webview, char* out_id, size_t len);

/**
 * 获取当前 URL
 */
WryResult wry_webview_url(const WryWebView* webview, char** out_url);

/**
 * 执行 JavaScript 代码
 */
WryResult wry_webview_eval(WryWebView* webview, const char* js);

/**
 * 加载 URL
 */
WryResult wry_webview_load_url(WryWebView* webview, const char* url);

/**
 * 加载 HTML
 */
WryResult wry_webview_load_html(WryWebView* webview, const char* html);

/**
 * 重新加载
 */
WryResult wry_webview_reload(WryWebView* webview);

/**
 * 打印
 */
WryResult wry_webview_print(WryWebView* webview);

/**
 * 设置缩放
 */
WryResult wry_webview_zoom(WryWebView* webview, double scale);

/**
 * 设置可见性
 */
WryResult wry_webview_set_visible(WryWebView* webview, int visible);

/**
 * 获取焦点
 */
WryResult wry_webview_focus(WryWebView* webview);

/**
 * 打开开发者工具
 */
void wry_webview_open_devtools(WryWebView* webview);

/**
 * 关闭开发者工具
 */
void wry_webview_close_devtools(WryWebView* webview);

/**
 * 检查开发者工具是否打开
 * @return 如果打开返回非 0，否则返回 0
 */
int wry_webview_is_devtools_open(const WryWebView* webview);

/**
 * 设置 WebView 边界（位置和大小）
 * 仅对使用 build_as_child 创建的 WebView 有效
 */
WryResult wry_webview_set_bounds(WryWebView* webview, const WryRect* bounds);

#ifdef __cplusplus
}
#endif

#endif /* WRY_H */


