/**
 * Custom Titlebar Example - C API
 * 
 * This example demonstrates a custom titlebar with minimize, maximize, and close buttons.
 * The titlebar is implemented in HTML/CSS/JavaScript.
 * 
 * Features:
 * - Borderless window (no system titlebar)
 * - Custom HTML/CSS titlebar with control buttons
 * - Window control via IPC (minimize, maximize, close)
 * - Window dragging from titlebar
 * 
 * Compile: zig cc -o custom_titlebar.exe custom_titlebar.c -I../../include -L../../target/debug -lwry
 * Run: custom_titlebar.exe
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "wry.h"

#define WINDOW_WIDTH 1240
#define WINDOW_HEIGHT 720

typedef struct {
    HWND hwnd;
    WryWebView* webview;
} AppState;

AppState g_app_state = {0};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
}

// IPC callback to handle window control messages
void on_ipc_message(const char* message, size_t len, void* userdata) {
    (void)userdata;
    
    if (len == 0 || !message) return;
    
    // Parse the message
    if (strncmp(message, "minimize", len) == 0) {
        printf("Minimizing window\n");
        ShowWindow(g_app_state.hwnd, SW_MINIMIZE);
    }
    else if (strncmp(message, "maximize", len) == 0) {
        printf("Toggling maximize\n");
        WINDOWPLACEMENT wp;
        wp.length = sizeof(WINDOWPLACEMENT);
        GetWindowPlacement(g_app_state.hwnd, &wp);
        
        if (wp.showCmd == SW_MAXIMIZE) {
            ShowWindow(g_app_state.hwnd, SW_RESTORE);
        } else {
            ShowWindow(g_app_state.hwnd, SW_MAXIMIZE);
        }
    }
    else if (strncmp(message, "close", len) == 0) {
        printf("Closing window\n");
        PostQuitMessage(0);
    }
    else if (strncmp(message, "drag_window", len) == 0) {
        printf("Dragging window\n");
        // Simulate window drag
        ReleaseCapture();
        SendMessageW(g_app_state.hwnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
    }
}

int main() {
    printf("=== Wry Custom Titlebar Example ===\n");
    printf("Version: %s\n\n", wry_version());
    
    if (wry_init() != WRY_OK) {
        printf("ERROR: Failed to initialize Wry\n");
        return 1;
    }
    printf("OK: Wry initialized\n");
    
    const wchar_t CLASS_NAME[] = L"WryCustomTitlebarClass";
    
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
    wc.hbrBackground = NULL;  // No background
    
    if (!RegisterClassW(&wc)) {
        printf("ERROR: Failed to register window class\n");
        return 1;
    }
    printf("OK: Window class registered\n");
    
    // Create borderless window (WS_POPUP)
    // This matches the Rust version with decorations(false)
    HWND hwnd = CreateWindowExW(
        0,  // No extended styles
        CLASS_NAME, L"Wry Custom Titlebar",
        WS_POPUP | WS_VISIBLE,  // Borderless window
        100, 100,  // Position
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, NULL, NULL
    );
    
    if (!hwnd) {
        printf("ERROR: Failed to create window\n");
        return 1;
    }
    printf("OK: Window created (borderless)\n");
    
    g_app_state.hwnd = hwnd;
    
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    printf("OK: Window shown\n\n");
    
    // HTML content with custom titlebar
    const char* html = 
        "<html><head><style>"
        "* { padding: 0; margin: 0; box-sizing: border-box; }"
        "html, body { width: 100%; height: 100%; font-family: Arial, sans-serif; }"
        ".titlebar { height: 30px; padding-left: 5px; display: grid; "
        "grid-auto-flow: column; grid-template-columns: 1fr max-content max-content max-content; "
        "align-items: center; background: #1F1F1F; color: white; user-select: none; "
        "-webkit-user-select: none; }"
        ".titlebar-title { cursor: move; padding: 0 10px; }"
        ".titlebar-buttons { display: flex; height: 30px; }"
        ".titlebar-button { display: inline-flex; justify-content: center; align-items: center; "
        "width: 30px; height: 30px; cursor: pointer; background: #1F1F1F; color: white; "
        "border: none; font-size: 16px; }"
        ".titlebar-button:hover { background: #3b3b3b; }"
        ".titlebar-button#close:hover { background: #da3d3d; }"
        "main { display: grid; place-items: center; height: calc(100vh - 30px); "
        "background: #f5f5f5; }"
        "h1 { color: #333; font-size: 32px; }"
        "</style></head><body>"
        "<div class=\"titlebar\">"
        "<div class=\"titlebar-title\">Custom Titlebar</div>"
        "<div class=\"titlebar-buttons\">"
        "<button class=\"titlebar-button\" onclick=\"window.ipc.postMessage('minimize')\">_</button>"
        "<button class=\"titlebar-button\" onclick=\"window.ipc.postMessage('maximize')\">□</button>"
        "<button class=\"titlebar-button\" id=\"close\" onclick=\"window.ipc.postMessage('close')\">✕</button>"
        "</div></div>"
        "<main><h1>WRYYYYYYYYYYYYYYYYYYYYYY!</h1></main>"
        "<script>"
        "document.addEventListener('mousemove', (e) => {"
        "window.ipc.postMessage(`mousemove:${e.clientX},${e.clientY}`);"
        "});"
        "document.addEventListener('mousedown', (e) => {"
        "if (e.target.classList.contains('titlebar-title') && e.button === 0) {"
        "if (e.detail === 2) {"
        "window.ipc.postMessage('maximize');"
        "} else {"
        "window.ipc.postMessage('drag_window');"
        "}"
        "} else {"
        "window.ipc.postMessage(`mousedown:${e.clientX},${e.clientY}`);"
        "}"
        "});"
        "</script>"
        "</body></html>";
    
    // Create WebView
    printf("Creating WebView...\n");
    WryWebViewBuilder* builder = wry_builder_new();
    if (!builder) {
        printf("ERROR: Failed to create builder\n");
        return 1;
    }
    
    // Set HTML content
    if (wry_builder_with_html(builder, html) != WRY_OK) {
        printf("ERROR: Failed to set HTML: %s\n", wry_last_error_message());
        wry_builder_destroy(builder);
        return 1;
    }
    
    // Set IPC handler
    if (wry_builder_with_ipc_handler(builder, on_ipc_message, NULL) != WRY_OK) {
        printf("ERROR: Failed to set IPC handler: %s\n", wry_last_error_message());
        wry_builder_destroy(builder);
        return 1;
    }
    
    wry_builder_with_devtools(builder, 1);
    wry_builder_with_visible(builder, 1);
    
    // Build WebView
    WryWebView* webview = wry_builder_build_as_child(builder, (void*)hwnd);
    if (!webview) {
        printf("ERROR: Failed to build WebView: %s\n", wry_last_error_message());
        wry_builder_destroy(builder);
        return 1;
    }
    
    g_app_state.webview = webview;
    printf("OK: WebView created successfully\n\n");
    printf("Custom titlebar is ready.\n");
    printf("Click the buttons to minimize, maximize, or close the window.\n");
    printf("Drag the titlebar to move the window.\n");
    
    // Message loop
    MSG msg = {0};
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    
    printf("\nCleaning up...\n");
    if (g_app_state.webview) {
        wry_webview_destroy(g_app_state.webview);
        printf("OK: WebView destroyed\n");
    }
    
    printf("Done!\n");
    return 0;
}
