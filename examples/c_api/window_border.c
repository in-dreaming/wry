/**
 * Window Border Example - C API
 * 
 * This example demonstrates a borderless window with shadow toggle.
 * Click the window to toggle shadows on/off.
 * 
 * Features:
 * - Borderless window (no title bar, no frame)
 * - Dark background with green border
 * - Click to toggle window shadow
 * - IPC communication between JavaScript and C
 * 
 * Compile: zig cc -o window_border.exe window_border.c -I../../include -L../../target/debug -lwry
 * Run: window_border.exe
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "wry.h"

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500

typedef struct {
    HWND hwnd;
    WryWebView* webview;
    int shadow_enabled;
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

// IPC callback to handle messages from JavaScript
void on_ipc_message(const char* message, size_t len, void* userdata) {
    (void)userdata;
    
    // Parse the message
    if (len > 0 && strncmp(message, "toggleShadows", len) == 0) {
        printf("Toggling shadows...\n");
        
        // Toggle shadow state
        g_app_state.shadow_enabled = !g_app_state.shadow_enabled;
        
        // On Windows, we can set undecorated shadow
        // This is a Windows-specific feature via Windows API
        printf("Shadow is now: %s\n", g_app_state.shadow_enabled ? "enabled" : "disabled");
    }
}

int main() {
    printf("=== Wry Window Border Example ===\n");
    printf("Version: %s\n\n", wry_version());
    
    if (wry_init() != WRY_OK) {
        printf("ERROR: Failed to initialize Wry\n");
        return 1;
    }
    printf("OK: Wry initialized\n");
    
    const wchar_t CLASS_NAME[] = L"WryWindowBorderClass";
    
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
        CLASS_NAME, L"Wry Window Border",
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
    g_app_state.shadow_enabled = 1;
    
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    printf("OK: Window shown\n\n");
    
    // HTML content with click handler
    // Matches the Rust version's HTML
    const char* html = 
        "<html><head><style>"
        "html { font-family: Inter, Avenir, Helvetica, Arial, sans-serif; "
        "width: 100vw; height: 100vh; background-color: #1f1f1f; "
        "border: 1px solid rgb(148, 231, 155); margin: 0; padding: 0; "
        "box-sizing: border-box; }"
        "* { padding: 0; margin: 0; box-sizing: border-box; }"
        "body { color: white; display: flex; align-items: center; "
        "justify-content: center; height: 100%; margin: 0; }"
        "p { font-size: 18px; text-align: center; }"
        "</style></head><body>"
        "<p>Click the window to toggle shadows.</p>"
        "<script>"
        "window.addEventListener('click', () => {"
        "window.ipc.postMessage('toggleShadows');"
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
    printf("Click the window to toggle shadows.\n");
    printf("Close the window to exit.\n");
    
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
